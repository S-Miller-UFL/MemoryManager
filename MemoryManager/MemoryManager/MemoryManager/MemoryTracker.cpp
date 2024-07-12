




//MemoryTracker.cpp
/*
* Author: Steven Miller
* Date created: October 29, 2023
* Purpose: data structure to keep track of memory blocks and holes, replaces linked list
* Used by: MemoryManager.h
* Uses: N/A
*/
#include "MemoryTracker.h"
#include <bitset>
#include <string>

//add segment to memory map
void MemoryTracker::add_segment(void* start,size_t const& sizeinbytes) //uint8_t can hold up to 255
{
	//when this function is called, it needs to:
	//1.check to see where this goes, the map does this for us by automatically placing the segment in the list based on its "start" pointer value
	//create new segment and add to map
	int numberofwords = sizeinbytes / this->wordsize;
	if (sizeinbytes % this->wordsize != 0)
	{
		numberofwords = numberofwords + 1;
	}
	segment* new_segment = new segment;
	new_segment->start = start;
	new_segment->end = (uint8_t*)start + sizeinbytes;
	new_segment->sizeinbytes = (numberofwords*this->wordsize);
	new_segment->sizeinwords = numberofwords;
	this->memory_map.emplace(start,new_segment);
	this->initialized = true;
}

//merge adjacent blocks by their start and end pointers
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
							size_t sizeinbytes = iterator->second->sizeinbytes + std::next(iterator)->second->sizeinbytes;
							size_t sizeinwords = iterator->second->sizeinwords + std::next(iterator)->second->sizeinwords;
							void* newstart = iterator->first;
							//erase segment
							delete_segment(std::next(iterator)->first);
							delete_segment(iterator->first);
							//edit current one
							add_segment(newstart, sizeinbytes);
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
							size_t sizeinbytes = iterator->second->sizeinbytes + std::prev(iterator)->second->sizeinbytes;
							size_t sizeinwords = iterator->second->sizeinwords + std::prev(iterator)->second->sizeinwords;
							void* newstart = std::prev(iterator)->first;
							//erase segment
							delete_segment(std::prev(iterator)->first);
							delete_segment(iterator->first);
							//edit current one
							add_segment(newstart,sizeinbytes);
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
							size_t sizeinbytes = std::prev(iterator)->second->sizeinbytes + iterator->second->sizeinbytes + std::next(iterator)->second->sizeinbytes;
							size_t sizeinwords = std::prev(iterator)->second->sizeinwords + iterator->second->sizeinwords + std::next(iterator)->second->sizeinwords;
							void* newstart = std::prev(iterator)->first;
							//erase segment
							delete_segment(std::prev(iterator)->first);
							delete_segment(std::next(iterator)->first);
							delete_segment(iterator->first);
							//edit current one
							add_segment(newstart, sizeinbytes);
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
//find segment in map
MemoryTracker::segment* MemoryTracker::find_segment(void* target)
{
	auto iterator = this->memory_map.find(target);
	if (iterator == this->memory_map.end())
	{
		return nullptr;
	}
	else
	{
		return iterator->second;
	}
	
}

//remove segment from map, dynamically frees.
void MemoryTracker::delete_segment(void* target)
{
	auto iterator = this->memory_map.find(target);
	if (iterator != this->memory_map.end())
	{
		delete iterator->second;
		iterator->second = nullptr;
		this->memory_map.erase(iterator->first);
	}
}

//returns start pointer of segment after allocation
void* MemoryTracker::alloc_segment(void* target, size_t const& sizetoalloc)
{
	auto iterator = this->memory_map.find(target);
	if(iterator == this->memory_map.end())
	{
		return nullptr;
	}
	else if ( (iterator->second->sizeinbytes >= sizetoalloc))
	{
		size_t segmentsizebytes = iterator->second->sizeinbytes;
		size_t numberofwords = sizetoalloc / this->wordsize;
		size_t segmentsizewords = iterator->second->sizeinwords;
		if (sizetoalloc % this->wordsize != 0)
		{
			numberofwords = numberofwords + 1;
		}
		void* newsegmentstart = (uint8_t*)target + (numberofwords*this->wordsize);
		delete_segment(target);
		if ((segmentsizewords - numberofwords)> 0)
		{
			add_segment(newsegmentstart, segmentsizebytes -(numberofwords*this->wordsize));
			merge();
		}
		return target;
	}
	else
	{
		return nullptr;
	}
}

//get size of map
size_t MemoryTracker::get_length()
{
	return this->memory_map.size();
}

//get list of all segments in map
void* MemoryTracker::get_list(void* basepntr)
{
	if (this->memory_map.size() > 0)
    {
		const int size = 1+((this->memory_map.size())*2);
		auto iterator = this->memory_map.begin();
		intptr_t segoffset = (intptr_t)(iterator->first);
		intptr_t basepointer = (intptr_t)(basepntr);
		uint16_t* list = new uint16_t[size];
		int segsize = 0;
        for (int i =0; i < size; i++)
        {
			if (i == 0)
			{
				list[i] = this->memory_map.size();
			}
			//odd case
			else if (i % 2 == 1)
			{
				//get offset
				segoffset = (intptr_t)(iterator->first) - (basepointer);
				segoffset = segoffset / this->wordsize;
				list[i] = segoffset;
			}
			//even case
			else if (i % 2 == 0)
			{
				//get size in words
				segsize = iterator->second->sizeinwords;
				list[i] = segsize;
				//edge case: size == 1
				//adding extra iterator check because im paranoid
				if (this->memory_map.size() == 1 || iterator == std::prev(this->memory_map.end()))
				{
					break;
				}
				else
				{
					if (iterator != std::prev(this->memory_map.end()))
					{
						iterator++;
					}
				}
			}
			
        }
        return (void*)list;
    }
 	else if (this->memory_map.size() == 0 && this->initialized == true)
	{
		uint16_t* list = new uint16_t[1];
		list[0] = 0;
		return (void*)list;
	}
	else if (this->initialized == false)
	{
		return nullptr;
	}
}
//get bitmap representing segments
void* MemoryTracker::get_bitmap(void* basepntr,int numwords)
{

	if (this->initialized == false)
	{
		return nullptr;
	}
	else
	{
		uint16_t remainder = numwords % 8;
		uint16_t bitmapsize = numwords;
		//rounding to nearest 8 bit word
		if (remainder != 0)
		{
			bitmapsize = bitmapsize - remainder;
			bitmapsize = bitmapsize + 8;
		}
		//2.call get_list() to get list of holes
		uint16_t* list = (uint16_t*)this->get_list(basepntr);
		std::string bitmap_temp;
		//3.create temp array based on number of words
		//4.initialize temp array to all 1's
		for (int i = 0; i < numwords; i++)
		{
			bitmap_temp = bitmap_temp + '1';
		}
		//5.for (i = 1; i < size of list)
		//	 for (j < list[i+1])
		//		bitmap[list[i]+j] = 1;
		//	i+=2
		for (int i = 0; i < (list[0] * 2) + 1; i++)
		{
			if (i % 2 != 0)
			{
				for (int j = 0; j < list[i + 1]; j++)
				{
					bitmap_temp[list[i] + j] = '0';
				}
			}
		}
		delete[](uint16_t*)list;
		std::string bitmap;
		//6. initialize array to zero.
		for (int i = 0; i < bitmapsize; i++)
		{
			bitmap = bitmap + '0';
		}
		//7. copy temp array to actual bit array
		for (int i = 0; i < bitmap_temp.size(); i++)
		{
			bitmap[i] = bitmap_temp[i];
		}
		//8.mirror each bit individually
		//print mirrored bitmap for testing

		//make temparray of size 8
		//uint16_t temparr[8];
		std::vector<std::string> bytes;
		for (int i = 0; i < bitmap.size() / 8; i++)
		{
			std::string tempstring;
			for (int j = 0; j < 8; j++)
			{
				tempstring = tempstring + bitmap[i * 8 + j];
			}
			bytes.push_back(tempstring);
		}
		uint16_t sizeofbytearray = bytes.size();
		//mirror individual bytes
		//christ this is getting inefficient
		//idea taken from:
		//www.prepbytes.com/blog/cpp-programming/reverse-elements-of-an-array-in-cpp/
		for (int i = 0; i < bytes.size(); i++)
		{
			std::reverse(bytes.at(i).begin(), bytes.at(i).end());
		}

		//put back into bitmap_actual
		for (int i = 0; i < bytes.size(); i++)
		{
			for (int j = 0; j < 8; j++)
			{
				bitmap[(i * 8) + j] = bytes.at(i)[j];
			}

		}
		//9.convert size of byte array into two byte binary
		std::bitset<16> binarysize(bytes.size());
		std::string sizestring = binarysize.to_string();

		//10.swap upper and lower byte of two size bytes
		std::string upper;
		std::string lower;
		for (int i = 0; i < 8; i++)
		{
			upper = upper + sizestring.at(i);
		}
		for (int i = 0; i < 8; i++)
		{
			lower = lower + sizestring.at(8 + i);
		}
		//11.append to byte array
		//the actual actual bitmap were returning
		//i swear this is the final array ill make
		std::string bitmap_final = lower + upper + bitmap;
		bytes.clear();
		for (int i = 0; i < bitmap_final.size() / 8; i++)
		{
			std::string temp;
			temp = bitmap_final.substr(8 * i, 8);
			bytes.push_back(temp);
		}
		//12.return actual bit array
		//convert to byte array
		uint8_t* finalarr = new uint8_t[bytes.size()];
		for (int i = 0; i < bytes.size(); i++)
		{
			std::bitset<8> foo(bytes.at(i));
			uint8_t temp = (uint8_t)foo.to_ulong();
			finalarr[i] = temp;
		}
		return finalarr;
	}
		
}

//get size of segment
size_t MemoryTracker::get_bytesize(void* target)
{
	auto iterator = this->memory_map.find(target);
	if (iterator == this->memory_map.end())
	{
		return -1;
	}
	else
	{
		return iterator->second->sizeinbytes;
	}
}

size_t MemoryTracker::get_wordsize(void* target)
{
	auto iterator = this->memory_map.find(target);
	if (iterator == this->memory_map.end())
	{
		return -1;
	}
	else
	{
		return iterator->second->sizeinwords;
	}
}
//nuke the memory map
void MemoryTracker::wipe_map()
{
	if(this->memory_map.size() > 0)
	{
		auto iterator = this->memory_map.begin();
		for(; iterator != this->memory_map.end(); iterator++)
		{
			delete iterator ->second;
			iterator ->second = nullptr;
		}
	this->memory_map.clear();
	}
	this->initialized = false;
}

MemoryTracker::~MemoryTracker()
{
	
	if(this->memory_map.size() > 0)
	{
		auto iterator = this->memory_map.begin();
		for(; iterator != this->memory_map.end(); iterator++)
		{
			delete iterator ->second;
			iterator ->second = nullptr;
		}
	this->memory_map.clear();
	}
	this->initialized = false;

}
