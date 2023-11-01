




//MemoryManager_linkedList.cpp
/*
* Author: Steven Miller
* Date created: October 17, 2023
* Purpose: data structure to keep track of memory blocks and holes.
* Used by: MemoryManager.h
* Uses: N/A
*/

#include <iostream>
#include <list>
class LinkedList
{
private:
    struct segment
    {
        //when the hole is bigger than the requested memory size
        //move the start pointer up by some offset and decrease the size
        bool hole = true;
        uint8_t sizeinbytes = 0;
        void* start= nullptr;
        void* end = nullptr;
        segment* next = nullptr;
        segment* prev = nullptr;
    };
    segment* head = nullptr;
    segment* tail = nullptr;
    uint8_t length = 0;
    std::list<segment*> free_list;

public:
    /*
    void add_segment(void*, uint8_t const&);
    segment* find_segment(void*);
    void delete_segment(void*);
    void* alloc_segment(segment*, uint8_t);
    uint8_t get_length();
    void* get_list(void*);
    uint8_t get_size(void*);
    ~LinkedList();
    */
};