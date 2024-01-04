#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define ALL 0b1u // ALL
#define LONG 0b1u << 1 // long output

struct d_info {
	int num;
	struct dirent* l;
};

enum err {
	noerror, mem
};

void printfile(const char *__restrict__ p, const char *__restrict__ f); // file path

int rdir(const char *__restrict__ p, struct d_info *__restrict__ info);

void print(const struct d_info *__restrict__ info, const char *__restrict__ p, unsigned int opt);

int case_in (const void* a, const void* b) {
	struct dirent* ad = (struct dirent*) a;
	struct dirent* bd = (struct dirent*) b;
	int ctr = 0;

	while (ad->d_name[ctr] || bd->d_name[ctr]) {
		if (ad->d_name[ctr] > bd->d_name[ctr]) return 1;
		else if (ad->d_name[ctr] < bd->d_name[ctr]) return -1;
		ctr++;
	}

	if (ad->d_name[ctr] > bd->d_name[ctr]) return 1;
	else if (ad->d_name[ctr] < bd->d_name[ctr]) return -1;
	return 0;
}


int main(int argc, char* argv[]) {
	struct d_info info = {0};
	const char *here = ".";
	unsigned int opt = 0;
	int arg;

	while ((arg = getopt(argc, argv, "laf")) != -1) {
		switch (arg) {
			case 'l': opt |= LONG; break;
			case 'a': opt |= ALL; break;
			case '?': fprintf(stderr, "Unknown option\n"); break;
		}
	}

	if (argc - optind == 0) {
		rdir(here, &info);
		qsort(info.l, info.num, sizeof(struct dirent), &case_in);
		print(&info, here, opt);
		free(info.l);
	}

	for (int i = optind; i < argc; i++) {
		struct stat file;
		if (stat(argv[i], &file) == 0) {
			if ((file.st_mode & S_IFMT) != S_IFDIR) {
				printf("%s ", argv[i]);
				continue;
			}
		} else {
			fprintf(stderr, "%s: %s: %s\n", argv[0], argv[i], strerror(errno));
			continue;
		}

		if (rdir(argv[i], &info) == mem) {
			return 1;
		}

		if (argc - optind > 1) {
			printf("%s:\n", argv[i]);
		}

		print(&info, argv[i], opt);
		free(info.l);
	}

}

int rdir(const char *__restrict__ p, struct d_info *__restrict__ info) {
	DIR* dir = opendir(p);
	struct dirent* entry;
	int rv = 0;
	info->num = 0;

	if (dir == NULL) {
		fprintf(stderr, "%s: %s\n", p,  strerror(errno));
		return 1;
	}

	
	info->l = malloc(2000 * sizeof(struct dirent));

	if (info->l == 0) {
		fprintf(stderr, "malloc: %s\n", strerror(errno));
		closedir(dir);
		return mem;
	}

	do {
		entry = readdir(dir);

		if (entry != 0) {
			memcpy(info->l + info->num, entry, sizeof(struct dirent));
			info->num++;
		} else if (errno != 0) {
			fprintf(stderr, "Error while processing %s: %s\n", p, strerror(errno));
			rv = 1;
		}

	} while(entry != 0);

	closedir(dir);

	putchar('\n');

	return rv;
}

void print(const struct d_info *__restrict__ info, const char *__restrict__ p, unsigned int opt) {
	for (int i = 0; i < info->num; i++) {
		if (!(opt & ALL) && info->l[i].d_name[0] == '.') continue;
		if (opt & LONG) printfile(p, info->l[i].d_name);
		else printf("%s ", info->l[i].d_name);
	}
	
	if (!(opt & LONG)) putchar('\n');
}

void printfile(const char *__restrict__ p, const char *__restrict__ f) { // p is parent and f is file
	char fpath[100] = {0};
	struct stat l_opt;
	char perm[20] = {0};
	char mtime[30];
	// struct passwd *uid;
	// struct group *gid;

	static int muid = 0; // memorize
	static char muname[30] = {0}; // memorize
	static char *uname = NULL;

	static int mgid = 0; // memorize
	static char mgname[30] = {0}; // memorize
	static char *gname = NULL;

	sprintf(fpath, "%s/%s", p, f);
	if (lstat(fpath, &l_opt) != 0) {
		fprintf(stderr, "Can't stat file %s: %s\n", fpath, strerror(errno));
	}

	switch (l_opt.st_mode & S_IFMT) {
		case S_IFREG: perm[0] = '-'; break;
		case S_IFDIR: perm[0] = 'd'; break;
		case S_IFLNK: perm[0] = 'l'; break;
		case S_IFCHR: perm[0] = 'c'; break;
		case S_IFBLK: perm[0] = 'b'; break;
		case S_IFSOCK: perm[0] = 's'; break;
		case S_IFIFO: perm[0] = 'p'; break;
	}

	perm[1] = (l_opt.st_mode & S_IRUSR) ? 'r' : '-';
	perm[2] = (l_opt.st_mode & S_IWUSR) ? 'w' : '-';
	perm[3] = (l_opt.st_mode & S_IXOTH && l_opt.st_mode & S_ISUID) ? 's' // special permission
		: (l_opt.st_mode & S_IXOTH) ? 'x' : (l_opt.st_mode & S_ISUID)
		? 'S' : '-';

	perm[4] = (l_opt.st_mode & S_IRGRP) ? 'r' : '-';
	perm[5] = (l_opt.st_mode & S_IWGRP) ? 'w' : '-';
	perm[6] = (l_opt.st_mode & S_IXOTH && l_opt.st_mode & S_ISGID) ? 's' // special permission
		: (l_opt.st_mode & S_IXOTH) ? 'x' : (l_opt.st_mode & S_ISGID)
		? 'S' : '-';

	perm[7] = (l_opt.st_mode & S_IROTH) ? 'r' : '-';
	perm[8] = (l_opt.st_mode & S_IWOTH) ? 'w' : '-';
	perm[9] = (l_opt.st_mode & S_IXOTH && l_opt.st_mode & S_ISVTX) ? 't' // sticky bit
		: (l_opt.st_mode & S_IXOTH) ? 'x' : (l_opt.st_mode & S_ISVTX)
		? 'T' : '-';

	// uid = getpwuid(l_opt.st_uid);
	
	if (muid == l_opt.st_uid) {
		uname = muname;
	} else {
		strcpy(muname, getpwuid(l_opt.st_uid)->pw_name);
		uname = muname;
	}

	// gid = getgrgid(l_opt.st_gid);

	strftime(mtime, 30,  "%b %e %R", localtime(&l_opt.st_mtim.tv_sec));

	if (mgid == l_opt.st_gid) {
		gname = mgname;
	} else {
		strcpy(mgname, getgrgid(l_opt.st_uid)->gr_name);
		gname = mgname;
	}


	// printf("%s %lu %s %-8s %8lu %s %s\n", perm, l_opt.st_nlink, uid->pw_name , gid->gr_name, l_opt.st_size, mtime, f);
	
	printf("%s %lu %s %-8s %8lu %s %s\n", perm, l_opt.st_nlink, uname , gname, l_opt.st_size, mtime, f);
}
