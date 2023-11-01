//#include "MemoryManager.h"

//#include "MemoryManager/LinkedList.h"
#include "MemoryTracker.h"
//#include "leaker.h"
#include <string>
#include <cmath>
#include <array>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdint>

int main()
{
	//MemoryManager mem(8,bestFit);
	//mem.initialize(1000);
	//mem.allocate(1000);
	typedef MemoryTracker MM_LL;
	MM_LL ll;
	size_t blocksize = 10000;
	int offset = 0;
	int allocsize = 100;
	void* basepntr = malloc(blocksize);
	//ll.add_segment(basepntr, blocksize);
	/*
	//allocate algorithm
	//1.find offset
	offset = 0;
	//2.get alloc size from user
	allocsize = 20;
	//3.find segment with start pointer = basepntr + offset
	//get its size
	segmentsize = ll.get_size((void*)((intptr_t)basepntr + offset));
	//4.delete that segment
	ll.delete_segment((void*)((intptr_t)basepntr + offset));
	//5.add new segment with adjusted size and start pointer
	ll.add_segment((void*)((intptr_t)basepntr + allocsize), segmentsize - allocsize);
	*/
	//add head, add tail, merge inbetween, merge head, merge tail
	ll.add_segment((uint8_t*)basepntr + (allocsize * 1), 100); //add head
	ll.add_segment((uint8_t*)basepntr + (allocsize * 3), 100); //add tail
	ll.add_segment((uint8_t*)basepntr + (allocsize * 5), 100); //add tail
	ll.add_segment((uint8_t*)basepntr + (allocsize * 6), 100); //merge tail
	ll.add_segment((uint8_t*)basepntr + (allocsize * 8), 100); //add tail
	//ll.add_segment((uint8_t*)basepntr + (allocsize * 2), 100); //merge between
	//ll.add_segment((uint8_t*)basepntr + (allocsize * 4), 100); //merge between
	//ll.add_segment((uint8_t*)basepntr + (allocsize * 7), 100); //merge between
	ll.add_segment((uint8_t*)basepntr + (allocsize * 0), 100); //merge between
	//allocate segments
	ll.alloc_segment(basepntr, 30);
	//ll.alloc_segment((uint8_t*)basepntr + 200, 100);

	//simulate freeing memory
	ll.add_segment(basepntr, 15);

	//simulate freeing more memory, this time we merge at head
	ll.add_segment((uint8_t*)basepntr+15, 15);

	//delete segments
	//ll.delete_segment(basepntr);
	//ll.delete_segment((uint8_t*)basepntr + 32);
	//ll.delete_segment((uint8_t*)basepntr + 66);
	//ll.delete_segment((uint8_t*)basepntr + 99);
	//ll.delete_segment((uint8_t*)basepntr + 132);
	//ll.delete_segment((uint8_t*)basepntr + 165);
	//ll.delete_segment((uint8_t*)basepntr + 198);

	char* list = (char*)ll.get_list(basepntr);

	for (int i = 0; i < sizeof(list)/sizeof(list[0]); i++)
	{
		std::cout << list[i];
	}


	free(basepntr);
	

	return 0;
}
