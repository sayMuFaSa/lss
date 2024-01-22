#ifndef LS_H
#define LS_H

#include <dirent.h>
#include <sys/stat.h>

enum options {
	DEF,
	UNKNOWN = 1u,
	ALL = UNKNOWN << 1,
	LONG = ALL << 1,
	ONEPL = LONG << 1
};

struct description {
	struct dirent child;
	struct stat stats;
};

typedef struct description desc_t;

typedef enum options opt_t;

struct d_info {
	opt_t opt;
	size_t size;
	size_t num;
	size_t max;
	struct dirent* child;
	desc_t* desc;
};

enum err {
	noerror, mem
};

typedef enum err err_t;

void printfile(const struct d_info *__restrict__ info, const char *__restrict__ p); // file path

err_t rdir(struct d_info *__restrict__ info, const char *__restrict__ p);

void print(const struct d_info *__restrict__ info, const char *__restrict__ p);

int sort_def (const void* a, const void* b);

int sort_long (const void* a, const void* b);

int init(struct d_info* info, const opt_t opt, const size_t min);

opt_t parse(const int argc, char* argv[]);

int ls (struct d_info *__restrict__ info, const char *__restrict__ target);

err_t stats(struct d_info *__restrict__ info , const char *__restrict__ p);

int alloc(struct d_info* info);

#endif
