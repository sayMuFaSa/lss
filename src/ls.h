#ifndef LS_H
#define LS_H

#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define VEC_MACROS
#include "vec.h"
#include "str.h"

typedef enum {
	DEF,
	UNKNOWN  = 1u,
	ALL      = 1u << 1,
	LONG     = 1u << 2,
	ONEPL    = 1u << 3,
	MULTIPLE = 1u << 4
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
