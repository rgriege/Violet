#include <stdlib.h>
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

const char *g_file_path_separator = "/";

b32 file_open_dialog(char *fname, u32 fname_sz, file_dialog_filter_t filter)
{
	return file__dialog(fname, fname_sz, "zenity --file-selection");
}

b32 file_save_dialog(char *fname, u32 fname_sz, file_dialog_filter_t filter)
{
	return file__dialog(fname, fname_sz, "zenity --file-selection --save");
}

b32 file_open_dialog_ex(char *fname, u32 fname_sz,
                        const file_dialog_filter_t filters[], u32 num_filters)
{
	const file_dialog_filter_t filter = {0};
	return file_open_dialog(fname, fname_sz, filter);
}

b32 file_save_dialog_ex(char *fname, u32 fname_sz,
                        const file_dialog_filter_t filters[], u32 num_filters)
{
	const file_dialog_filter_t filter = {0};
	return file_save_dialog(fname, fname_sz, filter);
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

static
b32 _mkdir(const char *path)
{
	return mkdir(path, S_IRWXU) == 0;
}

const char *app_dir(void)
{
	/* TODO(rgriege): this won't work for installed applications on Linux/MacOS */
	return ".";
}

char *app_data_dir(const char *app_name, allocator_t *a)
{
	char *path = amalloc(2, a);
	path[0] = '.';
	path[1] = '\0';
	return path;
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

const char *lib_err()
{
	return dlerror();
}
#endif // VIOLET_NO_LIB

b32 open_file_external(const char *filename)
{
	char command[256] = "xdg-open ";
	const size_t sz = strlen(command);
	strncpy(command + sz, filename, 256 - sz - 1);
	return run(command) == 0;
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
	if (rmdir(path))
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
	execv(argv[0], argv);
}

int run(const char *command)
{
	FILE *fp = popen(command, "r");
	if (!fp) {
		char local_command[256] = "./";
		strcpy(local_command+2, command);
		fp = popen(local_command, "r");
	}
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
