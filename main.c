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

void printfile(const char *__restrict__ p, const char *__restrict__ f); // file path

int rdir(const char *__restrict__ p, struct d_info *__restrict__ info);

void print(struct d_info *info, const char *p, unsigned int opt);

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
		print(&info, here, opt);
	}

	for (int i = optind; i < argc; i++) {
		struct stat file;
		if (stat(argv[i], &file) == 0) {
			if ((file.st_mode & S_IFMT) != S_IFDIR) {
				printf("%s ", argv[i]);
				continue;
			}
		} else {
			fprintf(stderr, "%s: %s: %s", argv[0], argv[i], strerror(errno));
			continue;
		}

		rdir(argv[i], &info);
		if (argc - optind > 1) {
			printf("%s:\n", argv[i]);
		}

		print(&info, argv[i], opt);
	}

}

int rdir(const char *__restrict__ p, struct d_info *__restrict__ info) {
	DIR* dir = opendir(p);
	struct dirent* entry;
	int rv = 0;
	info->l = malloc(2000 * sizeof(struct dirent));
	info->num = 0;

	if (dir == NULL) {
		fprintf(stderr, "%s: %s\n", p,  strerror(errno));
		return 1;
	}

	do {
		entry = readdir(dir);

		if (entry != 0) {
			memcpy(info->l + info->num, entry, sizeof(struct dirent));
			info->num++;
		} else if (errno != 0) {
			fprintf(stderr, "Readdir: %s", strerror(errno));
			free(info->l);
			rv = 1;
		}

	} while(entry != 0);

	closedir(dir);

	putchar('\n');

	return rv;
}

void print(struct d_info *__restrict__ info, const char *__restrict__ p, unsigned int opt) {
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
	char mdate[100] = {0};
	char size[100] = {0};
	struct passwd *uid;
	struct group *gid;
	char mtime[30];
	sprintf(fpath, "%s/%s", p, f);
	lstat(fpath, &l_opt);

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

	uid = getpwuid(l_opt.st_uid);
	gid = getgrgid(l_opt.st_gid);

	strftime(mtime, 30,  "%b %e %R", localtime(&l_opt.st_mtim.tv_sec));

	printf("%s %lu %s %s %lu %s %s\n", perm, l_opt.st_nlink, uid->pw_name, gid->gr_name, l_opt.st_size, mtime, f);
}
