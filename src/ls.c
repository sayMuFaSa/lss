#include "ls.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>


static int alpha(const void* a, const void* b);

opt_t parse(const int argc, char* argv[])
{
	int arg; opt_t opt = DEF;

	while ((arg = getopt(argc, argv, "laf1")) != -1) {
		
		switch (arg) {
			case 'l': 
				opt |= LONG | ONEPL;
				break;

			case 'a': 
				opt |= ALL;
				break;

			case '1':
				opt |= ONEPL;
				break;

			case '?': 
				return UNKNOWN;
		}

	}

	if (argc - optind > 1) opt |= MULTIPLE;

	if (!isatty(STDOUT_FILENO)) opt |= ONEPL;

	return opt;
}

int ls (struct d_info* info,  const char* target, const opt_t opt)
{
	struct stat file;

	if (stat(target, &file) != 0) {
		fprintf(stderr, "%s: %s\n", target, strerror(errno));
		return -1;
	} 

	if ((file.st_mode & S_IFMT) != S_IFDIR) {
		printf("%s ", target);
		return 0;
	}


	if (list(info, target, opt))
		return -1;

	vec_sort_dirent(&info->child, &alpha);

	if (opt & LONG)
		if (get_stats(info, target))
			return -1;

	print(info, target, opt);

	return 0;
}

int alpha (const void* a, const void* b)
{
	const struct dirent* ad = a;
	const struct dirent* bd = b;
	const char* aname       = ad->d_name;
	const char* bname       = bd->d_name;

	for (size_t i = 0; aname[i] || bname[i]; i++) {
		if (aname[i] > bname[i]) return 1;
		if (aname[i] < bname[i]) return -1;
	}

	return 0;
}



int get_stats(struct d_info* info, const char*  p)
{
	const size_t it = info->child.num;
	const size_t size = sizeof(struct stat);
	const struct dirent* child = info->child.data;
	char path[PATH_MAX];

	info->stats = malloc(it * size);

	if (info->stats == NULL) {
		fprintf(stderr, "Malloc failed in get_stats: %s\n", strerror(errno));
		return -1;
	}

	for (size_t i = 0; i < it; i++) {
		const char *name = child[i].d_name;
		sprintf(path, "%s/%s", p, name);
		lstat(path, info->stats + i);
	}

	return 0;
}
