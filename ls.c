#ifndef LS_C
#define LS_C

#include "ls.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <getopt.h>

int init (struct d_info *info, const opt_t opt, const size_t min) 
{
	const size_t size = (opt & LONG) ? sizeof(desc_t) : sizeof(struct dirent);
	void* space = malloc(size * min);

	if (space == NULL) {
		fprintf(stderr, "Init: %s", strerror(errno));
		return 1;
	}

	if (opt & LONG) 
		info->desc = space;
	else 
		info->child = space;
	
	info->size = size;
	
	info->opt = opt;

	info->max = min;
	
	return 0;
}

opt_t parse(const int argc, char* argv[])
{
	int arg; opt_t opt = DEF;

	while ((arg = getopt(argc, argv, "laf1")) != -1) {
		switch (arg) {
			case 'l': opt |= LONG;    break;
			case 'a': opt |= ALL;     break;
			case '1': opt |= ONEPL;   break;
			case '?': return UNKNOWN;
		}
	}

	return opt;
}

int ls (struct d_info *__restrict__ info,  const char *__restrict__ target)
{
	const  size_t size = info->size;
	struct stat   file;

	if (stat(target, &file) != 0) {
		fprintf(stderr, "%s: %s\n", target, strerror(errno));
		return 2;
	}

	if ((file.st_mode & S_IFMT) != S_IFDIR) {
		printf("%s ", target);
		return 0;
	}

	if (rdir(info, target) == mem) {
		return 2;
	}

	if (info->desc) {
		stats(info, target);
		qsort(info->desc, info->num, size, &sort_long);
	} 
	else 
		qsort(info->child, info->num, size, &sort_def);


	print(info, target);
	free(info->child);
	free(info->desc);
	return 0;
}

err_t rdir(struct d_info *__restrict__ info, const char *__restrict__ p)
{
	DIR* dir = opendir(p);
	int rv = 0;
	const size_t size = info->size;
	struct dirent* entry = NULL;
	const opt_t opt = info->opt;
	void *copy_to;

	if (dir == NULL) {
		fprintf(stderr, "%s: %s\n", p,  strerror(errno));
		return 1;
	}

	errno = 0;

	while ((entry = readdir(dir)) != NULL) {

		if (!(opt & ALL) && entry->d_name[0] == '.') {
			continue;
		}

		if (info->num >= info->max)
			if (alloc(info))
				break;

		copy_to = (info->desc)
			      ? &info->desc[info->num].child
			      : info->child + info->num;

		memcpy(copy_to, entry, size);

		info->num++;

		if (errno != 0) {
			printf("Max error: %s\n", strerror(errno));
		}
		errno = 0;
	}

	if (errno != 0) {
		fprintf(stderr, "Error while processing %s: %s\n", p, strerror(errno));
		free(info->child);
		free(info->desc);
		rv = 1;
	}

	closedir(dir);

	return rv;
}

int alloc(struct d_info *info) 
{
	const size_t size = info->size;
	const size_t max = info->max * 2;

	if (info->desc)
		info->desc = realloc(info->desc, size * max);
	else 
		info->child = realloc(info->child, size * max);

	if (info->desc == 0 && info->child == 0) {
		fprintf(stderr, "Alloc: %s\n", strerror(errno));
		return 1;
	}

	info->max = max;

	return 0;
}

err_t stats(struct d_info *__restrict__ info, const char* __restrict__ p) 
{
	const int it = info->num;
	char path[PATH_MAX];

	for (size_t i = 0; i < it; i++) {
		const char *name = info->desc[i].child.d_name;
		sprintf(path, "%s/%s", p, name);
		lstat(path, &info->desc[i].stats);
	}

	return noerror;
}

void print(const struct d_info *__restrict__ info, const char *__restrict__ p)
{
	const opt_t opt = info->opt;

	if (opt & LONG) {
		setvbuf(stdout, NULL, _IOFBF, 0);
		printfile(info, p);
	} else if (opt & ONEPL) {
		setvbuf(stdout, NULL, _IOFBF, 0);
	}

	for (int i = 0; i < info->num; i++) {
		if (opt & ONEPL) 
			puts(info->child[i].d_name);
		else 
			printf("%s ", info->child[i].d_name);
	}

	if (!(opt & (LONG | ONEPL))) putchar('\n');
}

void printfile(const struct d_info *__restrict__ info, const char *__restrict__ p)
{ 
/* 	char fpath[100] = {0};
	struct stat l_opt;
	char perm[20] = {0}; 
	char mtime[30];

	static unsigned int muid = 0; 
	static char muname[30] = {0}; 
	static char *uname = NULL;

	static unsigned int mgid = 0; 
	static char mgname[30] = {0}; 
	static char *gname = NULL;

	sprintf(fpath, "%s/%s", p, f);
	if (lstat(fpath, &l_opt) != 0) {
		fprintf(stderr, "Can't stat file %s: %s\n", fpath, strerror(errno));
	}

	switch (l_opt.st_mode & S_IFMT) {
		case S_IFREG:  perm[0] = '-'; break;
		case S_IFDIR:  perm[0] = 'd'; break;
		case S_IFLNK:  perm[0] = 'l'; break;
		case S_IFCHR:  perm[0] = 'c'; break;
		case S_IFBLK:  perm[0] = 'b'; break;
		case S_IFSOCK: perm[0] = 's'; break;
		case S_IFIFO:  perm[0] = 'p'; break;
	}

	perm[1] = (l_opt.st_mode & S_IRUSR) ? 'r' : '-';
	perm[2] = (l_opt.st_mode & S_IWUSR) ? 'w' : '-';
	perm[3] = (l_opt.st_mode & S_IXOTH && l_opt.st_mode & S_ISUID) ? 's' 
		: (l_opt.st_mode & S_IXOTH) ? 'x' : (l_opt.st_mode & S_ISUID)
		? 'S' : '-';

	perm[4] = (l_opt.st_mode & S_IRGRP) ? 'r' : '-';
	perm[5] = (l_opt.st_mode & S_IWGRP) ? 'w' : '-';
	perm[6] = (l_opt.st_mode & S_IXOTH && l_opt.st_mode & S_ISGID) ? 's' 
		: (l_opt.st_mode & S_IXOTH) ? 'x' : (l_opt.st_mode & S_ISGID)
		? 'S' : '-';

	perm[7] = (l_opt.st_mode & S_IROTH) ? 'r' : '-';
	perm[8] = (l_opt.st_mode & S_IWOTH) ? 'w' : '-';
	perm[9] = (l_opt.st_mode & S_IXOTH && l_opt.st_mode & S_ISVTX) ? 't' 
		: (l_opt.st_mode & S_IXOTH) ? 'x' : (l_opt.st_mode & S_ISVTX)
		? 'T' : '-';


	if (muid == l_opt.st_uid) {
		uname = muname;
	} else {
		strcpy(muname, getpwuid(l_opt.st_uid)->pw_name);
		uname = muname;
	}

	if (mgid == l_opt.st_gid) {
		gname = mgname;
	} else {
		strcpy(mgname, getgrgid(l_opt.st_uid)->gr_name);
		gname = mgname;
	}

	strftime(mtime, 30,  "%b %e %R", localtime(&l_opt.st_mtim.tv_sec));

	printf("%s %lu %s %-8s %8lu %s %s\n", perm, l_opt.st_nlink, uname , gname, l_opt.st_size, mtime, f); */
}

int sort_def (const void* a, const void* b)
{
	struct dirent* ad = (struct dirent*) a;
	struct dirent* bd = (struct dirent*) b;
	int ctr = 0;

	while (ad->d_name[ctr] || bd->d_name[ctr]) {
		if (ad->d_name[ctr] > bd->d_name[ctr]) return 1;
		if (ad->d_name[ctr] < bd->d_name[ctr]) return -1;
		ctr++;
	}

	if (ad->d_name[ctr] > bd->d_name[ctr]) return 1;
	else if (ad->d_name[ctr] < bd->d_name[ctr]) return -1;
	return 0;
}

int sort_long (const void* a, const void* b) 
{
	desc_t* ad = (desc_t*) a;
	desc_t* bd = (desc_t*) b;
	int ctr = 0;

	while (ad->child.d_name[ctr] || bd->child.d_name[ctr]) {
		if (ad->child.d_name[ctr] > bd->child.d_name[ctr]) return 1;
		if (ad->child.d_name[ctr] < bd->child.d_name[ctr]) return -1;
		ctr++;
	}

	if (ad->child.d_name[ctr] > bd->child.d_name[ctr]) return 1;
	else if (ad->child.d_name[ctr] < bd->child.d_name[ctr]) return -1;
	return 0;
}

#endif
