#include <assert.h>
#include <stdlib.h>

#include <tinydir/tinydir.h>

#include "violet/utility/log.h"
#include "violet/utility/os.h"

#ifdef _WIN32

#include "violet/core/windows.h"

#include <process.h>
#include <ShlObj.h>
#include <shobjidl.h>
#include <stdio.h>

#define execv _execv
#define popen _popen
#define pclose _pclose
#define WIFEXITED(x) 1
#define WEXITSTATUS(x) (x)

static
SSIZE_T getdelim(char **lineptr, size_t *n, int delim, FILE *stream)
{
	if (!lineptr || !n || !stream)
	{
		errno = EINVAL;
		return -1;
	}

	if (!*lineptr)
	{
		*n = 32;
		*lineptr = malloc(*n);
	}
	else if (*n == 0)
	{
		errno = EINVAL;
		return -1;
	}

	int i = 0, c;
	while ((c = fgetc(stream)) != EOF)
	{
		if (i == *n-1)
		{
			*n *= 2;
			*lineptr = realloc(*lineptr, *n);
		}
		(*lineptr)[i++] = c;
		if (c == delim)
			break;
	}

	if (ferror(stream) || feof(stream))
		return -1;

	(*lineptr)[i] = '\0';
	return i;
}

static
SSIZE_T getline(char **lineptr, size_t *n, FILE *stream)
{
	return getdelim(lineptr, n, '\n', stream);
}

static
int rmdir(const char *path)
{
	return !RemoveDirectory(path);
}

static
b8 _file_dialog(char *filename, u32 n, const char *ext[], u32 n_ext,
                CLSID clsid, IID iid)
{
	b8 retval = false;
	PWSTR ext_buf = NULL;
	COMDLG_FILTERSPEC filters = NULL;

	if (!SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)))
		goto out;

	void *vp = NULL;
	if (!SUCCEEDED(CoCreateInstance(&clsid, NULL, CLSCTX_ALL, &iid, &vp)))
		goto err_init;
	IFileDialog *dialog = vp;

	if (n_ext)
	{
		ext_buf = malloc(8*(n_ext+1)*sizeof(wchar_t));
		mbstowcs(ext_buf, ext[0], 8);
		if (!SUCCEEDED(dialog->lpVtbl->SetDefaultExtension(dialog, psz_ext)))
			goto err_ext;

		filters = malloc(n_ext*sizeof(COMDLG_FILTERSPEC));
		for (u32 i = 0; i < ext_n; ++i)
		{
			filters[i].pszName = L"";
			filters[i].pszSpec = ext_buf+(i+1)*8;
			filters[i].pszSpec[0] = L'*';
			filters[i].pszSpec[1] = L'.';
			mbstowcs(filters[i].pszSpec+2, ext[i], 6);
		}
		if (!SUCCEEDED(dialog->lpVtbl->SetFileTypes(dialog, ext_n, filters)))
			goto err_ext;
	}

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
	free(filters);
	free(ext_buf);
err_init:
	CoUninitialize();
out:
	return retval;
}

b8 file_open_dialog(char *fname, u32 n, const char *ext)
{
	const char *extensions[1] = { ext };
	file_open_dialog_ex(fname, n, ext, ext ? 1 : 0);
}

b8 file_save_dialog(char *fname, u32 n, const char *ext)
{
	const char *extensions[1] = { ext };
	file_save_dialog_ex(fname, n, ext, ext ? 1 : 0);
}

b8 file_open_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext)
{
	return _file_dialog(fname, n, ext, n_ext, CLSID_FileOpenDialog,
		IID_IFileOpenDialog);
}

b8 file_save_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext)
{
	return _file_dialog(fname, n, ext, n_ext, CLSID_FileSaveDialog,
		IID_IFileSaveDialog);
}

b8 file_exists(const char *path)
{
	const DWORD attrib = GetFileAttributes(path);
	return attrib != INVALID_FILE_ATTRIBUTES
	    && !(attrib & FILE_ATTRIBUTE_DIRECTORY);
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
	static char buf[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
	return buf;
}

#else

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

b8 _file_dialog(char *filename, u32 n, const char *cmd)
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

b8 file_open_dialog(char *fname, u32 n, const char *ext)
{
	return _file_dialog(fname, n, "zenity --file-selection");
}

b8 file_save_dialog(char *fname, u32 n, const char *ext)
{
	return _file_dialog(fname, n, "zenity --file-selection --save");
}

b8 file_open_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext)
{
	return file_open_dialog(fname, n, NULL);
}

b8 file_save_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext)
{
	return file_save_dialog(fname, n, NULL);
}

b8 file_exists(const char *path)
{
	struct stat s;
	return stat(path, &s) == 0 && !S_ISDIR(s.st_mode);
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
		log_write("rmdir: error reading directory '%s'", path);
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
					log_write("rmdir: error removing '%s'", file.path);
					goto out;
				}
			}
		}
		else
		{
			log_write("rmdir: error examining file");
			goto out;
		}
		if (tinydir_next(&dir) == -1)
		{
			log_write("rmdir: error iterating directory");
			goto out;
		}
	}
	if (rmdir(path))
		log_write("error removing '%s'", path);
	else
		success = true;
out:
	tinydir_close(&dir);
	return success;
}
#endif // VLT_USE_TINYDIR

void exec(char *const argv[])
{
	execv(argv[0], argv);
}

int run(const char *command)
{
	FILE *fp = popen(command, "r");
	if (!fp)
	{
		log_write("failed to execute %s", command);
		return -1;
	}
	char *log_buf = NULL;
	size_t log_buf_sz;
	while ((log_buf_sz = getline(&log_buf, &log_buf_sz, fp)) != -1)
	{
		assert(log_buf_sz != 0);
		if (log_buf[log_buf_sz - 1] == '\n')
			log_buf[log_buf_sz - 1] = '\0';
		log_write(log_buf);
	}
	free(log_buf);
	int status = pclose(fp);
	return status != -1 && WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}
