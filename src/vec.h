#ifndef VEC_H
#define VEC_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef enum {
	VEC_NOERROR,
	VEC_INVALID,
	VEC_OVERFLOW,
	VEC_MALLOC
} vec_error_t;


extern char* vec_error_msg[];

typedef struct {
	size_t num;
	size_t max;
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

#ifndef VEC_IMPLEMENTATION
#define vec_define(type,alias)\
typedef struct{\
	vec_t vec;\
} vec_ ## alias;\
extern int vec_init_ ## alias(vec_ ## alias* vec, const size_t max);\
extern int vec_push_ ## alias(vec_ ## alias* vec, const type* item);\
extern int vec_pop_ ## alias(vec_ ## alias* vec);\
extern type* vec_get_ ## alias(vec_ ## alias* vec, const size_t at);\
extern void vec_sort_ ## alias(vec_ ## alias* vec, int (*comp)(const void* a, const void* b));\
extern void vec_deinit_ ## alias(vec_ ## alias* vec);
#endif


#endif

#ifdef VEC_IMPLEMENTATION

#ifndef VEC_INTERNAL_H
#define VEC_INTERNAL_H


//////////////// Error handling ////////////////////////////////////
char* vec_error_msg[] = {
	[VEC_NOERROR] = "Success",
	[VEC_INVALID] = "Invalid arguments passed",
	[VEC_OVERFLOW] = "Operation results in overflow",
	[VEC_MALLOC] = "Malloc failed"
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
		vec->data = realloc(vec->data, size * max * 2);
		if (vec->data == NULL)
			return -1;

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

#define vec_define(type,alias)\
typedef struct{\
	vec_t vec;\
} vec_ ## alias;\
\
int vec_init_ ## alias(vec_ ## alias* vec, const size_t max)\
{\
	return vec_init(&vec->vec, sizeof(type), max);\
}\
\
int vec_push_ ## alias(vec_ ## alias* vec, const type* item)\
{\
	return vec_push(&vec->vec, item);\
}\
int vec_pop_ ## alias(vec_ ## alias* vec)\
{\
	return vec_pop(&vec->vec);\
}\
type* vec_get_ ## alias(vec_ ## alias* vec, const size_t at)\
{\
	return vec_get(&vec->vec, at);\
}\
void vec_sort_ ## alias(vec_ ## alias* vec, int (*comp)(const void* a, const void* b))\
{\
	vec_sort(&vec->vec, comp);\
}\
void vec_deinit_ ## alias(vec_ ## alias* vec)\
{\
	vec_deinit(&vec->vec);\
}

#endif
#endif
