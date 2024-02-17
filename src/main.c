#include "ls.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int init(struct d_info* info, const size_t min);
static int vec_init (vec_t* vec, const size_t size, const size_t max);
static void vec_destroy (vec_t* vec);
static void deinit(struct d_info* info, const opt_t opt);


int main(int argc, char* argv[])
{
	const char *here = ".";
	struct d_info info = {0};
	const opt_t opt = parse(argc, argv);
	int rv = 0;

	if (opt == UNKNOWN) return 1;

	if (init(&info, 2) != 0)
		return 2;

	for (int i = optind; i < argc; i++) {
		if (ls(&info, argv[i], opt))
			rv = 2;
	}

	if (argc - optind == 0)
		if (ls(&info, here, opt))
			rv = 2;

	deinit(&info, opt);
	return rv;
}

int init (struct d_info* info, const size_t min) 
{
	const size_t size = sizeof(struct dirent);

	if (vec_init(&info->child, size, min)) {
		fprintf(stderr, "Init: %s", strerror(errno));
		return -1;
	}

	return 0;
}

int vec_init(vec_t* vec, const size_t size, const size_t max)
{
	vec->data = malloc(max * size);
	if (vec->data == NULL) 
		return -1;

	vec->max = max;
	vec->size = size;
	return 0;
}

void vec_destroy(vec_t* vec)
{
	free(vec->data);
}

void deinit(struct d_info* info, const opt_t opt)
{
	vec_destroy(&info->child);

	if (opt & LONG)
		free(info->stats);
}
