#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

const char *g_file_path_separator = "/";

b32 file_open_dialog(char *fname, u32 fname_sz,
                     const file_dialog_filter_t filters[], u32 num_filters)
{
	assert(false);
	return false;
}

b32 file_save_dialog(char *fname, u32 fname_sz,
                     const file_dialog_filter_t filters[], u32 num_filters)
{
	assert(false);
	return false;
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

char *imappdir(void)
{
	return imstrcpy(".");
}

char *imapppath(const char *resource)
{
	return impathcat(imappdir(), resource);
}

char *imresdir(void)
{
	return imstrcpy("data");
}

char *imrespath(const char *resource)
{
	return impathcat(imresdir(), resource);
}

char *imlogdir(void)
{
	return imapppath(".logs");
}

char *imlogpath(const char *resource)
{
	return impathcat(imlogdir(), resource);
}

char *imcachedir(void)
{
	return imapppath(".cache");
}

char *imcachepath(const char *resource)
{
	return impathcat(imcachedir(), resource);
}

char *imdatadir(void)
{
	return imapppath(".data");
}

char *imdatapath(const char *resource)
{
	return impathcat(imdatadir(), resource);
}

void *file_read_all(const char *fname, const char *mode, allocator_t *a)
{
	FILE *fp;
	size_t file_size;
	void *bytes = NULL;

	fp = fopen(fname, mode);
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

out:
	fclose(fp);
	return bytes;
}

/* Dynamic library */

#ifndef VIOLET_NO_LIB
#error "Can't load external libraries with emscripten"
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
	assert(false);
}

int run(const char *command)
{
	assert(false);
	return -1;
}

b32 open_file_external(const char *filename)
{
	return EM_ASM_INT(return window.open(UTF8ToString($0)) != null, filename);
}
