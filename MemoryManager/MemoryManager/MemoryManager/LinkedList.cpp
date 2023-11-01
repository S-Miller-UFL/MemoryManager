




//MemoryManager_linkedList.cpp
/*
* Author: Steven Miller
* Date created: October 17, 2023
* Purpose: data structure to keep track of memory blocks and holes.
* Used by: MemoryManager.h
* Uses: N/A
*/

#include "LinkedList.h"
#include <stddef.h>
#include <algorithm>




//shitty manually made linked list version
/*
void LinkedList::add_segment(void* start, uint8_t const& sizeinbytes)
{
    segment* new_segment = new segment;
    new_segment->start = start;
    new_segment->end = (intptr_t*)start + sizeinbytes;
    this->free_list.push_back(new_segment);
}

LinkedList::segment* LinkedList::find_segment(void* target)
{
    auto iterator = this->free_list.end();
    for (;iterator != this->free_list.begin(); iterator--)
    {
        segment* it = *iterator;
        if (it->start = target)
        {
            return it;
        }
    }
    return nullptr;
}

void LinkedList::delete_segment(void* target)
{
    
    
}

uint8_t LinkedList::get_length()
{
    return this->free_list.size();
}

uint8_t LinkedList::get_size(void* target)
{

    auto iterator = this->free_list.end();
    for (; iterator != this->free_list.begin(); iterator--)
    {
        segment* it = *iterator;
        if (it->start = target)
        {
            return it->sizeinbytes;
        }
    }
    return 0;
}
*/

/*
//Create a new Node at the front of the list to store the passed in parameter.
void LinkedList::add_segment(void* address, uint8_t const& sizeinbytes)
{
    //allocate new head and tail
    segment* new_segment = new segment;
    //setting next Node to data
    new_segment->start = address;
    //setting word size equal to size passed in
    new_segment->sizeinbytes = sizeinbytes;

    new_segment->end = (uint8_t*)address + sizeinbytes;
    //Node checking
    if (length == 0)
    {
        head = new_segment;
        tail = head;
        this->length = length + 1;
    }
    //perform auto sort
    else if (length == 1)
    {
        //if the block is greater than the tail, set the new segment as our tail
        if ((uint8_t*)(address) > (uint8_t*)(tail->end))
        {
            tail->next = new_segment;
            tail->prev = new_segment;
            new_segment->prev = tail;
            new_segment->next = tail;
            head = tail;
            tail = new_segment;
            this->length = length + 1;
        }
        //if the tail is greater than our new segment, set the new segment as our head.
        else if ((uint8_t*)address + sizeinbytes < (uint8_t*)tail->start)
        {
            tail->next = new_segment;
            tail->prev = new_segment;
            new_segment->prev = tail;
            new_segment->next = tail;
            head = new_segment;
            this->length = length + 1;
        }
        //should go in front of the tail and merge
        else if ((uint8_t*)(address) == (uint8_t*)(tail->end))
        {
            //address space doesnt match block size for some reason
            //merge
            //1.delete tail
            new_segment->start = tail->start;
            new_segment->sizeinbytes = sizeinbytes + tail->sizeinbytes;
            new_segment->end = (uint8_t*)address + sizeinbytes;
            delete tail;
            //2.set current segment as new tail
            tail = new_segment;
            head = new_segment;
        }
        //should go behind the tail and merge
        else if ((uint8_t*)address + sizeinbytes  == (uint8_t*)tail->start)
        {
            //merge
            //1.delete tail
            new_segment->start = address;
            new_segment->sizeinbytes = sizeinbytes + tail->sizeinbytes;
            new_segment->end = tail->end;
            delete tail;
            //2.set current segment as new tail
            tail = new_segment;
            head = new_segment;
        }
    }
    else
    {

        //if our original tail has been freed
        if ((uint8_t*)(address) > (uint8_t*)(tail->end))
        {
            head->prev = new_segment;
            tail->next = new_segment;
            new_segment->prev = tail;
            new_segment->next = head;
            tail = new_segment;
            this->length = length + 1;
        }
        //our original head has been freed
        else if (((uint8_t*)address + sizeinbytes < (uint8_t*)head->start))
        {
            head->prev = new_segment;
            tail->next = new_segment;
            new_segment->prev = tail;
            new_segment->next = head;
            head = new_segment;
            this->length = length + 1;
        }
        else
        {
            segment* current = tail;
            for (int i = 0; i < length; i++)
            {
                //if the end of our block is less than the current blocks start, and the start of the block is greater than the end of the previous block
                //we can fit it inbetween
                if (((uint8_t*)address + sizeinbytes < (uint8_t*)current->start) && ((uint8_t*)(address) > (uint8_t*)(current->prev->end)))
                {
                    new_segment->prev = current->prev;
                    new_segment->next = current;
                    current->prev->next = new_segment;
                    current->prev = new_segment;
                    break;
                    this->length = length + 1;
                }
                //if this block used to belong to current block
                else if ((uint8_t*)address + sizeinbytes == (uint8_t*)current->start)
                {
                    //if this used to be one block
                    if (((uint8_t*)(address) == (uint8_t*)(current->prev->end)))
                    {
                        //merge

                        //merge three segments into one
                        
                        //1.delete previous segment, save its information first
                        void* firstnodestart = current->prev->start;
                        segment* firstnodeprev = current->prev->prev;
                        uint8_t firstnodesize = current->prev->sizeinbytes;
                        delete current->prev;
                        //2.delete current segment, save its information first
                        void* currentnodeend = current->end;
                        segment* currentnodenext = current->next;
                        uint8_t currentnodesize = current->sizeinbytes;
                        delete current;
                        //3.create new segment
                        segment* new_segment_block = new segment;
                        new_segment_block->prev = firstnodeprev;
                        new_segment_block->next = currentnodenext;
                        new_segment_block->sizeinbytes = firstnodesize + currentnodesize + sizeinbytes;
                        new_segment_block->start = firstnodestart;
                        new_segment_block->end = currentnodeend;
                        delete new_segment;
                        this->length = length - 1;
                    }
                    break;
                }
                //if this block used to belong to previous block
                else if (((uint8_t*)(address) == (uint8_t*)(current->prev->end)))
                {
                    //merge
                    //merge two blocks into one

                    //1.delete previous segment, save its information first
                    void* firstnodestart = current->prev->start;
                    segment* firstnodeprev = current->prev->prev;
                    uint8_t firstnodesize = current->prev->sizeinbytes;

                    delete current->prev;
                    //2.actual merge
                    new_segment->start = firstnodestart;
                    new_segment->prev = firstnodeprev;
                    new_segment->next = current;
                    new_segment->sizeinbytes = firstnodesize + sizeinbytes;
                    current->prev = new_segment;
                    break;
                    this->length = length;
                }
                else
                {
                    current = tail->prev;
                }
            }
        }
        
        //wrapping back around
        //new_segment->next = tail;
        //setting new segment to head
        //new_segment->prev = head;
       //head->next = new_segment;
        //set head to new Node
        //head = new_segment;
        //tail->prev = head;
        
    }
    

    std::cout << "start: " << new_segment->start << std::endl;
    std::cout << "end: " << new_segment->end << std::endl;
    //this->length = length + 1;
}

LinkedList::segment* LinkedList::merge(segment* )
{

}
LinkedList::segment* LinkedList::find_segment(void* pntr)
{
    segment* current = head;
    for (int i = 0; i < length; i++)
    {
        if (current->start == pntr )
        {
            return current;
        }
        current = current->next;
    }
    return nullptr;

}

void LinkedList::delete_segment(void* address)
{
    if (length == 0)
    {
        //do nothing
    }
    else if (length == 1)
    {
        segment* segmentone = find_segment(address);
        delete segmentone;
    }
    else
    {
        segment* segmentone = find_segment(address);
        segmentone->next->prev = segmentone->prev;
        segmentone->prev->next = segmentone->next;
        this->length = this->length - 1;

        delete segmentone;
    }
}

void* LinkedList::alloc_segment(segment*, uint8_t)
{
    return nullptr;
}


uint8_t LinkedList::get_length()
{
    return this->length;
}








uint8_t LinkedList::get_size(void* startpntr)
{
    segment* target = find_segment(startpntr);

    return target->sizeinbytes;
}

void* LinkedList::get_list(void* basepntr)
{            if (this->length > 0)    {        char* list = (char*)malloc((this->length * 2));        for (int i = 0; i < (this->length) * 2; i + 2)        {            list[i] = (uint8_t*)(head->start) - (uint8_t*)basepntr;            list[i + 1] = head->sizeinbytes;        }        return (void*)list;    }
    
    
    return nullptr;
}


LinkedList::~LinkedList()
{
    segment* destroy_next;

    while (head)
    {
        destroy_next = head->next;
        delete head;
        head = destroy_next;
    }
}
*/

