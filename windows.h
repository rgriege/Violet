#include "violet/core.h"
#include "violet/os.h"

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <ShlObj.h>
#include <shobjidl.h>
#include <Shellapi.h>

const char *g_file_path_separator = "\\";

b32 os_string_to_utf8(char *dst, size_t dstlen, const wchar_t* src)
{
	return WideCharToMultiByte(CP_UTF8, 0, src, -1, dst, (int)dstlen, NULL, NULL) != 0;
}

size_t os_string_to_utf8_size(const wchar_t* src)
{
	return (size_t)WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
}

b32 os_string_from_utf8(wchar_t *dst, size_t dstlen, const char *src)
{
	return MultiByteToWideChar(CP_UTF8, 0, src, -1, dst, (int)dstlen) != 0;
}

size_t os_string_from_utf8_size(const char *src)
{
	return (size_t)MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, 0);
}

const wchar_t *os_imstr_from_utf8(const char *str)
{
	static thread_local wchar_t str_w[IMPRINT_BUFFER_SIZE];
	if (!os_string_from_utf8(B2PC(str_w), str))
		str_w[0] = 0;
	return str_w;
}

const char *os_imstr_to_utf8(const wchar_t *str)
{
	static thread_local char str_utf8[IMPRINT_BUFFER_SIZE];
	if (!os_string_to_utf8(B2PC(str_utf8), str))
		str_utf8[0] = 0;
	return str_utf8;
}

static
b32 file_dialog__os_from_utf8(const char *src, const wchar_t **pdst)
{
	const size_t sz = os_string_from_utf8_size(src);
	wchar_t *dst = amalloc(sz * sizeof(wchar_t), g_temp_allocator);
	const b32 ret = os_string_from_utf8(dst, sz, src);
	*pdst = dst;
	return ret;
}
static
b32 file__dialog(char *filename, u32 fname_sz,
                 const file_dialog_filter_t filters[], u32 num_filters,
                 CLSID clsid, IID iid)
{
	temp_memory_mark_t mark = temp_memory_save(g_temp_allocator);
	b32 retval = false;
	const wchar_t *ext;
	wchar_t *filename_w;
	void *vp = NULL;
	IFileDialog *dialog;
	COMDLG_FILTERSPEC *filterspec = NULL;
	IShellItem *item;

	assert(num_filters > 0);

	if (!SUCCEEDED(CoInitializeEx(NULL,   COINIT_APARTMENTTHREADED
	                                    | COINIT_DISABLE_OLE1DDE)))
		goto out;

	if (!SUCCEEDED(CoCreateInstance(&clsid, NULL, CLSCTX_ALL, &iid, &vp)))
		goto err_init;
	dialog = vp;

	for (u32 i = 0; i < num_filters; ++i) {
		if (!(   strlen(filters[i].pattern) > 2
		      && filters[i].pattern[0] == '*'
		      && filters[i].pattern[1] == '.')) {
			assert(false);
			goto err_fltr;
		}
	}

	if (   !file_dialog__os_from_utf8(filters[0].pattern+2, &ext)
	    || !SUCCEEDED(dialog->lpVtbl->SetDefaultExtension(dialog, ext)))
		goto err_fltr;

	filterspec = amalloc(num_filters * sizeof(COMDLG_FILTERSPEC), g_temp_allocator);
	for (u32 i = 0; i < num_filters; ++i) {
		if (   !file_dialog__os_from_utf8(filters[i].name, &filterspec[i].pszName)
		    || !file_dialog__os_from_utf8(filters[i].pattern, &filterspec[i].pszSpec))
			goto err_fltr;
	}
	if (!SUCCEEDED(dialog->lpVtbl->SetFileTypes(dialog, num_filters, filterspec)))
		goto err_fltr;

	if (!SUCCEEDED(dialog->lpVtbl->Show(dialog, NULL)))
		goto err_dlg;

	if (!SUCCEEDED(dialog->lpVtbl->GetResult(dialog, &item)))
		goto err_dlg;

	if (!SUCCEEDED(item->lpVtbl->GetDisplayName(item, SIGDN_FILESYSPATH, &filename_w)))
		goto err_itm;

	retval = os_string_to_utf8(filename, fname_sz, filename_w);
	CoTaskMemFree(filename_w);

err_itm:
	item->lpVtbl->Release(item);
err_dlg:
	dialog->lpVtbl->Release(dialog);
err_fltr:
err_init:
	CoUninitialize();
out:
	temp_memory_restore(mark);
	return retval;
}

b32 file_open_dialog(char *fname, u32 fname_sz,
                     const file_dialog_filter_t filters[], u32 num_filters)
{
	return file__dialog(fname, fname_sz, filters, num_filters,
	                    CLSID_FileOpenDialog, IID_IFileOpenDialog);
}

b32 file_save_dialog(char *fname, u32 fname_sz,
                     const file_dialog_filter_t filters[], u32 num_filters)
{
	return file__dialog(fname, fname_sz, filters, num_filters,
	                    CLSID_FileSaveDialog, IID_IFileSaveDialog);
}

b32 file_exists(const char *path)
{
	wchar_t path_w[PATH_MAX];
	DWORD attrib;

	if (!os_string_from_utf8(B2PC(path_w), path)) {
		log_error("%s(%s): os_string_from_utf8(%s) error %d",
		          __FUNCTION__, path, path, GetLastError());
		return false;
	}

	attrib = GetFileAttributesW(path_w);
	return attrib != INVALID_FILE_ATTRIBUTES
	    && !(attrib & FILE_ATTRIBUTE_DIRECTORY);
}

b32 dir_exists(const char *path)
{
	wchar_t path_w[PATH_MAX];
	DWORD attrib;

	if (!os_string_from_utf8(B2PC(path_w), path)) {
		log_error("%s(%s): os_string_from_utf8(%s) error %d",
		          __FUNCTION__, path, path, GetLastError());
		return false;
	}

	attrib = GetFileAttributesW(path_w);
	return attrib != INVALID_FILE_ATTRIBUTES
	    && (attrib & FILE_ATTRIBUTE_DIRECTORY);
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

static
b32 CreateOrReuseDirectory(const wchar_t *path)
{
	return CreateDirectoryW(path, NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
}

b32 mkpath(const char *path)
{
	char path_mod[PATH_MAX];
	wchar_t path_w[PATH_MAX];
	char *p;
	char swap;

	if (dir_exists(path))
		return true;

	if (path[0] == '.' && path[1] == g_file_path_separator[0]) {
		strbcpy(path_mod, path);
		p = path_mod + 2;
	} else if (path[0] == g_file_path_separator[0] && path[1] == g_file_path_separator[0]) {
		strbcpy(path_mod, path);
		p = path_mod + 2;
	} else if (path[0] != 0 && path[1] == ':') {
		strbcpy(path_mod, path);
		p = path_mod + 2;
	} else {
		strbcpy(path_mod, path);
		p = path_mod;
	}

	if (p[0] == 0)
		return true;

	swap = 0;
	for (p = p + 1; *p; ++p) {
		if (*p == g_file_path_separator[0]) {
			memswp(swap, *p, char);
			if (!os_string_from_utf8(B2PC(path_w), path_mod)) {
				log_error("%s(%s): os_string_from_utf8(%s) error %d",
				          __FUNCTION__, path, path_mod, GetLastError());
				return false;
			}
			if (!CreateOrReuseDirectory(path_w)) {
				log_error("%s(%s): CreateDirectory(%s) error %d",
				          __FUNCTION__, path, path_mod, GetLastError());
				return false;
			}
			memswp(swap, *p, char);
		}
	}

	if (!os_string_from_utf8(B2PC(path_w), path_mod)) {
		log_error("%s(%s): os_string_from_utf8(%s) error %d",
		          __FUNCTION__, path, path_mod, GetLastError());
		return false;
	}
	if (!CreateOrReuseDirectory(path_w)) {
		log_error("%s(%s): CreateDirectory(%s) error %d",
		          __FUNCTION__, path, path_mod, GetLastError());
		return false;
	}
	return true;
}

#ifdef VLT_USE_TINYDIR
static
b32 rmdir_fw(const wchar_t *path_w)
{
	char path[PATH_MAX];
	b32 success = false;
	tinydir_dir dir;

	if (tinydir_open(&dir, path_w) == -1) {
		if (!os_string_to_utf8(B2PS(path), path_w))
			strbcpy(path, "<unprintable>");
		log_error("rmdir: error reading directory '%s'", path);
		goto out;
	}

	while (dir.has_next) {
		tinydir_file file;
		if (tinydir_readfile(&dir, &file) != -1) {
			if (   wcscmp(file.name, L".") != 0
			    && wcscmp(file.name, L"..") != 0) {
				if (file.is_dir) {
					rmdir_fw(file.path);
				} else if (_wremove(file.path)) {
					if (!os_string_to_utf8(B2PS(path), file.path))
						strbcpy(path, "<unprintable>");
					log_error("rmdir: error removing '%s'", path);
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
	if (RemoveDirectoryW(path_w)) {
		success = true;
	} else {
		if (!os_string_to_utf8(B2PS(path), path_w))
			strbcpy(path, "<unprintable>");
		log_error("error removing '%s'", path);
	}
out:
	tinydir_close(&dir);
	return success;
}

b32 rmdir_f(const char *path)
{
	wchar_t path_w[PATH_MAX];
	return os_string_from_utf8(B2PC(path_w), path)
	    && rmdir_fw(path_w);
}
#endif // VLT_USE_TINYDIR

char *impathcat(char *imstr, const char *path)
{
	return imstrcatn(imstrcatn(imstr, g_file_path_separator), path);
}

char *impathcatprintf(char *imstr, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	imstrcatprintfv(imstrcatn(imstr, g_file_path_separator), fmt, args);
	va_end(args);
	return imstr;
}

char *imappdir(void)
{
	static thread_local char path[PATH_MAX] = {0};
	wchar_t path_w[PATH_MAX];
	char *last;

	if (path[0] != 0)
		goto out;

	if (GetModuleFileNameW(NULL, B2PC(path_w)) == 0)
		goto out;

	if (!os_string_to_utf8(B2PC(path), path_w))
		goto out;

	if ((last = strrchr(path, '\\')) != NULL)
		*last = 0;
out:
	return imstrcpy(path);
}

char *imapppath(const char *resource)
{
	return impathcat(imappdir(), resource);
}

static
char *imknownfolderdir(KNOWNFOLDERID id, const char *default_dir)
{
#ifdef WINDOWS_PACKAGE_NAME
	HRESULT result;
	PWSTR wpath;
	if ((result = SHGetKnownFolderPath(&id, 0, NULL, &wpath)) != S_OK) {
		log_error("%s() SHGetKnownFolderPath error %d", __FUNCTION__, result);
		imstrcpy(default_dir);
		goto out;
	}
	if (!os_string_to_utf8(imstr(), IMPRINT_BUFFER_SIZE, wpath)) {
		log_error("%s() os_string_to_utf8 error %d", __FUNCTION__, GetLastError());
		imstrcpy(default_dir);
		goto err;
	}
	impathcat(imstr(), WINDOWS_PACKAGE_NAME);
err:
	CoTaskMemFree(wpath);
out:
	return imstr();
#else
	return imapppath(default_dir);
#endif
}

static
char *improgdatadir(void)
{
	return imknownfolderdir(FOLDERID_ProgramData, ".ProgramData");
}

static
char *improgdatapath(const char *resource)
{
	return impathcat(improgdatadir(), resource);
}

char *imresdir(void)
{
	return imappdir();
}

char *imrespath(const char *resource)
{
	return imapppath(resource);
}

char *imlogdir(void)
{
	return improgdatapath(".logs");
}

char *imlogpath(const char *resource)
{
	return impathcat(imlogdir(), resource);
}

char *imcachedir(void)
{
	return improgdatadir();
}

char *imcachepath(const char *resource)
{
	return impathcat(imcachedir(), resource);
}

char *imdatadir(void)
{
	return improgdatadir();
}

char *imdatapath(const char *resource)
{
	return impathcat(imdatadir(), resource);
}

char *imuserdatadir(void)
{
	return imknownfolderdir(FOLDERID_LocalAppData, ".UserData");
}

char *imuserdatapath(const char *resource)
{
	return impathcat(imuserdatadir(), resource);
}

FILE *file_open(const char *fname, const char *mode)
{
	wchar_t fname_w[PATH_MAX];
	wchar_t mode_w[PATH_MAX];
	FILE *fp;
	if (!os_string_from_utf8(B2PS(fname_w), fname)) {
		log_error("%s(%s): os_string_from_utf8(%s) error %d",
		          __FUNCTION__, fname, fname, GetLastError());
		return NULL;
	}
	if (!os_string_from_utf8(B2PS(mode_w), mode)) {
		log_error("%s(%s): os_string_from_utf8(%s) error %d",
		          __FUNCTION__, fname, mode, GetLastError());
		return NULL;
	}
	if (!(fp = _wfopen(fname_w, mode_w))) {
		log_error("%s(%s, %s): _wfopen() error %d", __FUNCTION__, fname, mode, errno);
		return NULL;
	}
	return fp;
}

void *file_read_all(const char *fname, const char *mode, size_t *sz, allocator_t *a)
{
	FILE *fp;
	size_t file_size;
	void *bytes = NULL;

	fp = file_open(fname, mode);
	if (!fp)
		return NULL;

	if (fseek(fp, 0, SEEK_END) == -1)
		goto out;

	file_size = ftell(fp);
	bytes = amalloc(file_size, a);

	fseek(fp, 0, SEEK_SET);

	if (fread(bytes, 1, file_size, fp) != file_size) {
		afree(bytes, a);
		bytes = NULL;
		goto out;
	}

	if (sz)
		*sz = file_size;

out:
	fclose(fp);
	return bytes;
}

/* Dynamic library */

#ifndef VIOLET_NO_LIB
lib_handle lib_load(const char *filename)
{
	static const wchar_t *ext = L".dll";
	lib_handle hnd = NULL;
	const size_t sz = os_string_from_utf8_size(filename);
	wchar_t *filename_w = amalloc(sz + wcslen(ext), g_temp_allocator);
	if (!os_string_from_utf8(filename_w, sz, filename))
		goto out;
	wcscat(filename_w, ext);
	hnd = LoadLibraryW(filename_w);
out:
	afree(filename_w, g_temp_allocator);
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

const char *lib_err(void)
{
	static thread_local char buf[256];
	wchar_t buf_w[256];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
	              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), B2PC(buf_w), NULL);
	if (!os_string_to_utf8(B2PC(buf), buf_w))
		buf[0] = 0;
	return buf;
}
#endif // VIOLET_NO_LIB

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

b32 open_file_external(const char *filename)
{
	wchar_t filename_w[PATH_MAX];
	INT_PTR ret;

	if (!os_string_from_utf8(B2PC(filename_w), filename))
		return false;

	ret = (INT_PTR)ShellExecuteW(NULL, L"open", filename_w,
	                             NULL, NULL, SW_SHOWNORMAL);
	if (ret <= 32) {
		log_error("failed to open %s in an external program with error %d",
		          filename, ret);
		return false;
	}
	return true;
}

/* System */

s32 cpu_max_sse(void)
{
	int info[4] = {0};
	int max_function_id;
	__cpuid(info, 0);
	max_function_id = info[0];
	if (max_function_id < 1) {
		log_error("failed to fetch highest cpuid functionid");
		return false;
	}
	__cpuid(info, 1);
	return cpu_max_sse_(info);
}

b32 cpu_supports_sse41(void)
{
	return cpu_max_sse() >= OS_SSE_VERSION_41;
}

b32 os_uname(os_utsname_t *os_utsname)
{
	/* TODO(ben): implement this */
	return false;
}
