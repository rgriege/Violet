#include <stdlib.h>
#include <cpuid.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include <sys/utsname.h>
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFURL.h>
#include <dlfcn.h>
#include <unistd.h>

const char *g_file_path_separator = "/";

b32 os_string_from_utf8(char *dst, size_t dstlen, const char *src)
{
	strncpy(dst, src, dstlen);
	return true;
}

size_t os_string_from_utf8_size(const char *src)
{
	return strlen(src) + 1;
}

b32 os_string_to_utf8(char *dst, size_t dstlen, const char *src)
{
	strncpy(dst, src, dstlen);
	return true;
}

size_t os_string_to_utf8_size(const char *src)
{
	return strlen(src) + 1;
}

const char *os_imstr_from_utf8(const char *str)
{
	return str;
}

const char *os_imstr_to_utf8(const char *str)
{
	return str;
}

b32 file_exists(const char *path)
{
	struct stat s;
	return stat(path, &s) == 0 && !S_ISDIR(s.st_mode);
}

s64 file_modified_time(const char *path)
{
	struct stat s;
	return stat(path, &s) == 0 ? s.st_mtime : 0;
}

b32 dir_exists(const char *path)
{
	struct stat s;
	return stat(path, &s) == 0 && S_ISDIR(s.st_mode);
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

b32 mkpath(const char *path_)
{
	if (dir_exists(path_))
		return true;

	char path[PATH_MAX];
	if (strlen(path_) > sizeof(path))
		return false;

	if (path_[0] == '.' && path_[1] == '/')
		strcpy(path, path_ + 2);
	else
		strcpy(path, path_);

	if (strlen(path) == 0)
		return true;

	u32 pos = 0;
	char c = 0;
	for (char *p = path + 1; *p; ++p) {
		if (*p == g_file_path_separator[0]) {
			memswp(c, *p, char);
			if (!dir_exists(path) && mkdir(path, S_IRWXU) != 0 && errno != EEXIST)
				return false;
			memswp(c, *p, char);
			pos = 0;
		} else {
			++pos;
		}
	}
	return mkdir(path, S_IRWXU) == 0;
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
	if (path[0] != 0)
		goto out;
	CFBundleRef bundle = CFBundleGetMainBundle();
	if (bundle == NULL)
		goto out;
	CFURLRef url = CFBundleCopyExecutableURL(bundle);
	if (url == NULL)
		goto out;
	if (CFURLGetFileSystemRepresentation(url, true, (UInt8*)B2PC(path))) {
		char *last = strrchr(path, '/');
		if (last)
			*last = 0;
		else
			path[0] = 0;
	} else {
		path[0] = 0;
	}
	CFRelease(url);
out:
	return imstrcpy(path);
}

char *imapppath(const char *resource)
{
	return impathcat(imappdir(), resource);
}

char *imresdir(void)
{
#ifdef MACOS_BUNDLE_ID
	imstrcpy("");

	CFBundleRef bundle = CFBundleGetMainBundle();
	if (bundle == NULL)
		goto out;

	CFURLRef url = CFBundleCopyResourcesDirectoryURL(bundle);
	if (url == NULL)
		goto out;

	if (!CFURLGetFileSystemRepresentation(url, true, (UInt8*)imstr(), IMPRINT_BUFFER_SIZE))
		imstrcpy("");

	CFRelease(url);
out:
	return imstr();
#else
	return imappdir();
#endif
}

char *imrespath(const char *resource)
{
#ifdef MACOS_BUNDLE_ID
	imstrcpy("");

	CFBundleRef bundle = CFBundleGetMainBundle();
	if (bundle == NULL)
		goto out;

	CFStringRef str = CFStringCreateWithCString(NULL, resource, kCFStringEncodingUTF8);
	CFURLRef url = CFBundleCopyResourceURL(bundle, str, NULL, NULL);
	if (url == NULL)
		goto err;

	if (!CFURLGetFileSystemRepresentation(url, true, (UInt8*)imstr(), IMPRINT_BUFFER_SIZE))
		imstrcpy("");

	CFRelease(url);
err:
	CFRelease(str);
out:
	return imstr();
#else
	return imapppath(resource);
#endif
}

char *imhomedir(void);

static
char *imuserdir(const char *bundle_dir, const char *nbundle_dir)
{
#ifdef MACOS_BUNDLE_ID
	return impathcat(impathcat(impathcat(imhomedir(), "Library"), bundle_dir), MACOS_BUNDLE_ID);
#else
	return imapppath(nbundle_dir);
#endif
}

char *imlogdir(void)
{
	return imuserdir("Logs", ".logs");
}

char *imlogpath(const char *resource)
{
	return impathcat(imlogdir(), resource);
}

char *imcachedir(void)
{
	return imuserdir("Caches", ".cache");
}

char *imcachepath(const char *resource)
{
	return impathcat(imcachedir(), resource);
}

char *imdatadir(void)
{
	return imuserdir("Application Support", ".data");
}

char *imdatapath(const char *resource)
{
	return impathcat(imdatadir(), resource);
}

b32 is_data_dir_in_use_by_another_instance(void)
{
	static b32 logged_once = false; // don't spam logs with this warning
	if (!logged_once) {
		log_warn("is_data_dir_in_use_by_another_instance is not implemented for this platform");
		logged_once = true;
	}
	return false;
}

char *imuserdatadir(void)
{
	return imdatadir();
}

char *imuserdatapath(const char *resource)
{
	return imdatapath(resource);
}

FILE *file_open(const char *fname, const char *mode)
{
	return fopen(fname, mode);
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

const char *lib_err(void)
{
	return dlerror();
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
	execv(argv[0], argv);
}

b32 open_file_external(const char *filename)
{
	b32 result;
	str_t cmd = str_create(g_temp_allocator);
	str_cpy(&cmd, "open ");
	str_cat(&cmd, filename);
	result = (system(cmd) == 0);
	str_destroy(&cmd);
	return result;
}

/* System */

s32 cpu_max_sse(void)
{
	unsigned int info[4] = {0};
	unsigned int max_function_id = __get_cpuid_max(0, NULL);
	if (max_function_id < 1) {
		log_error("failed to fetch highest cpuid functionid");
		return OS_SSE_VERSION_UNKNOWN;
	}
	__get_cpuid(1, &info[0], &info[1], &info[2], &info[3]);
	return cpu_max_sse_(info);
}

b32 cpu_supports_sse41(void)
{
	return cpu_max_sse() >= OS_SSE_VERSION_41;
}

void cpu_vendor(char vendor[32])
{
	unsigned int info[4] = {0};
	__get_cpuid(0, &info[0], &info[1], &info[2], &info[3]);
	cpu_vendor_(info, vendor);
}

os_utsname_t os_uname(void)
{
	os_utsname_t os_utsname = {0};
	struct utsname os ={0};
	if (uname(&os) == 0) {
		strbcpy(os_utsname.sysname, os.sysname);
		strbcpy(os_utsname.release, os.release);
		strbcpy(os_utsname.version, os.version);
		strbcpy(os_utsname.machine, os.machine);
	} else {
		strbcpy(os_utsname.sysname, "MacOS");
	}
	return os_utsname;
}
