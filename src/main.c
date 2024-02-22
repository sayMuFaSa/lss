#define VEC_IMPLEMENTATION
#include "ls.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int init(struct d_info* info, const size_t min);
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
	if (vec_init_dirent(&info->child, min)) {
		const char *err_msg = (vec_err == VEC_MALLOC) 
			                ? strerror(errno) 
							:  vec_strerror(vec_err);

		fprintf(stderr, "Init: %s", err_msg);
		return -1;
	}

	return 0;
}


void deinit(struct d_info* info, const opt_t opt)
{
	vec_deinit_dirent(&info->child);

	if (opt & LONG)
		free(info->stats);
}
