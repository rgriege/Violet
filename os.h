#ifndef VIOLET_OS_H
#define VIOLET_OS_H

#include "violet/core.h"

/* File system */

b32  file_open_dialog(char *fname, u32 n, const char *ext);
b32  file_save_dialog(char *fname, u32 n, const char *ext);
b32  file_open_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext);
b32  file_save_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext);

b32  file_exists(const char *path);
b32  dir_exists(const char *path);
void path_append(char *lhs, const char *rhs);
void path_appendn(char *lhs, const char *rhs, u32 sz);
b32  mkpath(const char *path);
char *app_data_dir(const char *app_name, allocator_t *a);
#ifdef VLT_USE_TINYDIR
b32  rmdir_f(const char *path);
#endif

/* Dynamic library */

#ifdef _WIN32
typedef HMODULE lib_handle;
#else
typedef void* lib_handle;
#endif

lib_handle  lib_load(const char *filename);
void       *lib_func(lib_handle hnd, const char *name);
b32         lib_close(lib_handle hnd);
const char *lib_err();

/* IO */

size_t vgetdelim(char **lineptr, size_t *n, int delim, FILE *stream, allocator_t *a);
size_t vgetline(char **lineptr, size_t *n, FILE *stream, allocator_t *a);

/* Other applications */

void exec(char *const argv[]);
int  run(const char *command);
b32  open_file_external(const char *filename);

#endif


/* Implementation */

#ifdef OS_IMPLEMENTATION

#include <stdlib.h>

#ifdef VLT_USE_TINYDIR
#define TINYDIR_IMPLEMENTATION
#include <tinydir/tinydir.h>
#endif

#ifdef _WIN32

#include <process.h>
#include <ShlObj.h>
#include <shobjidl.h>
#include <Shellapi.h>
#include <stdio.h>

#define execv _execv
#define popen _popen
#define pclose _pclose
#define WIFEXITED(x) 1
#define WEXITSTATUS(x) (x)

static
int rmdir(const char *path)
{
	return !RemoveDirectory(path);
}

static
b32 file__dialog(char *filename, u32 n, const char *ext[], u32 n_ext,
                 CLSID clsid, IID iid)
{
	b32 retval = false;
	PWSTR ext_buf = NULL;
	COMDLG_FILTERSPEC *filters = NULL;

	if (!SUCCEEDED(CoInitializeEx(NULL,   COINIT_APARTMENTTHREADED
	                                    | COINIT_DISABLE_OLE1DDE)))
		goto out;

	void *vp = NULL;
	if (!SUCCEEDED(CoCreateInstance(&clsid, NULL, CLSCTX_ALL, &iid, &vp)))
		goto err_init;
	IFileDialog *dialog = vp;

	if (n_ext) {
		ext_buf = amalloc(8*(n_ext+1)*sizeof(wchar_t), g_temp_allocator);
		mbstowcs(ext_buf, ext[0], 8);
		if (!SUCCEEDED(dialog->lpVtbl->SetDefaultExtension(dialog, ext_buf)))
			goto err_ext;

		filters = amalloc(n_ext*sizeof(COMDLG_FILTERSPEC), g_temp_allocator);
		for (u32 i = 0; i < n_ext; ++i) {
			filters[i].pszName = L"";
			PWSTR spec = ext_buf+(i+1)*8;
			spec[0] = L'*';
			spec[1] = L'.';
			mbstowcs(spec+2, ext[i], 6);
			filters[i].pszSpec = spec;
		}
		if (!SUCCEEDED(dialog->lpVtbl->SetFileTypes(dialog, n_ext, filters)))
			goto err_ext;
	}

	if (!SUCCEEDED(dialog->lpVtbl->Show(dialog, NULL)))
		goto err_dlg;

	IShellItem *item;
	if (!SUCCEEDED(dialog->lpVtbl->GetResult(dialog, &item)))
		goto err_dlg;

	PWSTR psz_file_path;
	if (!SUCCEEDED(item->lpVtbl->GetDisplayName(item, SIGDN_FILESYSPATH,
	                                            &psz_file_path)))
		goto err_itm;

	wcstombs(filename, psz_file_path, n);
	CoTaskMemFree(psz_file_path);
	retval = true;

err_itm:
	item->lpVtbl->Release(item);
err_dlg:
	dialog->lpVtbl->Release(dialog);
err_ext:
	afree(filters, g_temp_allocator);
	afree(ext_buf, g_temp_allocator);
err_init:
	CoUninitialize();
out:
	return retval;
}

b32 file_open_dialog(char *fname, u32 n, const char *ext)
{
	const char *extensions[1] = { ext };
	return file_open_dialog_ex(fname, n, extensions, ext ? 1 : 0);
}

b32 file_save_dialog(char *fname, u32 n, const char *ext)
{
	const char *extensions[1] = { ext };
	return file_save_dialog_ex(fname, n, extensions, ext ? 1 : 0);
}

b32 file_open_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext)
{
	return file__dialog(fname, n, ext, n_ext, CLSID_FileOpenDialog,
	                    IID_IFileOpenDialog);
}

b32 file_save_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext)
{
	return file__dialog(fname, n, ext, n_ext, CLSID_FileSaveDialog,
	                    IID_IFileSaveDialog);
}

b32 file_exists(const char *path)
{
	const DWORD attrib = GetFileAttributes(path);
	return    attrib != INVALID_FILE_ATTRIBUTES
	       && !(attrib & FILE_ATTRIBUTE_DIRECTORY);
}

b32 dir_exists(const char *path)
{
	const DWORD attrib = GetFileAttributes(path);
	return    attrib != INVALID_FILE_ATTRIBUTES
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
b32 _mkdir(const char *path)
{
	// catch drive creation
	if (strlen(path) == 2 && path[1] == ':')
		return true;

	return CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

char *app_data_dir(const char *app_name, allocator_t *a)
{
	u32 n;
	char *path;
	FILE *fp = fopen(".access_check", "w");
	if (!fp) {
		PWSTR psz_file_path;
		if (   SHGetKnownFolderPath(&FOLDERID_ProgramData, 0, NULL, &psz_file_path)
		    != S_OK)
			return NULL;

		n = wcslen(psz_file_path) * sizeof(wchar_t);
		path = amalloc(n + 4 + strlen(app_name), a);
		wcstombs(path, psz_file_path, n);
		CoTaskMemFree(psz_file_path);
		path_append(path, app_name);
		return path;
	} else {
		fclose(fp);
		remove(".access_check");
		path = amalloc(2, a);
		path[0] = '.';
		path[1] = '\0';
		return path;
	}
}

/* Dynamic library */

lib_handle lib_load(const char *_filename)
{
	const u32 sz = strlen(_filename);
	char *filename = amalloc(sz + 4, g_temp_allocator);
	strcpy(filename, _filename);
	strcat(filename, ".dll");
	lib_handle hnd = LoadLibrary(filename);
	afree(filename, g_temp_allocator);
	return hnd;
}

void *lib_func(lib_handle hnd, const char *name)
{
	return GetProcAddress(hnd, name);
}

b32 lib_close(lib_handle hnd)
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

b32 open_file_external(const char *filename)
{
	const int ret = (int)ShellExecute(NULL, "open", filename, NULL, NULL,
	                                  SW_SHOWNORMAL);
	if (ret <= 32) {
		log_error("failed to open %s in an external program with error %d",
		          filename, ret);
		return false;
	}
	return true;
}

#else

#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

b32 file__dialog(char *filename, u32 n, const char *cmd)
{
	b32 retval = false;
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

b32 file_open_dialog(char *fname, u32 n, const char *ext)
{
	return file__dialog(fname, n, "zenity --file-selection");
}

b32 file_save_dialog(char *fname, u32 n, const char *ext)
{
	return file__dialog(fname, n, "zenity --file-selection --save");
}

b32 file_open_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext)
{
	return file_open_dialog(fname, n, NULL);
}

b32 file_save_dialog_ex(char *fname, u32 n, const char *ext[], u32 n_ext)
{
	return file_save_dialog(fname, n, NULL);
}

b32 file_exists(const char *path)
{
	struct stat s;
	return stat(path, &s) == 0 && !S_ISDIR(s.st_mode);
}

b32 dir_exists(const char *path)
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
b32 _mkdir(const char *path)
{
	return mkdir(path, S_IRWXU) == 0;
}

char *app_data_dir(const char *app_name, allocator_t *a)
{
	char *path;
	FILE *fp = fopen(".access_check", "w");
	if (fp) {
		fclose(fp);
		remove(".access_check");
		path = amalloc(2, a);
		path[0] = '.';
		path[1] = '\0';
		return path;
	}
	return NULL;
}

/* Dynamic library */

lib_handle lib_load(const char *_filename)
{
	const u32 sz = strlen(_filename);
	char *filename = amalloc(sz + 4, g_temp_allocator);
	strcpy(filename, _filename);
	strcat(filename, ".so");
	lib_handle hnd = dlopen(filename, RTLD_NOW | RTLD_LOCAL);
	afree(filename, g_temp_allocator);
	return hnd;
}

void *lib_func(lib_handle hnd, const char *name)
{
	return dlsym(hnd, name);
}

b32 lib_close(lib_handle hnd)
{
	return dlclose(hnd) == 0;
}

const char *lib_err()
{
	return dlerror();
}

b32 open_file_external(const char *filename)
{
	char command[256] = "xdg-open ";
	const size_t sz = strlen(command);
	strncpy(command + sz, filename, 256 - sz - 1);
	return run(command) == 0;
}

#endif

b32 mkpath(const char *path)
{
	char _path[256];
	if (strlen(path) > sizeof(_path))
		return false;

	if (path[0] == '.' && (path[1] == '/' || path[1] == '\\'))
		strcpy(_path, path + 2);
	else
		strcpy(_path, path);

	if (strlen(_path) == 0)
		return true;

	errno = 0;

	u32 pos = 0, dot_pos = 0;
	for (char *p = _path + 1; *p; ++p) {
		switch (*p) {
		case '/':
		case '\\': {
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
b32 rmdir_f(const char *path)
{
	b32 success = false;
	tinydir_dir dir;
	if (tinydir_open(&dir, path) == -1) {
		log_error("rmdir: error reading directory '%s'", path);
		goto out;
	}
	while (dir.has_next) {
		tinydir_file file;
		if (tinydir_readfile(&dir, &file) != -1) {
			if (   strcmp(file.name, ".") != 0
			    && strcmp(file.name, "..") != 0) {
				if (file.is_dir) {
					rmdir_f(file.path);
				} else if (remove(file.path)) {
					log_error("rmdir: error removing '%s'", file.path);
					goto out;
				}
			}
		} else {
			log_error("rmdir: error examining file");
			goto out;
		}
		if (tinydir_next(&dir) == -1) {
			log_error("rmdir: error iterating directory");
			goto out;
		}
	}
	if (rmdir(path))
		log_error("error removing '%s'", path);
	else
		success = true;
out:
	tinydir_close(&dir);
	return success;
}
#endif // VLT_USE_TINYDIR

/* IO */

size_t vgetdelim(char **lineptr, size_t *n, int delim, FILE *stream, allocator_t *a)
{
	if (!lineptr || !n || !stream) {
		errno = EINVAL;
		return -1;
	}

	if (!*lineptr) {
		*n = 32;
		*lineptr = amalloc(*n, a);
	} else if (*n == 0) {
		errno = EINVAL;
		return -1;
	}

	int i = 0, c;
	while ((c = fgetc(stream)) != EOF) {
		if (i == *n-1) {
			*n *= 2;
			*lineptr = arealloc(*lineptr, *n, a);
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

size_t vgetline(char **lineptr, size_t *n, FILE *stream, allocator_t *a)
{
	return vgetdelim(lineptr, n, '\n', stream, a);
}

/* Other applications */

void exec(char *const argv[])
{
	execv(argv[0], argv);
}

int run(const char *command)
{
#ifndef _WIN32
	FILE *fp = popen(command, "r");
	if (!fp) {
		char local_command[256] = "./";
		strcpy(local_command+2, command);
		fp = popen(local_command, "r");
	}
#else
	FILE *fp = popen(command, "r");
#endif
	if (!fp) {
		log_error("failed to execute %s", command);
		return -1;
	}
	char *log_buf = NULL;
	size_t log_buf_sz;
	log_buf_sz = vgetline(&log_buf, &log_buf_sz, fp, g_temp_allocator);
	while (log_buf_sz != 0 && log_buf_sz != -1) {
		if (log_buf[log_buf_sz - 1] == '\n')
			log_buf[log_buf_sz - 1] = '\0';
		log_info("%s", log_buf);
		log_buf_sz = vgetline(&log_buf, &log_buf_sz, fp, g_temp_allocator);
	}
	afree(log_buf, g_temp_allocator);
	int status = pclose(fp);
	return status != -1 && WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

#undef OS_IMPLEMENTATION
#endif // OS_IMPLEMENTAITON
