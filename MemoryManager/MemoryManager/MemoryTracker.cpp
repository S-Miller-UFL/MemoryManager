




//MemoryTracker.cpp
/*
* Author: Steven Miller
* Date created: October 29, 2023
* Purpose: data structure to keep track of memory blocks and holes, replaces linked list
* Used by: MemoryManager.h
* Uses: N/A
*/
#include "MemoryTracker.h"

//add segment to memory map
void MemoryTracker::add_segment(void* start,size_t const& sizeinbytes) //uint8_t can hold up to 255
{
	//when this function is called, it needs to:
	//1.check to see where this goes, the map does this for us by automatically placing the segment in the list based on its "start" pointer value
	//2.once the block is added, we need to call merge()

	//create new segment and add to map
	//segment* new_segment = new segment;
	//new_segment->start = start;
	//new_segment->end = (intptr_t*)start + sizeinbytes;
	//new_segment->sizeinbytes = sizeinbytes;
	//this->memory_map.emplace(start,new_segment);

	//create new segment and add to map
	segment* new_segment = new segment;
	new_segment->start = start;
	new_segment->end = (uint8_t*)start + sizeinbytes;
	new_segment->sizeinbytes = sizeinbytes;
	this->memory_map.emplace(start,new_segment);

	merge();
}

void MemoryTracker::merge()
{

		bool loop = true;
		while (loop)
		{
			bool newiteratorneeded = false;
			auto iterator = this->memory_map.begin();
			if (this->memory_map.size() > 1)
			{
				//iterate through list and check for merge opportunities
				for (; iterator != this->memory_map.end(); iterator++)
				{
					//edge case: were at the beginning
					if ((iterator == this->memory_map.begin()))
					{
						//check if we can merge front
						if (iterator->second->end == std::next(iterator)->first) //if true, we can merge two blocks into one.
						{
							//merge
							size_t size = iterator->second->sizeinbytes + std::next(iterator)->second->sizeinbytes;
							void* newend = std::next(iterator)->second->end;
							void* newstart = iterator->first;
							//erase segment
							delete_segment(std::next(iterator)->first);
							delete_segment(iterator->first);
							//edit current one
							segment* new_segment = new segment;
							new_segment->start = newstart;
							new_segment->end = newend;
							new_segment->sizeinbytes = size;
							this->memory_map.emplace(newstart, new_segment);
							//we now need a new iterator
							newiteratorneeded = true;
							break;

						}
					}
					//edge case: were at the end
					else if (iterator == std::prev(this->memory_map.end()))
					{
						//check if we can merge back
						if (iterator->first == std::prev(iterator)->second->end) //if true, we can merge two blocks into one.
						{
							//merge
							size_t size = iterator->second->sizeinbytes + std::prev(iterator)->second->sizeinbytes;
							void* newend = iterator->second->end;
							void* newstart = std::prev(iterator)->first;
							//erase segment
							delete_segment(std::prev(iterator)->first);
							delete_segment(iterator->first);
							//edit current one
							segment* new_segment = new segment;
							new_segment->start = newstart;
							new_segment->end = newend;
							new_segment->sizeinbytes = size;
							this->memory_map.emplace(newstart, new_segment);
							//we now need a new iterator
							newiteratorneeded = true;
							break;
						}
					}
					//in every other case
					else
					{
						//check if we can squeeze
						if ((std::prev(iterator)->second->end == iterator->first) && (std::next(iterator)->first == iterator->second->end)) //if true, we can merge two blocks into one.
						{
							//merge
							size_t size = std::prev(iterator)->second->sizeinbytes + iterator->second->sizeinbytes + std::next(iterator)->second->sizeinbytes;
							void* newend = std::next(iterator)->second->end;
							void* newstart = std::prev(iterator)->first;
							//erase segment
							delete_segment(std::prev(iterator)->first);
							delete_segment(std::next(iterator)->first);
							delete_segment(iterator->first);
							//edit current one
							segment* new_segment = new segment;
							new_segment->start = newstart;
							new_segment->end = newend;
							new_segment->sizeinbytes = size;
							this->memory_map.emplace(newstart, new_segment);
							//we now need a new iterator
							newiteratorneeded = true;
							break;
						}
					}
				}
			}
			if (!newiteratorneeded)
			{
				loop = false;
			}
		}
	
}
MemoryTracker::segment* MemoryTracker::find_segment(void* target)
{
	auto iterator = this->memory_map.find(target);
	return iterator->second;
}

void MemoryTracker::delete_segment(void* target)
{
	auto iterator = this->memory_map.find(target);
	delete iterator->second;
	this->memory_map.erase(iterator);
}

void* MemoryTracker::alloc_segment(void* target, size_t const& sizetoalloc)
{
	auto iterator = this->memory_map.find(target);
	if ( (iterator->second->sizeinbytes >= sizetoalloc)  || (iterator == this->memory_map.end()) )
	{
		size_t segmentsize = iterator->second->sizeinbytes;
		void* newsegmentstart = (uint8_t*)target + sizetoalloc;
		delete_segment(target);
		add_segment(newsegmentstart, segmentsize-sizetoalloc);
		return newsegmentstart;
	}
	else
	{
		return nullptr;
	}
}

size_t MemoryTracker::get_length()
{
	return this->memory_map.size();
}

void* MemoryTracker::get_list(void* basepntr)
{
	if (this->memory_map.size() > 0)
    {
		auto iterator = this->memory_map.begin();
        int* list = (int*)malloc((this->memory_map.size() * 2));

		intptr_t x = (intptr_t)(iterator->second->start);
		intptr_t y = (intptr_t)(basepntr);
		int z = 0;

        for (int i = 0; i < (this->memory_map.size()) * 2; i += 2)
        {
			x = (intptr_t)(iterator->second->start);
			y = (intptr_t)(basepntr);
			z = (int)(iterator->second->sizeinbytes);
            list[i] = x-y;
			list[i + 1] = z;
			if (iterator == std::prev(this->memory_map.end()))
			{
				break;
			}
			else
			{
				iterator++;
			}
        }
        return (void*)list;
    }
    
    
    return nullptr;
}

size_t MemoryTracker::get_size(void* target)
{
	auto iterator = this->memory_map.find(target);
	return iterator->second->sizeinbytes;

}

MemoryTracker::~MemoryTracker()
{
	auto iterator = this->memory_map.begin();
	for (; iterator != this->memory_map.end(); iterator++)
	{
		delete iterator->second;
	}

	this->memory_map.clear();
}
