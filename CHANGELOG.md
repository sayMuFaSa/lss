# Changelog

## v0.1.0

### Use string library. - 2024-03-02
- Now a handmade string library is used for group and user names.
- All calls to fprintf are replaced by perror where appropriate.
- More readable declaration of opt_t enum.
- Dockerfile's added.
- Configure script will link the executable statically on musl systems.
- More readable variable names for user and group data.

### Optimize the user and group outputing. - 2024-02-27
- Vectors are now used for collecting user and group data so systemcalls for fetching the owner and group are not invoked for each individual file.
- The macros and functions are now separated in the vector library, macros are defined only if requested by defning VEC_MACROS before the header.
- The comparison function is now completely readable.
- Observing silent commands in Makefile.

### Use vector library. - 2024-02-22
- A dedicated single header vector(growable array) library is written and used.
- Sorting pattern modified in the function alpha.


### It works! See changelog for details. - 2024-02-18
All the source code is now under the src/ directory. Configure script is introduced so you can compile the project in a separate build directory.
Some compilation options can be set in config.mk file. Now vec_t is used for handling allocation instead of alloc function.
