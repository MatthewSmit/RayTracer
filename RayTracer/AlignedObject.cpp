#include "AlignedObject.h"

#include <new>

#if !defined(_MSC_VER)
#define _aligned_malloc(size, alignment) aligned_alloc((alignment), (size))
#define _aligned_free(ptr) free(ptr)
#endif

void* AlignedObject::operator new(size_t size)
{
	auto storage = _aligned_malloc(size, 16);
	if (storage == nullptr)
		throw std::bad_alloc();

	return storage;
}

void* AlignedObject::operator new[](size_t size)
{
	auto storage = _aligned_malloc(size, 16);
	if (storage == nullptr)
		throw std::bad_alloc();

	return storage;
}

void AlignedObject::operator delete(void* data)
{
	_aligned_free(data);
}

void AlignedObject::operator delete[](void* data)
{
	_aligned_free(data);
}
