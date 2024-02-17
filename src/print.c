#include "ls.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>

static void print_long(const struct d_info* info, const char* p);

static void print_default(const struct d_info* info);

static int get_format(const struct d_info* info, size_t* rows, size_t* cols, size_t* width);

static void print_per_line(const struct d_info* info);



void print(const struct d_info* info, const char* p, const opt_t opt)
{
	if (opt & LONG) {
		print_long(info, p);
		return;
	}

	if (opt & MULTIPLE)
		printf("%s:\n", p);

	if (opt & ONEPL) {
		print_per_line(info);
		return;
	}

	print_default(info);

}

void print_default(const struct d_info* info)
{
	const struct dirent* data = info->child.data;
	const size_t n = info->child.num;
	size_t rows, cols, width;
	
	if (get_format(info, &rows, &cols, &width))
		return;

	for (size_t j = 0; j < rows; j++) {
		for (size_t i = 0; i < cols; i++) {
			const size_t pos = i * rows + j;
			const char* name = data[pos].d_name;
			const size_t align = width - strlen(name);

			if (pos >= n) {
				putchar('\n'); return;
			}

			printf("%s  ", name);
			for (size_t k = 0; k < align; k++)
				putchar(' ');
		}

		putchar('\n');

	}

}

void print_per_line (const struct d_info* info)
{
	const size_t n = info->child.num;
	const struct dirent* data = info->child.data;

	for (size_t i = 0; i < n; i++)
		puts(data[i].d_name);
}

int get_format (const struct d_info* info, size_t* rows, size_t* cols, size_t* width)
{
	struct winsize size = {0};
	size_t max_name_length = 1;
	const size_t tab = 2;
	const size_t n = info->child.num;
	size_t col_max;
	const struct dirent* data = info->child.data;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1) {
		fprintf(stderr, "get_format: %s", strerror(errno));
		return -1;
	}

	for (size_t i = 0; i < n; i++) {
		const char*  name   = data[i].d_name;
		const size_t length = strlen(name);

		if (max_name_length < length) max_name_length = length;
	}

	col_max = size.ws_col / (max_name_length + tab);

	*rows = (n / col_max) + ((n % col_max) ? 1 : 0);

	*cols = (n / *rows) + ((n % *rows) ? 1 : 0);

	*width = max_name_length;

	return 0;
}


void print_long(const struct d_info* info, const char* p)
{ 
	const struct dirent* data  = info->child.data;
	const size_t n = info->child.num;

	char fpath[PATH_MAX] = {0};
	char link_buf[PATH_MAX] = {0};
	struct stat l_opt;
	char perm[16] = {0}; 
	char mtime[30];

	for (size_t i = 0; i < n; i++) {
		const char *name = data[i].d_name;

		sprintf(fpath, "%s/%s", p, name);
		if (lstat(fpath, &l_opt) != 0) {
			fprintf(stderr, "Can't stat file %s: %s\n", fpath, strerror(errno));
		}
		
		const char *uname = getpwuid(l_opt.st_uid)->pw_name;
		const char *gname = getgrgid(l_opt.st_uid)->gr_name;
		
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

		strftime(mtime, 30,  "%b %e %R", localtime(&l_opt.st_mtim.tv_sec));

		if (perm[0] != 'l') {
			printf("%s %lu %s %-8s %8lu %s %s\n", perm, l_opt.st_nlink, uname , gname, l_opt.st_size, mtime, name);
			continue;
		}

		memset(link_buf, 0, PATH_MAX);

		if (readlink(fpath, link_buf, PATH_MAX) == -1) {
			fprintf(stderr, "While reading soft link %s: %s", fpath, strerror(errno));
			return;
		}

		printf("%s %lu %s %-8s %8lu %s %s -> %s\n", perm, l_opt.st_nlink, uname , gname, l_opt.st_size, mtime, name, link_buf);
	}
}
