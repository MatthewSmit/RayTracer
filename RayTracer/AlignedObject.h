#pragma once
#include <cstring>

struct alignas(16) AlignedObject
{
	void* operator new(size_t);
	void* operator new[](size_t);
	void operator delete(void*);
	void operator delete[](void*);
};