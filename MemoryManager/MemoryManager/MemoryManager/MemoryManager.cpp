


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
typedef int file_descriptor;

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
    this->free_map.set_word_size(wordSize);
    this->taken_map.set_word_size(wordSize);
}

//call shutdown in here
MemoryManager::~MemoryManager()
{
    if(this->memory_limit > 0)
    {
        this->shutdown();
    }
}

//when this is called, you will initialize an array on the heap using a systemcall
//that big block of contiguous memory is what youll use to rent out to functions
//number of bytes to allocate = sizeInWords*wordSize
void MemoryManager::initialize(size_t sizeInWords)
{
    if (sizeInWords > 65536)
    {
         this->alloc_start = nullptr;
    }
    else if (sizeInWords < 0)
    {
        this->alloc_start = nullptr;
    }
    else
    {
        if (this->memory_limit > 0)
        {
            this->shutdown();
        }
        this->numberofwords = sizeInWords;
        this->alloc_start = (void*)(new uint8_t[this->numberofwords * this->wordSize]);
        this->free_map.add_segment(this->alloc_start, this->numberofwords * this->wordSize);
        this->memory_limit = this->numberofwords * this->wordSize;
        this->bytes_memory_left = this->numberofwords * this->wordSize;

    }
}

//responsible for actually clearing out the memory
//clear out the heap, the linked list, reset member variables, etc
void MemoryManager::shutdown()
{
    this->free_map.wipe_map();
    this->taken_map.wipe_map();
    delete[](uint8_t*)this->alloc_start;
    this->memory_limit = 0;
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
    if (sizeInBytes <= this->memory_limit && sizeInBytes <= bytes_memory_left && sizeInBytes != 0)
    {
        //call getlist first, you need a list of holes
        void* list = this->getList();
        //call allocation algorithm, pass in size in words, and pass in list of holes
        int offsetwords = this->allocator_type(sizeInBytes / this->wordSize, list);
        delete[](uint16_t*)list;
        if (offsetwords == -1)
        {
            return nullptr;
        }
        else
        {
            void* alloc_new = (uint8_t*)this->alloc_start + (offsetwords * wordSize);
            size_t numberofwords = this->free_map.get_wordsize((uint8_t*)this->alloc_start + (offsetwords * wordSize));
            void* allocated = this->free_map.alloc_segment((uint8_t*)this->alloc_start + (offsetwords * wordSize), sizeInBytes);
            this->taken_map.add_segment(allocated, sizeInBytes);
            this->bytes_memory_left = this->bytes_memory_left - sizeInBytes;
            return allocated;
        }
    }
    else
    {
        return nullptr;
    }
}

//free
void MemoryManager::free(void* address)
{
    size_t sizeinbytes = this->taken_map.get_bytesize(address);
     if (sizeinbytes != -1)
        {
            this->taken_map.delete_segment(address);
            this->free_map.add_segment(address, sizeinbytes);
            this->free_map.merge();
            this->bytes_memory_left = this->bytes_memory_left + sizeinbytes;
        }
}

void MemoryManager::setAllocator(std::function<int(int, void*)> allocator)
{
    this->allocator_type = allocator;
}

//until you have your data structure made, and your getlist completed,
//ignore this.
int MemoryManager::dumpMemoryMap(char* filename)
{
     uint16_t* list = (uint16_t*)this->getList();
    if (list == nullptr)
    {
        return -1;
    }
    else if(list[0] == 0)
    {
        std::string stringlist = "";
        const char* str = stringlist.c_str();
        file_descriptor fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0777);
        ssize_t status = write(fd, (void*)str, (size_t)stringlist.size() * sizeof(char));
        delete[](uint16_t*)list;
        if (status == -1)
        {
            close(fd);
            return -1;
        }
        else
        {
            close(fd);
            return 0;
        }
    }
    else
    {
        std::string stringlist;
        for (int i = 1; i < (list[0] * 2); i += 2)
        {
            stringlist = stringlist + ' ';
            stringlist = stringlist + '[';
            stringlist = stringlist + std::to_string(list[i]);
            stringlist = stringlist + ',';
            stringlist = stringlist + ' ';
            stringlist = stringlist + std::to_string(list[i + 1]);
            stringlist = stringlist + ']';
            stringlist = stringlist + ' ';
            stringlist = stringlist + '-';

        }
        stringlist = stringlist.substr(1, stringlist.size() - 3);
        const char* str = stringlist.c_str();
        file_descriptor fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0777);
        ssize_t status = write(fd, (void*)str, (size_t)stringlist.size() * sizeof(char));
        delete[](uint16_t*)list;
        if (status == -1)
        {
            close(fd);
            return -1;
        }
        else
        {
            close(fd);
            return 0;
        }
    }
delete[](uint16_t*)list;
return -1;

}


void* MemoryManager::getList()
{
    return this->free_map.get_list(this->alloc_start);
}

//until you have your data structure made, and your getlist completed,
//ignore this.
void* MemoryManager::getBitmap()
{
    return this->free_map.get_bitmap(this->alloc_start, this->numberofwords);
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
int bestFit(int sizeInWords, void* list)
{
    //in the allocation algorithms you find the offset of the hole
    uint16_t* listholes = (uint16_t*)list;
    int sizearr = (listholes[0] * 2) + 1;
    int offset = 0;
    int size = 65537; //since 65537 is larger than the largest possible hole, any hole in the list will automatically be less than this.
    for (int i = 1; i < sizearr; i++)
    {
        //find hole that fits our requested space and is smallest
        //if even index
        if (i % 2 == 0)
        {
            if ((listholes[i] >= sizeInWords) && (listholes[i]< size))
            {
                offset = listholes[i-1];
                size = listholes[i];
            }
        }
        
    }
    if (size == 65537)
    {
        return -1;
    }

    return offset;
}

//Returns word offset of hole selected by the worst fit memory allocation algorithm, 
//and -1 if there is no fit.
int worstFit(int sizeInWords, void* list)
{
    //in the allocation algorithms you find the offset of the hole
    uint16_t* listholes = (uint16_t*)list;
    int sizearr = (listholes[0] * 2) + 1;
    int offset = 0;
    int size = 0;
    for (int i = 1; i < sizearr; i++)
    {
        //find hole that fits our requested space and is smallest
        //if even index
        if (i % 2 == 0)
        {
            if ((listholes[i] >= sizeInWords) && (listholes[i] > size))
            {
                offset = listholes[i - 1];
                size = listholes[i];
            }
        }

    }
    if (size == 0)
    {
        return -1;
    }
    return offset;
}
