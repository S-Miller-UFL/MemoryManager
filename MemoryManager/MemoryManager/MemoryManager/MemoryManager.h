


//MemoryManager.h
/*
* Author: Steven Miller
* Date created: October 17, 2023
* Purpose: Function declarations for MemoryManager.cpp
* Used by: MemoryManager.cpp
* Uses: MemoryManager_LinkedList.h, MemoryManager_AllocAlgorithms.h
*/

#pragma once
#include <functional>
#include "MemoryTracker.h"
#include<unistd.h>
#include <string>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
class MemoryManager
{
public:
	//Constructor; 
	//sets native word size (in bytes, for alignment) 
	//and default allocator for finding a memory hole
	MemoryManager(unsigned wordSize, std::function<int(int, void*)> allocator);

	//Releases all memory allocated by this object without leaking memory.
	~MemoryManager();

	//Instantiates block of requested size, 
	//no larger than 65536 words; cleans up previous block if applicable
	void initialize(size_t sizeInWords);

	//Releases memory block acquired during initialization,if any.
	//This should only include memory created for
	//long term use not those that returned such as getList() or 
	//getBitmap() as whatever is calling those should delete it instead.
	void shutdown();

	//Allocates a memory using the allocator function.
	//If no memory is available or size is invalid, returns nullptr
	void* allocate(size_t sizeInBytes);

	//Frees the memory block within the memory manager so that it can be reused.
	void free(void* address);

	//Changes the allocation algorithm to identifying the memory hole to use for allocation.
	void setAllocator(std::function<int(int, void*)> allocator);

	//Uses standard POSIX calls to write hole list to filename as text, returning -1 on error and 0 if successful
	int dumpMemoryMap(char* filename);

	//Returns an array of information(in decimal) 
	//about holes for use by the allocator function(little - Endian).
	//Offset and length are in words.
	//If no memory has been allocated, the function should return a NULL pointer.
	void* getList();

	//Returns a bit - stream of bits in terms of an array 
	// representing whether words are used(1) or free(0).The
	//first two bytes are the size of the bitmap(little - Endian);
	//the rest is the bitmap, word - wise.
	void* getBitmap();

	//Returns the word size used for alignment.
	unsigned getWordSize();

	//Returns the byte-wise memory address of the beginning of the memory block.
	void* getMemoryStart();

	//Returns the byte limit of the current memory block.
	unsigned getMemoryLimit();

private:
	unsigned wordSize = 0;
	std::function<int(int, void*)> allocator_type;
	unsigned int memory_limit = 0;
	void* alloc_start = nullptr;
	void* hole_pointer = nullptr;
	MemoryTracker free_map;
	MemoryTracker taken_map;
	size_t numberofwords = 0;
	unsigned int bytes_memory_left = 0;

};

//Returns word offset of hole selected by the best fit memory allocation algorithm, 
//and -1 if there is no fit
int bestFit(int sizeInWords, void* list);

//Returns word offset of hole selected by the worst fit memory allocation algorithm, 
//and -1 if there is no fit.
int worstFit(int sizeInWords, void* list);

