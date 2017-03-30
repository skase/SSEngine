#pragma once

#include "Core\CoreMinimal.h"

// TO DO
class StackAllocator
{
public:
	static StackAllocator *CreateStackAllocator(void *i_pMemory, const size_t i_sizeMemory);
	
	~StackAllocator();

	void destroy();

	void freeToMarker();
	void alloc(const size_t i_size);
	
	void clear();
	void* getMarker() { return lastMark_; }

private:
	StackAllocator(void *i_pMemory, const size_t i_sizeMemory);
	FORCEINLINE StackAllocator(const StackAllocator &i_other) {}

private:
	void * stackMemoryBase_;
	void * memorySize_;
	void * lastMark_;
	void * currentLocation_;
};
