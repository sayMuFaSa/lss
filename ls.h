#ifndef LS_H
#define LS_H


#include <dirent.h>



enum options {
	DEF,
	UNKNOWN = 1u,
	ALL = UNKNOWN << 1,
	LONG = ALL << 1,
	ONEPL = LONG << 1
};

typedef enum options opt_t;

struct d_info {
	size_t num;
	size_t max;
	struct dirent* child;
};

enum err {
	noerror, mem
};

typedef enum err err_t;

void printfile(const char *__restrict__ p, const char *__restrict__ f); // file path

err_t rdir(const char *__restrict__ p, struct d_info *__restrict__ info);

void print(const struct d_info *__restrict__ info, const char *__restrict__ p, opt_t opt);

int case_in (const void* a, const void* b);

opt_t parse(const int argc, char* argv[]);

int process (struct d_info *__restrict__ info, const char *__restrict__ direct, const opt_t opt);

#endif
