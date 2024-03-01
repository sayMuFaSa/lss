#ifndef STR_H
#define STR_H

#include <string.h>
#include <stdlib.h>

typedef enum {
	STR_NOERROR,
	STR_MALLOC
} str_err_t;

typedef struct {
	size_t max;
	size_t length;
	char*  data;
} str_t;

extern str_err_t str_err;


str_t* str_create(const char*);
void   str_destroy(str_t*);

/************************ Implementation ***************************/
#ifdef STR_IMPLEMENTATION
#ifndef STR_INTERNAL_H
#define STR_INTERNAL_H
str_err_t str_err = STR_NOERROR;

str_t* str_create(const char* c_str)
{
	const size_t size = strlen(c_str) + 1;
	str_t* string = malloc(sizeof(str_t));

	if (string == NULL) {
		str_err = STR_MALLOC;
		return 0;
	}

	if (size == 1)
		return string;

	if ((string->data = malloc(size)) == NULL) {
		str_err = STR_MALLOC;
		free(string);
		return 0;
	}

	memcpy(string->data, c_str, size);
	string->data[size - 1] = 0;
	string->length = size;
	string->max    = size;

	return string;
}

void str_destroy(str_t* string)
{
	free(string->data);
	free(string);
}
#endif
#endif
/************************************************************************/

#endif
