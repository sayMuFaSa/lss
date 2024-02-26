#include "ls.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

		if (vec_push_dirent(&info->child, entry)) {
			fprintf(stderr, "List: %s\n", strerror(errno));
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
