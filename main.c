#include "ls.h"
#include <getopt.h>

int main(int argc, char* argv[])
{
	const char *here = ".";
	struct d_info info = {0};
	const opt_t opt = parse(argc, argv);

	if (opt == UNKNOWN) return 2;

	if (init(&info, opt, 2) != 0)
		return 2;

	if (argc - optind == 0){
		return ls(&info, here);
	}

	for (int i = optind; i < argc; i++) {
		const int rv = ls(&info, argv[i]);
		if (rv) return rv;
	}
}
