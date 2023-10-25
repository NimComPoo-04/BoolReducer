#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "reducer.h"

expr_t *convert_expr(func_t *f);

static uint64_t *reduced = 0;
static int reduced_count = 0;
static char *reduced_check= NULL;

static uint64_t *singleton = 0;
static int singleton_count = 0;

static int bits(uint64_t t)
{
	int k = 0;
	while(t)
	{
		if(t & 1) k++;
		t >>= 1;
	}
	return k == 1;
}

uint64_t deactive(uint64_t a, uint64_t b)
{
	uint32_t ac = (uint32_t)a;
	uint32_t d = (a >> 32) ^ (b >> 32);
	int pos = -1;

	while(d)
	{
		d >>= 1;
		pos++;
	}

	ac = ac & ~(1 << pos);
	uint32_t t = (a >> 32) & ~(1 << pos);

	return ((uint64_t)t << 32) | ac;
}

func_t *temp = 0;

static void reduce_eval()
{
	if(reduced_count == 0)
	{
		free(reduced);
		return;
	}

	uint64_t *new = calloc(sizeof(uint64_t), reduced_count);
	int new_count = 0;

	for(int i = 0; i < reduced_count; i++)
	{
		for(int j = i + 1; j < reduced_count; j++)
		{
			if(bits(reduced[i] ^ reduced[j]))
			{
				new[new_count++] = deactive(reduced[i], reduced[j]);
				reduced_check[i] = 1;
				reduced_check[j] = 1;
			}
		}
	}

	for(int i = 0; i < reduced_count; i++)
	{
		if(!reduced_check[i])
		{
			singleton[singleton_count++] = reduced[i];
		}
	}

	printf("NvO: %d ?= %d\n", new_count, reduced_count);

	// After a while
	free(reduced);
	reduced = new;
	reduced_count = new_count;

	memset(reduced_check, 0, reduced_count);

	reduce_eval();
}

expr_t *convert_expr(func_t *f)
{
	char *buffer = calloc(sizeof(char), 1024);
	int length = 0;
	int size = 1024;

	for(int i = 0; i < singleton_count; i++)
	{
		if(length >= size)
		{
			size = size + 1024;
			buffer  = realloc(buffer, sizeof(char) * size);
		}

		uint32_t ac = (uint32_t)singleton[i];
		uint32_t va = singleton[i] >> 32;

		for(int j = 0; j < f->count; j++)
		{
			if(ac & 1)
			{
				buffer[length++] = f->vars[j];
				if((va & 1) == 0)
					buffer[length++] = '\'';
				buffer[length++] = '.';
			}

			va >>= 1;
			ac >>= 1;
		}
		length--;

		if(i != singleton_count - 1)
			buffer[length++] = '+';
	}

// 	printf("%.*s\n", length, buffer);

	expr_t *e = parse_expr(buffer, length);
	free(buffer);
	return e;
}

expr_t *reduce_func(func_t *f)
{
	reduced = calloc(sizeof(uint64_t), 1 << f->count);
	reduced_check =  calloc(sizeof(char), 1 << f->count);
	singleton = calloc(sizeof(uint64_t), 1 << f->count);

	uint32_t action = (1 << f->count) - 1;

	int k = 0;

	temp = f;

	reduced_count = 0;
	for(uint32_t i = 0; i < (uint32_t)(1 << f->count); i++)
	{
		if(f->min_terms[i])
		{
			reduced[reduced_count++] = ((uint64_t)i << 32) | action;
			k++;
		}
	}

	puts("\n== UNREDUCED FORM ==\n");
	for(int i = 0; i < reduced_count; i++)
	{
		uint64_t k = reduced[i];
		while(k)
		{
			printf("%d", k & 1);
			k >>= 1;
		}
		puts("");
	}
	puts("");

	if(k == 0)
	{
		free(reduced);
		free(reduced_check);
		free(singleton);
		return parse_expr("0", 1);
	}
	if(k == 1 << f->count)
	{
		free(reduced);
		free(reduced_check);
		free(singleton);
		return parse_expr("1", 1);
	}

	reduce_eval();

	expr_t *e = convert_expr(f);

	puts("\n== REDUCED FORM ==\n");
	for(int i = 0; i < singleton_count; i++)
	{
		uint64_t k = singleton[i];
		while(k)
		{
			printf("%d", k & 1);
			k >>= 1;
		}
		puts("");
	}
	puts("");

	free(reduced_check);
	free(singleton);

	return e;
}
