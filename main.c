#include "ls.h"
#include <getopt.h>

int main(int argc, char* argv[]) {
	struct d_info info = {0};
	const char *here = ".";
	const opt_t opt = parse(argc, argv);

	if (opt & UNKNOWN) return 2;

	if (argc - optind == 0)
		return process(&info, here, opt);

	for (int i = optind; i < argc; i++) {
		const int rv = process(&info, argv[i], opt);
		if (rv) return rv;
	}
}
