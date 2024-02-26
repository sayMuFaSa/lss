#ifndef VEC_H
#define VEC_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {
	VEC_NOERROR,
	VEC_INVALID,
	VEC_OVERFLOW,
	VEC_MALLOC,
	VEC_REALLOC
} vec_error_t;


extern char* vec_error_msg[];

typedef struct {
	size_t max;
	size_t num;
	size_t size;
	void*  data;
} vec_t;

extern vec_error_t vec_err;

extern int vec_init(vec_t* vec, const size_t size, const size_t max);

extern int vec_push(vec_t* vec, const void* item);

extern void* vec_get(vec_t* vec, const size_t at);

extern void vec_sort(vec_t* vec, int (*comp)(const void* a, const void* b));

extern int vec_pop(vec_t* vec);

extern void vec_deinit(vec_t* vec);

extern char* vec_strerror(vec_error_t err);

#endif

#ifdef VEC_IMPLEMENTATION
#ifndef VEC_INTERNAL_H
#define VEC_INTERNAL_H


//////////////// Error handling ////////////////////////////////////
char* vec_error_msg[] = {
	[VEC_NOERROR] = "Success",
	[VEC_INVALID] = "Invalid arguments passed",
	[VEC_OVERFLOW] = "Operation results in overflow",
	[VEC_MALLOC] = "Malloc failed",
	[VEC_REALLOC] = "Realloc failed"
};

vec_error_t vec_err = VEC_NOERROR;

char* vec_strerror(vec_error_t err)
{
	return vec_error_msg[err];
}
/////////////////////////////////////////////////////////////////////




int vec_init(vec_t* vec, const size_t size, const size_t max)
{
	if (size == 0) {
		vec_err = VEC_INVALID;
		return -1;
	}

	if (SIZE_MAX / size < max) {
		vec_err = VEC_OVERFLOW;
		return -1;
	}

	vec->data = malloc(max * size);
	if (vec->data == NULL) {
		vec_err = VEC_MALLOC;
		return -1;
	}

	vec->size = size;
	vec->max = max;
	return 0;
}

int vec_push(vec_t* vec, const void* item)
{
	const size_t size = vec->size;
	const size_t max  = vec->max;
	const size_t n    = vec->num;
	char* dest        = vec->data;

	if (n >= max) {
		vec->data = realloc(dest, size * max * 2);
		if (vec->data == NULL) {
			vec_err = VEC_REALLOC;
			return -1;
		}

		dest = vec->data;
		vec->max *= 2;
	}

	dest += size * n;
	memcpy(dest, item, size);
	vec->num++;
	return 0;
}

void* vec_get(vec_t* vec, const size_t at)
{
	const size_t size = vec->size;
	const size_t n    = vec->num;
	char* ptr   = vec->data;

	if (at >= n) return 0;

	return ptr + at * size;
}

void vec_sort(vec_t* vec, int (*comp)(const void* a, const void* b))
{
	const size_t n    = vec->num;
	const size_t size = vec->size;
	void* data  = vec->data;

	qsort(data, n, size, comp);
}

int vec_pop(vec_t* vec)
{
	if (vec->num < 1 && vec->num != SIZE_MAX) return -1;

	vec->num--;
	return 0;
}


void vec_deinit(vec_t* vec)
{
	free(vec->data);
	vec->num = 0;
	vec->max = 0;
}

#endif
#endif


// Usefull macros for static typing.
#ifdef VEC_MACROS
#define vec_declare(type,alias)\
typedef struct{\
	size_t max;\
	size_t num;\
	size_t size;\
	type*  data;\
} vec_ ## alias;\
extern int vec_init_ ## alias(vec_ ## alias* vec, const size_t max);\
extern int vec_push_ ## alias(vec_ ## alias* vec, const type* item);\
extern int vec_pop_ ## alias(vec_ ## alias* vec);\
extern type* vec_get_ ## alias(vec_ ## alias* vec, const size_t at);\
extern void vec_sort_ ## alias(vec_ ## alias* vec, int (*comp)(const void* a, const void* b));\
extern void vec_deinit_ ## alias(vec_ ## alias* vec);

#define vec_define(type,alias)\
int vec_init_ ## alias(vec_ ## alias* vec, const size_t max)\
{\
	vec_t* ptr = (vec_t*) vec;\
	return vec_init(ptr, sizeof(type), max);\
}\
\
int vec_push_ ## alias(vec_ ## alias* vec, const type* item)\
{\
	vec_t* ptr = (vec_t*) vec;\
	return vec_push(ptr, item);\
}\
int vec_pop_ ## alias(vec_ ## alias* vec)\
{\
	vec_t* ptr = (vec_t*) vec;\
	return vec_pop(ptr);\
}\
type* vec_get_ ## alias(vec_ ## alias* vec, const size_t at)\
{\
	vec_t* ptr = (vec_t*) vec;\
	return vec_get(ptr, at);\
}\
void vec_sort_ ## alias(vec_ ## alias* vec, int (*comp)(const void* a, const void* b))\
{\
	vec_t* ptr = (vec_t*) vec;\
	vec_sort(ptr, comp);\
}\
void vec_deinit_ ## alias(vec_ ## alias* vec)\
{\
	vec_t* ptr = (vec_t*) vec;\
	vec_deinit(ptr);\
}

#define vec_define_static(type,alias)\
typedef struct{\
	size_t max;\
	size_t num;\
	size_t size;\
	type*  data;\
} vec_ ## alias;\
static int vec_init_ ## alias(vec_ ## alias* vec, const size_t max)\
{\
	vec_t* ptr = (vec_t*) vec;\
	return vec_init(ptr, sizeof(type), max);\
}\
\
static int vec_push_ ## alias(vec_ ## alias* vec, const type* item)\
{\
	vec_t* ptr = (vec_t*) vec;\
	return vec_push(ptr, item);\
}\
static int vec_pop_ ## alias(vec_ ## alias* vec)\
{\
	vec_t* ptr = (vec_t*) vec;\
	return vec_pop(ptr);\
}\
static type* vec_get_ ## alias(vec_ ## alias* vec, const size_t at)\
{\
	vec_t* ptr = (vec_t*) vec;\
	return vec_get(ptr, at);\
}\
static void vec_sort_ ## alias(vec_ ## alias* vec, int (*comp)(const void* a, const void* b))\
{\
	vec_t* ptr = (vec_t*) vec;\
	vec_sort(ptr, comp);\
}\
static void vec_deinit_ ## alias(vec_ ## alias* vec)\
{\
	vec_t* ptr = (vec_t*) vec;\
	vec_deinit(ptr);\
}

#endif
