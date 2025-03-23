// SPDX-FileCopyrightText: 2021 AJ Walter
// SPDX-License-Identifier: GPL-3.0-or-later OR MIT

/**
 * Note: This compiler seems to be missing some definitions.
 *       Stubs are defined here to fix compiler errors.
 *       This only applies to the nSpire platform.
 */

#include <errno.h>
#include <sys/stat.h>

/*
int chdir(const char* path) {
	(void)(path);
	errno = ENOSYS;
	return -1;
}

int mkdir(const char* pathname, mode_t mode) {
	(void)(pathname);
	(void)(mode);
	errno = ENOSYS;
	return -1;
}
*/

int chmod(const char* pathname, mode_t mode) {
	(void)(pathname);
	(void)(mode);
	errno = ENOSYS;
	return -1;
}

long pathconf(const char* path, int name) {
	(void)(path);
	(void)(name);
	errno = ENOSYS;
	return -1;
}

/*
char* getcwd(char* buf, size_t size) {
	(void)(buf);
	(void)(size);
	errno = ENOSYS;
	return NULL;
}

int _unlink(const char* name) {
	(void)(name);
	errno = ENOSYS;
	return -1;
}

int _link(const char* old, const char* new) {
	(void)(old);
	(void)(new);
	errno = ENOSYS;
	return -1;
}

int _stat(const char* file, struct stat* st) {
	(void)(file);
	(void)(st);
	errno = ENOSYS;
	return -1;
}
*/
