libMemoryManager.a: MemoryManager.cpp MemoryManager.h MemoryTracker.cpp MemoryTracker.h 
	g++ -o MemoryTracker.o -c MemoryTracker.cpp
	g++ -o MemoryManager.o -c MemoryManager.cpp
	ar -cr libMemoryManager.a MemoryManager.o MemoryTracker.o

