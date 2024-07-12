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
#include <vector>
#include <algorithm>  

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
        size_t sizeinwords = 0;
     //   segment* next = nullptr;
     //   segment* prev = nullptr;
    };
   // segment* head = nullptr;
   // segment* tail = nullptr;
    std::map<void*,segment*> memory_map;
    segment* find_segment(void*);
   unsigned wordsize = 0;
   bool initialized = false;
public:
    void merge();
    void add_segment(void*, size_t const&);
    void delete_segment(void*);
    void* alloc_segment(void*, size_t const&);
    size_t get_length();
    void* get_list(void*);
    size_t get_bytesize(void*);
    size_t get_wordsize(void*);
    void wipe_map();
    void set_word_size(unsigned const& word_size)
    {
        this->wordsize = word_size;
    }
    void* get_bitmap(void*,int);
    ~MemoryTracker();
};