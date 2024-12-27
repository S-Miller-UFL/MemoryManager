This is a replacement for the "new" operator in C++.

It uses an std::map to implement a best fit and a worst fit algorithm for memory management.

This library should work under any OS, but if you want to include it as a library, its recommended you do it
under a linux kernel named reptilian. https://cise.ufl.edu/research/reptilian/wiki/doku.php
Functionality with other OS's is not guaranteed.

to use this library, do the following:
1. download MemoryManager.tgz
2. input the following commands
  "tar zxvf MemoryManager.tgz"
  "cd MemoryManager"
  "make"
  "cd .."
3. to link against the memory manager library, input the following command:
     "c++ -std=c++17 -o [program_name] [sourcefilename].c -L ./MemoryManager -lMemoryManager

to allocate memory in your program, simply create a memory manager object like the following:
MemoryManager mm(amountofmemorywanted, bestFit); //if you want the best fit algorithm
MemoryManager mm(amountofmemorywanted, worstFit); //if you want the worst fit algorithm
