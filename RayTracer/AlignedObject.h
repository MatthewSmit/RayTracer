#pragma once

struct alignas(16) AlignedObject
{
public:
	void* operator new(size_t);
	void* operator new[](size_t);
	void operator delete(void*);
	void operator delete[](void*);
};