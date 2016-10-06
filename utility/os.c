#include <assert.h>
#include <stdlib.h>

#include <tinydir/tinydir.h>

#include "violet/utility/os.h"

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

	void *vp = NULL;
	if (!SUCCEEDED(CoCreateInstance(&clsid, NULL, CLSCTX_ALL, &iid, &vp)))
		goto err_init;
	IFileDialog *dialog = vp;

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

b8 dir_exists(const char *path)
{
	const DWORD attrib = GetFileAttributes(path);
	return attrib != INVALID_FILE_ATTRIBUTES
	    && (attrib & FILE_ATTRIBUTE_DIRECTORY);
}

void path_append(char *lhs, const char *rhs)
{
	strcat(lhs, "\\");
	strcat(lhs, rhs);
}

void path_appendn(char *lhs, const char *rhs, u32 sz)
{
	strcat(lhs, "\\");
	strncat(lhs, rhs, sz);
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

lib_handle lib_load(const char *_filename)
{
	const u32 sz = strlen(_filename);
	char *filename = malloc(sz+4);
	strcpy(filename, _filename);
	strcat(filename, ".dll");
	lib_handle hnd = LoadLibrary(filename);
	free(filename);
	return hnd;
}

void *lib_func(lib_handle hnd, const char *name)
{
	return GetProcAddress(hnd, name);
}

b8 lib_close(lib_handle hnd)
{
	return FreeLibrary(hnd);
}

const char *lib_err()
{
	return GetLastError();
}

#else

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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

b8 dir_exists(const char *path)
{
	struct stat s;
	return stat(path, &s) == 0 && S_ISDIR(s.st_mode);
}

void path_append(char *lhs, const char *rhs)
{
	strcat(lhs, "/");
	strcat(lhs, rhs);
}

void path_appendn(char *lhs, const char *rhs, u32 sz)
{
	strcat(lhs, "/");
	strncat(lhs, rhs, sz);
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

lib_handle lib_load(const char *_filename)
{
	const u32 sz = strlen(_filename);
	char *filename = malloc(sz+4);
	strcpy(filename, _filename);
	strcat(filename, ".so");
	lib_handle hnd = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
	free(filename);
	return hnd;
}

void *lib_func(lib_handle hnd, const char *name)
{
	return dlsym(hnd, name);
}

b8 lib_close(lib_handle hnd)
{
	return dlclose(hnd) == 0;
}

const char *lib_err()
{
	return dlerror();
}

void exec(char *const argv[])
{
	execv(argv[0], argv);
}

#endif

b8 mkpath(const char *path)
{
	char _path[256];
	if (strlen(path) > sizeof(_path))
		return false;

	if (   path[0] == '.'
	    && (path[1] == '/' || path[1] == '\\'))
		strcpy(_path, path + 2);
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

#ifdef VLT_USE_TINYDIR
b8 rmdir_f(const char *path)
{
	b8 success = false;
	tinydir_dir dir;
	if (tinydir_open(&dir, path) == -1)
	{
		printf("rmdir: error reading directory '%s'\n", path);
		goto out;
	}
	while (dir.has_next)
	{
		tinydir_file file;
		if (tinydir_readfile(&dir, &file) != -1)
		{
			if (   strcmp(file.name, ".") != 0
			    && strcmp(file.name, "..") != 0)
			{
				if (file.is_dir)
					rmdir_f(file.path);
				else if (remove(file.path))
				{
					printf("rmdir: error removing '%s'", file.path);
					goto out;
				}
			}
		}
		else
		{
			printf("rmdir: error examining file\n");
			goto out;
		}
		if (tinydir_next(&dir) == -1)
		{
			printf("rmdir: error iterating directory\n");
			goto out;
		}
	}
	if (remove(path))
		printf("error removing '%s'\n", path);
	else
		success = true;
out:
	tinydir_close(&dir);
	return success;
}
#endif // VLT_USE_TINYDIR

