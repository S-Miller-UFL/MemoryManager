


//MemoryManager.cpp
/*
* Author: Steven Miller
* Date created: October 17, 2023
* Purpose: Function definitions for MemoryManager.h
* Used by: N/A
* Uses: MemoryManager.h
*/

#include <iostream>
#include "MemoryManager.h"


//youre going to need to use some kind of data structure to keep track of 
//memory blocks and holes
//length = word size
//offsets = index
//store pointers to where the blocks are aswell

//its wiser to use an abstract class instead of multi-dimensional arrays or std::pair structures

MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void*)> allocator)
{
    this->wordSize = wordSize;
    this->allocator_type = allocator;

}

//call shutdown in here
MemoryManager::~MemoryManager()
{
    shutdown();
}

//when this is called, you will initialize an array on the heap using a systemcall
//that big block of contiguous memory is what youll use to rent out to functions

//number of bytes to allocate = sizeInWords*wordSize
void MemoryManager::initialize(size_t sizeInWords)
{
    if (sizeInWords > 65536)
    {
       //do something here
    }
    else if (sizeInWords < 0)
    {
        //do something here
    }
    else
    {
        this->alloc_start = malloc(sizeInWords * this->wordSize);
        //hole_list.add_segment(alloc_start,sizeInWords*wordSize);

    }
}

//responsible for actually clearing out the memory
//clear out the heap, the linked list, reset member variables, etc
void MemoryManager::shutdown()
{
}

//request to rent out a certain portion of that block
//this function needs to call getlist function aswell
//return a pointer somewhere in your memory to the newly allocated space
//you then need to free the memory you used in getlist
//whatever function calls getlist needs to free the memory getlist used.
//the allocator function is going to return the offset in words
//you need to convert that offset in words to bytes, then to pointer location
//you will need to use pointer arithmetic here
void* MemoryManager::allocate(size_t sizeInBytes)
{
    
    /*
    char* pntr = (char*)getList();
    for (int i = 0; i < hole_list.get_length(); i++)
    {
        std::cout << pntr[i];
    }
    */

    //call getlist first, you need a list of holes
    //call allocation algorithm, pass in size in words, and pass in list of holes

    return nullptr;
}

//free
void MemoryManager::free(void* address)
{
}

void MemoryManager::setAllocator(std::function<int(int, void*)> allocator)
{
    this->allocator_type = allocator;
}

//until you have your data structure made, and your getlist completed,
//ignore this.
int MemoryManager::dumpMemoryMap(char* filename)
{
    return -1;
}


void* MemoryManager::getList()
{
    /*
   char* list = (char*)hole_list.get_list(alloc_start);
   for (int i = 1; i < hole_list.get_length(); i++)
   {
     std::cout << list[i] << " ";
   }
   

    return list;
    */
    return nullptr;
}

//until you have your data structure made, and your getlist completed,
//ignore this.
void* MemoryManager::getBitmap()
{
    return nullptr;
}

unsigned MemoryManager::getWordSize()
{
    return this->wordSize;
}

void* MemoryManager::getMemoryStart()
{
    return this->alloc_start;
}

unsigned MemoryManager::getMemoryLimit()
{
    return this->memory_limit;
}

//Returns word offset of hole selected by the best fit memory allocation algorithm, 
//and -1 if there is no fit
//call getlist inside 
int bestFit(int sizeInWords, void* list)
{
    //in the allocation algorithms you find the offset of the hole
    return 0;
}

//Returns word offset of hole selected by the worst fit memory allocation algorithm, 
//and -1 if there is no fit.
int worstFit(int sizeInWords, void* list)
{
    return 0;
}