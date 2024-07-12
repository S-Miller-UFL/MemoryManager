/*
COP 4600 Project 2 Memory Management Catch2 Tests
Brian Gunnar Magnuson
10/29/2023

Steven Miller
11/9/2023
Remember to run this test with valgrind:
valgrind --track-origins=yes --leak-check=full --show-leak-kinds=all ./PROGRAMNAME
This file has been checked to be leak free, if you encounter any leaks, its most likely your implementation.
*/

#include "../MemoryManager/MemoryManager.h"
#include "./catch/catch_amalgamated.hpp"
#include <iostream>
#include <fstream>
typedef int file_descriptor;

void setSubstr(std::string& str, int start, int length, const char c) {
    for (int i = 0; i < length; i++) {
        str.at(i + start) = c;
    }
}

std::string getMemDiagram(MemoryManager& memoryManager) 
{
    int numberOfWords = memoryManager.getMemoryLimit() / memoryManager.getWordSize();
    std::string diagram = std::string(numberOfWords, '#');
    uint16_t* holeList = static_cast<uint16_t*>(memoryManager.getList());
    uint16_t* listEntryPoint = holeList;
    if (holeList != nullptr) {
        uint16_t holeListlength = *holeList++;
        for (uint16_t i = 1; i < holeListlength * 2; i += 2) {
            setSubstr(diagram, holeList[i - 1], holeList[i], '.');
        }
        delete[] listEntryPoint;
    }
    diagram = "[" + diagram + "]";
    return diagram;
}

void* verboseAllocate(MemoryManager& memoryManager, size_t sizeInBytes) {
    const char* green = "\033[1;32m";
    const char* yellow = "\033[1;33m";
    const char* reset = "\033[0m";
    std::string beforeDiagram = getMemDiagram(memoryManager);
    std::cout << "Allocating " << sizeInBytes << " bytes; word size is "
              << memoryManager.getWordSize() << std::endl;
    void* memory = memoryManager.allocate(sizeInBytes);
    std::string afterDiagram = getMemDiagram(memoryManager);
    int startDiff = 1;
    int endDiff = afterDiagram.length() - 1;
    bool diffFound = false;
    for (int i = 0; i < beforeDiagram.length(); i++) {
        if (!diffFound && beforeDiagram.at(i) != afterDiagram.at(i)) {
            startDiff = i;
            diffFound = true;
        }
        if (diffFound && beforeDiagram.at(i) == afterDiagram.at(i)) {
            endDiff = i;
            break;
        }
    }
    afterDiagram.insert(endDiff, reset);
    if (diffFound) {
        afterDiagram.insert(startDiff, green);
    } else {
        afterDiagram.insert(startDiff, yellow);
    }
    std::cout << afterDiagram << std::endl;
    return memory;
}

void verboseFree(MemoryManager& memoryManager, void* address) {
    const char* red = "\033[1;31m";
    const char* yellow = "\033[1;33m";
    const char* reset = "\033[0m";
    std::string beforeDiagram = getMemDiagram(memoryManager);
    std::cout << "Freeing memory" << std::endl;
    memoryManager.free(address);
    std::string afterDiagram = getMemDiagram(memoryManager);
    int startDiff = 1;
    int endDiff = afterDiagram.length() - 1;
    bool diffFound = false;
    for (int i = 0; i < beforeDiagram.length(); i++) {
        if (!diffFound && beforeDiagram.at(i) != afterDiagram.at(i)) {
            startDiff = i;
            diffFound = true;
        }
        if (diffFound && beforeDiagram.at(i) == afterDiagram.at(i)) {
            endDiff = i;
            break;
        }
    }
    afterDiagram.insert(endDiff, reset);
    if (diffFound) {
        afterDiagram.insert(startDiff, red);
    } else {
        afterDiagram.insert(startDiff, yellow);
    }
    std::cout << afterDiagram << std::endl;
}

TEST_CASE("Sanity Check", "[basic]") {
    REQUIRE(1 == 1);
    std::cout << std::endl;
}

TEST_CASE("init 10", "[alloc]") {
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    REQUIRE(getMemDiagram(mm) == "[..........]");
}

TEST_CASE("alloc 2", "[alloc]") {
    std::cout << "Verbose Test: alloc 2" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    std::cout << std::endl;
}

TEST_CASE("alloc 2,2,2", "[alloc]") {
    std::cout << "Verbose Test: alloc 2,2,2" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[####......]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######....]");
    std::cout << std::endl;
}

TEST_CASE("alloc too much", "[alloc]") {
    std::cout << "Verbose Test: alloc too much" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    verboseAllocate(mm, 5);
    REQUIRE(getMemDiagram(mm) == "[#####.....]");
    verboseAllocate(mm, 5);
    REQUIRE(getMemDiagram(mm) == "[##########]");
    void* res = verboseAllocate(mm, 1);
    REQUIRE(res == nullptr);
    REQUIRE(getMemDiagram(mm) == "[##########]");
    std::cout << std::endl;
}

TEST_CASE("alloc too much 2", "[alloc]") {
    std::cout << "Verbose Test: alloc too much 2" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    verboseAllocate(mm, 4);
    REQUIRE(getMemDiagram(mm) == "[####......]");
    verboseAllocate(mm, 4);
    REQUIRE(getMemDiagram(mm) == "[########..]");
    void* res = verboseAllocate(mm, 4);
    REQUIRE(res == nullptr);
    REQUIRE(getMemDiagram(mm) == "[########..]");
    std::cout << std::endl;
}

TEST_CASE("free correct order", "[free]") {
    std::cout << "Verbose Test: free correct order" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    void* arr1 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    void* arr2 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[####......]");
    void* arr3 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######....]");

    verboseFree(mm, arr2);
    REQUIRE(getMemDiagram(mm) == "[##..##....]");
    verboseFree(mm, arr1);
    REQUIRE(getMemDiagram(mm) == "[....##....]");
    verboseFree(mm, arr3);
    REQUIRE(getMemDiagram(mm) == "[..........]");
    std::cout << std::endl;
}

TEST_CASE("free at ends", "[free]") {
    std::cout << "Verbose Test: free at ends" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    void* arr1 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    void* arr2 = verboseAllocate(mm, 6);
    REQUIRE(getMemDiagram(mm) == "[########..]");
    void* arr3 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##########]");

    verboseFree(mm, arr1);
    REQUIRE(getMemDiagram(mm) == "[..########]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##########]");
    verboseFree(mm, arr3);
    REQUIRE(getMemDiagram(mm) == "[########..]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##########]");
    std::cout << std::endl;
}

TEST_CASE("holes at ends", "[free]") {
    std::cout << "Verbose Test: holes at ends" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    void* arr1 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    void* arr2 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[####......]");
    void* arr3 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######....]");
    void* arr4 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[########..]");
    void* arr5 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##########]");

    uint16_t* res;

    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res != nullptr);
    REQUIRE(res[0] == 0);
    delete[] res;

    verboseFree(mm, arr1);
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res[0] == 1);
    delete[] res;

    verboseFree(mm, arr5);
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res[0] == 2);
    delete[] res;

    verboseFree(mm, arr2);
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res[0] == 2);
    delete[] res;

    verboseFree(mm, arr4);
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res[0] == 2);
    delete[] res;

    std::cout << std::endl;
}

TEST_CASE("hole in middle", "[free]") {
    std::cout << "Verbose Test: hole in middle" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    void* arr1 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    void* arr2 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[####......]");
    void* arr3 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######....]");
    void* arr4 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[########..]");
    void* arr5 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##########]");

    uint16_t* res;

    verboseFree(mm, arr2);
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res[0] == 1);
    delete[] res;

    verboseFree(mm, arr4);
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res[0] == 2);
    delete[] res;

    verboseFree(mm, arr3);
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res[0] == 1);
    delete[] res;

    std::cout << std::endl;
}

TEST_CASE("first best fit", "[method]") {
    std::cout << "Verbose Test: first best fit" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    void* arr2 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[####......]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######....]");
    void* arr4 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[########..]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##########]");

    verboseFree(mm, arr2);
    verboseFree(mm, arr4);
    REQUIRE(getMemDiagram(mm) == "[##..##..##]");

    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######..##]");

    std::cout << std::endl;
}

TEST_CASE("first worst fit", "[method]") {
    std::cout << "Verbose Test: first worst fit" << std::endl;
    MemoryManager mm = MemoryManager(1, worstFit);
    mm.initialize(10);
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    void* arr2 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[####......]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######....]");
    void* arr4 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[########..]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##########]");

    verboseFree(mm, arr2);
    verboseFree(mm, arr4);
    REQUIRE(getMemDiagram(mm) == "[##..##..##]");

    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######..##]");

    std::cout << std::endl;
}

TEST_CASE("basic worst fit", "[method]") {
    std::cout << "Verbose Test: basic worst fit" << std::endl;
    MemoryManager mm = MemoryManager(1, worstFit);
    mm.initialize(12);
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##..........]");
    void* arr2 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[####........]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######......]");
    void* arr4 = verboseAllocate(mm, 4);
    REQUIRE(getMemDiagram(mm) == "[##########..]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[############]");

    verboseFree(mm, arr2);
    verboseFree(mm, arr4);
    REQUIRE(getMemDiagram(mm) == "[##..##....##]");

    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##..####..##]");

    std::cout << std::endl;
}

TEST_CASE("change of method", "[method]") {
    std::cout << "Verbose Test: change of method" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(16);
    verboseAllocate(mm, 2);
    void* arr1 = verboseAllocate(mm, 2);
    verboseAllocate(mm, 2);
    void* arr2 = verboseAllocate(mm, 2);
    verboseAllocate(mm, 2);
    void* arr3 = verboseAllocate(mm, 4);
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[################]");
    verboseFree(mm, arr1);
    verboseFree(mm, arr2);
    verboseFree(mm, arr3);
    REQUIRE(getMemDiagram(mm) == "[##..##..##....##]");

    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######..##....##]");
    mm.setAllocator(worstFit);
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[######..####..##]");

    std::cout << std::endl;
}

TEST_CASE("wordSize 2", "[align]") {
    std::cout << "Verbose Test: wordSize 2" << std::endl;
    MemoryManager mm = MemoryManager(2, bestFit);
    mm.initialize(10);
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[#.........]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[###.......]");
}

TEST_CASE("wordSize 3", "[align]") {
    std::cout << "Verbose Test: wordSize 3" << std::endl;
    MemoryManager mm = MemoryManager(3, bestFit);
    mm.initialize(10);
    verboseAllocate(mm, 3);
    REQUIRE(getMemDiagram(mm) == "[#.........]");
    verboseAllocate(mm, 3);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    verboseAllocate(mm, 3);
    REQUIRE(getMemDiagram(mm) == "[###.......]");
}

TEST_CASE("alloc required words", "[align]") {
    std::cout << "Verbose Test: alloc required words" << std::endl;
    MemoryManager mm = MemoryManager(2, bestFit);
    mm.initialize(10);
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[#.........]");
    void* arr1 = verboseAllocate(mm, 1);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[###.......]");
    void* arr2 = verboseAllocate(mm, 1);
    REQUIRE(getMemDiagram(mm) == "[####......]");
    verboseAllocate(mm, 1);
    REQUIRE(getMemDiagram(mm) == "[#####.....]");
    verboseFree(mm, arr1);
    REQUIRE(getMemDiagram(mm) == "[#.###.....]");
    verboseFree(mm, arr2);
    REQUIRE(getMemDiagram(mm) == "[#.#.#.....]");
}

TEST_CASE("alloc required words 2", "[align]") {
    std::cout << "Verbose Test: alloc required words 2" << std::endl;
    MemoryManager mm = MemoryManager(3, bestFit);
    mm.initialize(10);
    void* arr1 = verboseAllocate(mm, 1);
    REQUIRE(getMemDiagram(mm) == "[#.........]");
    void* arr2 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    void* arr3 = verboseAllocate(mm, 3);
    REQUIRE(getMemDiagram(mm) == "[###.......]");
    void* arr4 = verboseAllocate(mm, 4);
    REQUIRE(getMemDiagram(mm) == "[#####.....]");

    verboseFree(mm, arr1);
    REQUIRE(getMemDiagram(mm) == "[.####.....]");
    verboseFree(mm, arr2);
    REQUIRE(getMemDiagram(mm) == "[..###.....]");
    verboseFree(mm, arr3);
    REQUIRE(getMemDiagram(mm) == "[...##.....]");
    verboseFree(mm, arr4);
    REQUIRE(getMemDiagram(mm) == "[..........]");
}

TEST_CASE("no double free", "[input]") {
    std::cout << "Verbose Test: no double free" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    void* arr1 = verboseAllocate(mm, 2);
    REQUIRE(getMemDiagram(mm) == "[##........]");
    verboseFree(mm, arr1);
    REQUIRE(getMemDiagram(mm) == "[..........]");
    verboseFree(mm, arr1);
    REQUIRE(getMemDiagram(mm) == "[..........]");
    verboseFree(mm, nullptr);
    REQUIRE(getMemDiagram(mm) == "[..........]");
    std::cout << std::endl;
}

TEST_CASE("getList empty and full", "[input]") {
    std::cout << "Verbose Test: getList empty and full" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    uint16_t* res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res == nullptr);

    mm.initialize(10);
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res != nullptr);
    REQUIRE(res[0] == 1);
    REQUIRE(res[1] == 0);
    REQUIRE(res[2] == 10);
    delete[] res;

    verboseAllocate(mm, 10);
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res != nullptr);
    REQUIRE(res[0] == 0);
    delete[] res;

    mm.shutdown();
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res == nullptr);

    std::cout << std::endl;
}

TEST_CASE("getMemoryStart", "[input]") {
    std::cout << "Verbose Test: getMemoryStart" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    void* res = verboseAllocate(mm, 10);
    REQUIRE(res == mm.getMemoryStart());
    std::cout << std::endl;
}

TEST_CASE("getMemoryLimit", "[input]") {
    std::cout << "Verbose Test: getMemoryLimit" << std::endl;
    MemoryManager mm = MemoryManager(2, bestFit);
    mm.initialize(10);
    REQUIRE(mm.getMemoryLimit() == 20);
    verboseAllocate(mm, 10);
    REQUIRE(mm.getMemoryLimit() == 20);
    std::cout << std::endl;
}

TEST_CASE("init too much mem", "[input]") {
    std::cout << "Non-verbose Test: init too much mem" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(65535);
    uint16_t* res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res != nullptr);
    REQUIRE(res[0] == 1);
    REQUIRE(res[1] == 0);
    REQUIRE(res[2] == 65535);
    mm.shutdown();
    delete[] res;
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res == nullptr);
    mm.initialize(65537);
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res == nullptr);
    std::cout << std::endl;
}

TEST_CASE("re-init", "[input]") {
    std::cout << "Non-verbose Test: re-init" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    uint16_t* res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res != nullptr);
    REQUIRE(res[0] == 1);
    REQUIRE(res[1] == 0);
    REQUIRE(res[2] == 10);
    mm.initialize(20);
    delete[] res;
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res != nullptr);
    REQUIRE(res[0] == 1);
    REQUIRE(res[1] == 0);
    REQUIRE(res[2] == 20);
    delete[] res;
    std::cout << std::endl;
}

TEST_CASE("alloc 0", "[input]") {
    std::cout << "Verbose Test: alloc 0" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    REQUIRE(getMemDiagram(mm) == "[..........]");
    void* res = verboseAllocate(mm, 0);
    REQUIRE(res == nullptr);
    REQUIRE(getMemDiagram(mm) == "[..........]");
    std::cout << std::endl;
}

TEST_CASE("max init and alloc", "[max]") {
    std::cout << "Non-verbose Test: max init and alloc" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(65535);
    uint16_t* res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res != nullptr);
    REQUIRE(res[0] == 1);
    REQUIRE(res[1] == 0);
    REQUIRE(res[2] == 65535);
    mm.allocate(32768);
    delete[] res;
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res != nullptr);
    REQUIRE(res[0] == 1);
    REQUIRE(res[1] == 32768);
    REQUIRE(res[2] == 32767);
    mm.allocate(32767);
    delete[] res;
    res = static_cast<uint16_t*>(mm.getList());
    REQUIRE(res != nullptr);
    REQUIRE(res[0] == 0);
    delete[] res;

    std::cout << std::endl;
}

//addended test functions
/*
* Author: Steven Miller
* Date created: November 9, 2023
* Purpose: tests bitmap and dumpmemorymap
* Used by: N/A
* Uses: MemoryManager.h
*/

/*
* getbitmap test functions
*/
//test bitmap function when list hasnt been initialized
TEST_CASE("bitmap uninitialized ", "[bitmap]")
{
    std::cout << "get bitmap when list hasnt been initialized" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    uint8_t* bitmap = static_cast<uint8_t*>(mm.getBitmap());
    REQUIRE(bitmap == nullptr);
    delete[](uint8_t*)bitmap;
    mm.shutdown();
    std::cout << std::endl;
}

//test bitmap function when list has been initialized but no memory allocated
TEST_CASE("bitmap unallocated", "[bitmap]")
{
    std::cout << "get bitmap when memory hasnt been allocated" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    uint8_t* bitmap = static_cast<uint8_t*>(mm.getBitmap());
    REQUIRE(bitmap != nullptr);
    REQUIRE((int)bitmap[0] == 2);
    REQUIRE((int)bitmap[1] == 0);
    REQUIRE((int)bitmap[2] == 0);
    REQUIRE((int)bitmap[3] == 0);
    delete[](uint8_t*)bitmap;
    mm.shutdown();
    std::cout << std::endl;
}

//test bitmap function when memory has been allocated
TEST_CASE("bitmap allocated", "[bitmap]")
{
    std::cout << "get bitmap when memory has been allocated" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    mm.allocate(2);
    uint8_t* bitmap = static_cast<uint8_t*>(mm.getBitmap());
    REQUIRE(bitmap != nullptr);
    REQUIRE((int)bitmap[0] == 2);
    REQUIRE((int)bitmap[1] == 0);
    REQUIRE((int)bitmap[2] == 3);
    REQUIRE((int)bitmap[3] == 0);
    delete[](uint8_t*)bitmap;
    mm.shutdown();
    std::cout << std::endl;
}


//test bitmap function when memory has been allocated then deallocated
TEST_CASE("bitmap deallocated", "[bitmap]")
{
    std::cout << "get bitmap when memory has been allocated" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    void* pnt = mm.allocate(2);
    void* pnttwo = mm.allocate(2);
    mm.free(pnttwo);
    uint8_t* bitmap = static_cast<uint8_t*>(mm.getBitmap());
    REQUIRE(bitmap != nullptr);
    REQUIRE((int)bitmap[0] == 2);
    REQUIRE((int)bitmap[1] == 0);
    REQUIRE((int)bitmap[2] == 3);
    REQUIRE((int)bitmap[3] == 0);
    delete[](uint8_t*)bitmap;
    mm.shutdown();
    std::cout << std::endl;
}

// test bitmap function when memory deallocated at ends
TEST_CASE("bitmap completely allocated", "[bitmap]")
{
    std::cout << "get bitmap when memory has been completely allocated" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(16);
    void* pnt = mm.allocate(16);
    uint8_t* bitmap = static_cast<uint8_t*>(mm.getBitmap());
    REQUIRE(bitmap != nullptr);
    REQUIRE((int)bitmap[0] == 2);
    REQUIRE((int)bitmap[1] == 0);
    REQUIRE((int)bitmap[2] == 255);
    REQUIRE((int)bitmap[3] == 255);
    delete[](uint8_t*)bitmap;
    mm.shutdown();
    std::cout << std::endl;
}

// test bitmap function when memory deallocated at ends
TEST_CASE("bitmap deallocated at ends", "[bitmap]")
{
    std::cout << "get bitmap when memory has been deallocated at ends" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    void* pnt = mm.allocate(2);
    void* pnttwo = mm.allocate(2);
    void* pntthree = mm.allocate(2);
    void* pntfour = mm.allocate(2);
    void* pntfive = mm.allocate(2);
    mm.free(pntfive);
    mm.free(pnt);
    uint8_t* bitmap = static_cast<uint8_t*>(mm.getBitmap());
    REQUIRE(bitmap != nullptr);
    REQUIRE((int)bitmap[0] == 2);
    REQUIRE((int)bitmap[1] == 0);
    REQUIRE((int)bitmap[2] == 252);
    REQUIRE((int)bitmap[3] == 0);
    delete[](uint8_t*)bitmap;
    mm.shutdown();
    std::cout << std::endl;
}

/*
* dumpmemorymap test functions
*/

// test dumpmemorymap function when memory deallocated at ends
TEST_CASE("dumpmemorymap uninitialized", "[dumpmemorymap]")
{
    std::cout << "get dumpmemorymap when memory is uninitialized" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    int32_t res = mm.dumpMemoryMap((char*)"dumpmemorymap_uninitialized.txt");
    REQUIRE(res == -1);
    mm.shutdown();
    std::cout << std::endl;
}

// test dumpmemorymap function when memory deallocated at ends
TEST_CASE("dumpmemorymap unallocated", "[dumpmemorymap]")
{
    std::cout << "get dumpmemorymap when memory is unallocated" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    std::string filename = "dumpmemorymap_unallocated.txt";
    char dump[7];
    std::string correct = "[0, 10]";
    int32_t status = static_cast<int32_t>(mm.dumpMemoryMap((char*)filename.c_str()));
    REQUIRE(status == 0);
    file_descriptor fd = open((char*)filename.c_str(), O_RDONLY, 0777);
    ssize_t read_status = read(fd,(void*)dump,(size_t)sizeof(std::string)*correct.size());
    REQUIRE(read_status == 7);
    REQUIRE(dump == correct);
    mm.shutdown();
    std::cout << std::endl;
}


//test dumpmemorymap function when all memory allocated
TEST_CASE("dumpmemorymap full", "[dumpmemorymap]")
{
    std::cout << "get dumpmemorymap when memory is full" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    mm.allocate(10);
    std::string filename = "dumpmemorymap_full.txt";
    char dump[1] = {NULL};
    std::string correct = "";
    int32_t status = static_cast<int32_t>(mm.dumpMemoryMap((char*)filename.c_str()));
    REQUIRE(status == 0);
    file_descriptor fd = open((char*)filename.c_str(), O_RDONLY, 0777);
    ssize_t read_status = read(fd,(void*)dump,(size_t)sizeof(std::string)*correct.size());
    REQUIRE(read_status == 0);
    REQUIRE(dump == correct);
    mm.shutdown();
    std::cout << std::endl;
}

// test dumpmemorymap function when memory is allocated
TEST_CASE("dumpmemorymap allocated", "[dumpmemorymap]")
{
    std::cout << "get dumpmemorymap when memory is allocated" << std::endl;
    MemoryManager mm = MemoryManager(1, bestFit);
    mm.initialize(10);
    std::string filename = "dumpmemorymap_allocated.txt";
    void* pnt = mm.allocate(2);
    void* pnttwo = mm.allocate(2);
    void* pntthree = mm.allocate(2);
    void* pntfour = mm.allocate(2);
    void* pntfive = mm.allocate(2);
    mm.free(pntthree);
    mm.free(pnt);
    char dump[15];
    std::string correct = "[0, 2] - [4, 2]";
    int32_t status = static_cast<int32_t>(mm.dumpMemoryMap((char*)filename.c_str()));
    REQUIRE(status == 0);
    file_descriptor fd = open((char*)filename.c_str(), O_RDONLY, 0777);
    ssize_t read_status = read(fd,(void*)dump,(size_t)sizeof(std::string)*correct.size());
    REQUIRE(read_status == 15);
    REQUIRE(dump == correct);
    mm.shutdown();
    std::cout << std::endl;
}