#pragma once




//MemoryTracker.h
/*
* Author: Steven Miller
* Date created: October 29, 2023
* Purpose: data structure to keep track of memory blocks and holes, replaces linked list
* Used by: MemoryManager.h
* Uses: N/A
*/
#include <iostream>
#include <map>

class MemoryTracker
{
private:
    struct segment
    {
        //when the hole is bigger than the requested memory size
        //move the start pointer up by some offset and decrease the size
        bool hole = true;
        size_t sizeinbytes = 0;
        void* start = nullptr;
        void* end = nullptr;
     //   segment* next = nullptr;
     //   segment* prev = nullptr;
    };
   // segment* head = nullptr;
   // segment* tail = nullptr;
    std::map<void*,segment*> memory_map;
    void merge();
public:
    void add_segment(void*, size_t const&);
    segment* find_segment(void*);
    void delete_segment(void*);
    void* alloc_segment(void*, size_t const&);
    size_t get_length();
    void* get_list(void*);
    size_t get_size(void*);
    ~MemoryTracker();
};