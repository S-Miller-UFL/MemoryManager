//#include "MemoryManager.h"

//#include "MemoryManager/LinkedList.h"

#include "MemoryManager/MemoryTracker.h"
#include "MemoryManager/MemoryManager.h"
#include <string>
#include <cmath>
#include <array>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdint>
//#include "testfile.cpp"

void memorytrackertest()
{
	//testing "MemoryTracker.h"
	//create memorymanager object
	MemoryTracker mm;
	size_t blocksize = 10000;
	int offset = 0;
	int allocsize = 100;
	unsigned wordsize = 8;
	void* basepntr = (void*)new uint8_t[blocksize];
	mm.set_word_size(wordsize);
	//test initializing block of memory
	mm.add_segment(basepntr,blocksize);

	//mm.print_map();

	//test allocating segment of memory
	void* alloc = mm.alloc_segment(basepntr,allocsize);
	//mm.print_map();
	void* alloctwo = mm.alloc_segment(basepntr,101);
	//mm.print_map();
	//void* allocthree = mm.allocate(33);
	//mm.print_map();

	//uint16_t* list = (uint16_t*)mm.getBitmap();
	//delete[] list;
	//free memory
	mm.add_segment(alloc,allocsize);
	mm.add_segment(alloctwo,101);
	//shut it down!
	//mm.wipe_map();
	/*
	//MemoryManager mem(8,bestFit);
	//mem.initialize(1000);
	//mem.allocate(1000);
	typedef MemoryTracker MM_LL;
	MM_LL ll;
	size_t blocksize = 10000;
	int offset = 0;
	int allocsize = 100;
	unsigned wordsize = 8;
	void* basepntr = (void*)new uint8_t[blocksize];
	ll.set_word_size(wordsize);
	//add head, add tail, merge inbetween, merge head, merge tail
	ll.add_segment((uint8_t*)basepntr + (allocsize * 1), 100); //add head
	ll.add_segment((uint8_t*)basepntr + (allocsize * 3), 100); //add tail
	ll.add_segment((uint8_t*)basepntr + (allocsize * 5), 100); //add tail
	ll.add_segment((uint8_t*)basepntr + (allocsize * 6), 100); //merge tail
	ll.add_segment((uint8_t*)basepntr + (allocsize * 8), 100); //add tail
	ll.add_segment((uint8_t*)basepntr + (allocsize * 0), 100); //merge between

	//allocate segments
	ll.alloc_segment(basepntr, 30);

	//simulate freeing memory
	ll.add_segment(basepntr, 15);

	//simulate freeing more memory, this time we merge at head
	ll.add_segment((uint8_t*)basepntr + 15, 15);

	//delete segments
	//ll.delete_segment(basepntr);
	//ll.delete_segment((uint8_t*)basepntr + 32);
	//ll.delete_segment((uint8_t*)basepntr + 66);
	//ll.delete_segment((uint8_t*)basepntr + 99);
	//ll.delete_segment((uint8_t*)basepntr + 132);
	//ll.delete_segment((uint8_t*)basepntr + 165);
	//ll.delete_segment((uint8_t*)basepntr + 198);

	delete[] (uint8_t*)basepntr;
	*/
	delete[] (uint8_t*)basepntr;
}

void memorymanagertest()
{
	//testing "MemoryManager.h"

	//create memorymanager object
	MemoryManager mm(8, bestFit);

	//test initializing block of memory
	mm.initialize(10);

	//mm.print_map();

	//test allocating segment of memory
	void* alloc = mm.allocate(7);
	//mm.print_map();
	void* alloctwo = mm.allocate(1);
	//mm.print_map();
	//void* allocthree = mm.allocate(33);
	//mm.print_map();
	uint8_t* bitmap = (uint8_t*)mm.getBitmap();
	uint16_t size = sizeof(bitmap)/sizeof(bitmap[0]);

	std::cout << size << std::endl;

	for (int i = 0; i < size; i++)
	{
		std::cout << bitmap[i] << " ";
	}
	//uint16_t* list = (uint16_t*)mm.getBitmap();
	//delete[] list;
	//free memory
	mm.free(alloc);
	mm.free(alloctwo);
	//shut it down!
	mm.shutdown();
}

/*
int main()
{
	//memorytrackertest();
	//memorymanagertest();
	return 0;
}
*/