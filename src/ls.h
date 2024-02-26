#ifndef LS_H
#define LS_H

#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define VEC_MACROS
#include "vec.h"

typedef enum {
	DEF,
	UNKNOWN = 1u,
	ALL = UNKNOWN << 1,
	LONG = ALL << 1,
	ONEPL = LONG << 1,
	MULTIPLE = ONEPL << 1
} opt_t;

vec_declare(struct dirent,dirent)

struct d_info {
	vec_dirent child;
	struct stat* stats;
};



int list(struct d_info* info, const char* p, const opt_t opt);

void print(const struct d_info* info, const char* p, const opt_t opt);

opt_t parse(const int argc, char* argv[]);

int ls (struct d_info* info, const char* target, const opt_t opt);

int get_stats(struct d_info* info , const char* p);

#endif
