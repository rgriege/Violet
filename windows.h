#include "violet/core.h"
#include "violet/os.h"

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <ShlObj.h>
#include <shobjidl.h>
#include <Shellapi.h>

static
b32 file__dialog(char *filename, u32 fname_sz,
                 const file_dialog_filter_t filters[], u32 num_filters,
                 CLSID clsid, IID iid)
{
	b32 retval = false;
	PWSTR ext_buf = NULL;
	COMDLG_FILTERSPEC *filterspec = NULL;
	size_t ttl_sz;
	PWSTR p;

	assert(num_filters > 0);

	if (!SUCCEEDED(CoInitializeEx(NULL,   COINIT_APARTMENTTHREADED
	                                    | COINIT_DISABLE_OLE1DDE)))
		goto out;

	void *vp = NULL;
	if (!SUCCEEDED(CoCreateInstance(&clsid, NULL, CLSCTX_ALL, &iid, &vp)))
		goto err_init;
	IFileDialog *dialog = vp;

	ttl_sz = strlen(filters[0].pattern) - 1;
	for (u32 i = 0; i < num_filters; ++i) {
		ttl_sz += strlen(filters[i].name) + 1;
		ttl_sz += strlen(filters[i].pattern) + 1;
	}
	ext_buf = amalloc(ttl_sz * sizeof(wchar_t), g_temp_allocator);

	p = ext_buf;
	{
		const size_t pattern_sz = strlen(filters[0].pattern) - 1;
		assert(   strlen(filters[0].pattern) > 2
		       && filters[0].pattern[0] == '*'
		       && filters[0].pattern[1] == '.');
		mbstowcs(p, filters[0].pattern + 2, pattern_sz);
		p += pattern_sz;
	}
	for (u32 i = 0; i < num_filters; ++i) {
		const size_t name_sz = strlen(filters[i].name) + 1;
		const size_t pattern_sz = strlen(filters[i].pattern) + 1;
		assert(   pattern_sz > 2
		       && filters[i].pattern[0] == '*'
		       && filters[i].pattern[1] == '.');
		mbstowcs(p, filters[i].name, name_sz);
		p += name_sz;
		mbstowcs(p, filters[i].pattern, pattern_sz);
		p += pattern_sz;
	}

	p = ext_buf;
	if (!SUCCEEDED(dialog->lpVtbl->SetDefaultExtension(dialog, p)))
		goto err_ext;
	p += strlen(filters[0].pattern) - 1;

	filterspec = amalloc(num_filters * sizeof(COMDLG_FILTERSPEC), g_temp_allocator);
	for (u32 i = 0; i < num_filters; ++i) {
		filterspec[i].pszName = p;
		p += strlen(filters[i].name) + 1;
		filterspec[i].pszSpec = p;
		p += strlen(filters[i].pattern) + 1;
	}
	if (!SUCCEEDED(dialog->lpVtbl->SetFileTypes(dialog, num_filters, filterspec)))
		goto err_exts;

	if (!SUCCEEDED(dialog->lpVtbl->Show(dialog, NULL)))
		goto err_dlg;

	IShellItem *item;
	if (!SUCCEEDED(dialog->lpVtbl->GetResult(dialog, &item)))
		goto err_dlg;

	PWSTR psz_file_path;
	if (!SUCCEEDED(item->lpVtbl->GetDisplayName(item, SIGDN_FILESYSPATH,
	                                            &psz_file_path)))
		goto err_itm;

	wcstombs(filename, psz_file_path, fname_sz);
	CoTaskMemFree(psz_file_path);
	retval = true;

err_itm:
	item->lpVtbl->Release(item);
err_dlg:
	dialog->lpVtbl->Release(dialog);
err_exts:
	afree(filterspec, g_temp_allocator);
err_ext:
	afree(ext_buf, g_temp_allocator);
err_init:
	CoUninitialize();
out:
	return retval;
}

const char *g_file_path_separator = "\\";

b32 file_open_dialog(char *fname, u32 fname_sz, file_dialog_filter_t filter)
{
	return file_open_dialog_ex(fname, fname_sz, &filter, 1);
}

b32 file_save_dialog(char *fname, u32 fname_sz, file_dialog_filter_t filter)
{
	return file_save_dialog_ex(fname, fname_sz, &filter, 1);
}

b32 file_open_dialog_ex(char *fname, u32 fname_sz,
                        const file_dialog_filter_t filters[], u32 num_filters)
{
	return file__dialog(fname, fname_sz, filters, num_filters,
	                    CLSID_FileOpenDialog, IID_IFileOpenDialog);
}

b32 file_save_dialog_ex(char *fname, u32 fname_sz,
                        const file_dialog_filter_t filters[], u32 num_filters)
{
	return file__dialog(fname, fname_sz, filters, num_filters,
	                    CLSID_FileSaveDialog, IID_IFileSaveDialog);
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

static
b32 _mkdir(const char *path)
{
	// catch drive creation
	if (strlen(path) == 2 && path[1] == ':')
		return true;

	return CreateDirectory(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

const char *app_dir(void)
{
	static char path[MAX_PATH] = {0};
	if (path[0] != '\0') {
	} else if (GetModuleFileName(NULL, B2PC(path)) != 0) {
		char *last = strrchr(path, '\\');
		if (last)
			*last = '\0';
	} else {
		path[0] = '\0';
	}
	return path;
}

char *app_data_dir(const char *app_name, allocator_t *a)
{
#ifndef WINDOWS_LOCAL_PROGRAM_DATA
	PWSTR psz_file_path;
	if (SHGetKnownFolderPath(&FOLDERID_ProgramData, 0, NULL, &psz_file_path) == S_OK) {
		const size_t n = wcslen(psz_file_path) * sizeof(wchar_t);
		char *path = amalloc(n + 4 + strlen(app_name), a);
		wcstombs(path, psz_file_path, n);
		CoTaskMemFree(psz_file_path);
		path_append(path, app_name);
		return path;
	}
	return NULL;
#else
	char *path = amalloc(2, a);
	path[0] = '.';
	path[1] = '\0';
	return path;
#endif
}

/* Dynamic library */

#ifndef VIOLET_NO_LIB
lib_handle lib_load(const char *_filename)
{
	const size_t sz = strlen(_filename);
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
#endif // VIOLET_NO_LIB

b32 open_file_external(const char *filename)
{
	const INT_PTR ret = (INT_PTR)ShellExecute(NULL, "open", filename,
	                                          NULL, NULL, SW_SHOWNORMAL);
	if (ret <= 32) {
		log_error("failed to open %s in an external program with error %d",
		          filename, ret);
		return false;
	}
	return true;
}

void path_append(char *lhs, const char *rhs)
{
	strcat(lhs, g_file_path_separator);
	strcat(lhs, rhs);
}

void path_appendn(char *lhs, const char *rhs, u32 sz)
{
	strcat(lhs, g_file_path_separator);
	strncat(lhs, rhs, sz);
}

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
	if (!RemoveDirectory(path))
		log_error("error removing '%s'", path);
	else
		success = true;
out:
	tinydir_close(&dir);
	return success;
}
#endif // VLT_USE_TINYDIR

char *imapppath(const char *path_relative_to_app)
{
	return imstrcatn(imstrcat2(app_dir(), g_file_path_separator), path_relative_to_app);
}

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
	_execv(argv[0], argv);
}

int run(const char *command)
{
	FILE *fp = _popen(command, "r");
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
	int status = _pclose(fp);
	return status != -1 ? status : -1;
}
