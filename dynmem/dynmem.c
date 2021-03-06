#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>
#include "dynmem.h"
#include "structs.h"


void* malloc(size_t size)
{
	size_t pageSize = (size_t)getpagesize();

	printf("size to allocate: %d\n", (int)size);

	block* dest = sfree(size);

	if( dest == NULL ){
		size_t size1 = size;
		size1 += blockSize + areaSize;
		size1 += (pageSize - (size1 % pageSize)) % pageSize;

		void* ptr = mmap(NULL, size1, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if( ptr == MAP_FAILED ){
			perror("mmap error\n");
			return NULL;
		}

		printf("size allocated: %d\n", (int)size1);

		createArea(ptr, size1, size);

		return ptr + areaSize + blockSize;
	}	
	else {
		divideBlock(dest, size);

		void* ptr = (void*) dest;
		return ptr + blockSize;
	}
}

void* calloc(size_t count, size_t size)
{
	return malloc(count * size);
}

void* realloc(void* ptr, size_t size)
{
	return NULL;
}

int posix_memalign(void** memptr, size_t alignment, size_t size)
{
	return 0;
}

void free1(void* ptr)
{
	ptr -= blockSize;
	block* ptr1 = (block*)ptr;
	
	ptr1->size *= -1;
}