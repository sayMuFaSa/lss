#include "ls.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int vec_push(vec_t* vec, const void* item);

int list(struct d_info* info, const char* p, const opt_t opt)
{
	DIR* dir = opendir(p);
	struct dirent* entry = NULL;
	int rv = 0;

	if (dir == NULL) {
		fprintf(stderr, "%s: %s\n", p,  strerror(errno));
		return -1;
	}

	errno = 0;
	info->child.num = 0;

	while ((entry = readdir(dir)) != NULL) {

		if (~opt & ALL && entry->d_name[0] == '.') {
			continue;
		}

		if (vec_push(&info->child, entry)) {
			rv = -1;
			goto CLEAN;
		}

		if (errno != 0) {
			printf("Max error: %s\n", strerror(errno));
		}
		errno = 0;
	}

	if (errno != 0) {
		fprintf(stderr, "Error while processing %s: %s\n", p, strerror(errno));
		rv = -1;
	}

CLEAN:
	closedir(dir);
	return rv;
}

int vec_push(vec_t* vec, const void* item)
{
	const size_t size = vec->size;
	const size_t max  = vec->max;
	const size_t n    = vec->num;
	char* dest        = vec->data;

	if (n >= max) {
		vec->data = realloc(vec->data, size * max * 2);
		if (vec->data == NULL)
			return -1;

		dest = vec->data;
		vec->max *= 2;
	}

	dest += size * n;
	memcpy(dest, item, size);
	vec->num = n + 1;
	return 0;
}
