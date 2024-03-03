#include "ls.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int list(struct d_info* info, const char* p, const opt_t opt)
{
	DIR* dir = opendir(p);
	struct dirent* entry = NULL;
	int rv = 0;
	errno  = 0;
	info->child.num = 0;

	if (dir == NULL) {
		fprintf(stderr, "%s: %s\n", p,  strerror(errno));
		return -1;
	}

	for (; (entry = readdir(dir)) != NULL; errno = 0) {
		
		if (~opt & ALL && entry->d_name[0] == '.')
			continue;

		if (vec_push_dirent(&info->child, entry)) {
			perror("List");
			rv = -1;
			goto CLEAN;
		}
	}

	if (errno != 0) {
		fprintf(stderr, "Error while processing %s: %s\n", p, strerror(errno));
		rv = -1;
	}

CLEAN:
	closedir(dir);
	return rv;
}
