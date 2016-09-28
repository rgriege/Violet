#include "violet/utility/os.h"

#include <assert.h>

#ifdef _WIN32

#include "violet/core/windows.h"

#include <ShlObj.h>
#include <shobjidl.h>
#include <stdio.h>

static
b8 _vlt_file_dialog(char *filename, u32 n, const char *ext, CLSID clsid, IID iid)
{
	b8 retval = false;

	if (!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
		goto out;

	IFileDialog *dialog = NULL;
	if (!SUCCEEDED(CoCreateInstance(&clsid, NULL, CLSCTX_ALL, &iid, &dialog)))
		goto err_init;

	PWSTR psz_ext = malloc(sizeof(wchar_t)*n);
	mbstowcs(psz_ext, ext, n);
	if (!SUCCEEDED(dialog->lpVtbl->SetDefaultExtension(dialog, psz_ext)))
		goto err_ext;

	COMDLG_FILTERSPEC filters[1] = { { L"", psz_ext } };
	if (!SUCCEEDED(dialog->lpVtbl->SetFileTypes(dialog, 1, filters)))
		goto err_ext;

	if (!SUCCEEDED(dialog->lpVtbl->Show(dialog, NULL)))
		goto err_dlg;

	IShellItem *item;
	if (!SUCCEEDED(dialog->lpVtbl->GetResult(dialog, &item)))
		goto err_dlg;

	PWSTR psz_file_path;
	if (!SUCCEEDED(item->lpVtbl->GetDisplayName(item, SIGDN_FILESYSPATH, &psz_file_path)))
		goto err_itm;

	CoTaskMemFree(psz_file_path);
	wcstombs(filename, psz_file_path, n);
	retval = true;

err_itm:
	item->lpVtbl->Release(item);
err_dlg:
	dialog->lpVtbl->Release(dialog);
err_ext:
	free(psz_ext);
err_init:
	CoUninitialize();
out:
	return retval;
}

b8 vlt_file_open_dialog(char *filename, u32 n, const char *ext)
{
	return _vlt_file_dialog(filename, n, ext, CLSID_FileOpenDialog,
		IID_IFileOpenDialog);
}

b8 vlt_file_save_dialog(char *filename, u32 n, const char *ext)
{
	return _vlt_file_dialog(filename, n, ext, CLSID_FileSaveDialog,
		IID_IFileSaveDialog);
}

void path_append(char *lhs, const char *rhs)
{
	strcat(lhs, "\\");
	strcat(lhs, rhs);
}

static
b8 _mkdir(const char *path)
{
	// catch drive creation
	if (strlen(path) == 2 && path[1] == ':')
		return true;

	return CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

b8 app_data_dir(char *path, u32 n)
{
	FILE *fp = fopen(".access_check", "w");
	if (!fp)
	{
		PWSTR psz_file_path;
		if (SHGetKnownFolderPath(&FOLDERID_ProgramData, 0, NULL, &psz_file_path) != S_OK)
			return false;

		wcstombs(path, psz_file_path, n);
		CoTaskMemFree(psz_file_path);
		return true;
	}
	else
	{
		fclose(fp);
		remove(".access_check");
		path[0] = '.';
		path[1] = '\0';
		return true;
	}
}

#else

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

b8 _vlt_file_dialog(char *filename, u32 n, const char *cmd)
{
	b8 retval = false;
	FILE *pipe = popen(cmd, "r");
	if (!pipe)
		goto out;

	if (fgets(filename, n-1, pipe) != NULL)
	{
		// NOTE(rgriege): newline char added for some reason
		char *c = strchr(filename, '\n');
		if (c)
			*c = 0;
		retval = true;
	}

	pclose(pipe);
out:
	return retval;
}

b8 vlt_file_open_dialog(char *filename, u32 n, const char *ext)
{
	return _vlt_file_dialog(filename, n, "zenity --file-selection");
}

b8 vlt_file_save_dialog(char *filename, u32 n, const char *ext)
{
	return _vlt_file_dialog(filename, n,
		"zenity --file-selection --save");
}

void path_append(char *lhs, const char *rhs)
{
	strcat(lhs, "/");
	strcat(lhs, rhs);
}

static
b8 _mkdir(const char *path)
{
	return mkdir(path, S_IRWXU) == 0;
}

b8 app_data_dir(char *path, u32 n)
{
	FILE *fp = fopen(".access_check", "w");
	if (fp)
	{
		fclose(fp);
		remove(".access_check");
		path[0] = '.';
		path[1] = '\0';
		return true;
	}
	return false;
}

#endif

b8 mkpath(const char *path)
{
	char _path[256];
	if (strlen(path) > sizeof(_path))
		return false;

	if (path[0] == '.')
	{
		assert(path[1] == '/' || path[1] == '\\');
		strcpy(_path, path + 2);
	}
	else
		strcpy(_path, path);

	if (strlen(_path) == 0)
		return true;

	errno = 0;

	u32 pos = 0, dot_pos = 0;
	for (char *p = _path + 1; *p; ++p)
	{
		switch (*p)
		{
		case '/':
		case '\\':
		{
			char c = *p;
			*p = '\0';

			if (!_mkdir(_path) && errno != EEXIST)
				return false;

			*p = c;
			pos = 0;
		}
		break;
		case '.':
			dot_pos = pos;
		break;
		default:
			++pos;
		break;
		}
	}

	return dot_pos != 0 || _mkdir(_path) || errno == EEXIST;
}
