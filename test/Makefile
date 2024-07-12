build:
	cd ../MemoryManager && make
	make test

test: out/catch.o test.cpp ../MemoryManager/libMemoryManager.a
	g++ test.cpp out/catch.o -o test -L ../MemoryManager -lMemoryManager -g

out/catch.o: catch/catch_amalgamated.cpp catch/catch_amalgamated.hpp
	g++ -c catch/catch_amalgamated.cpp -o out/catch.o

clean:
	rm out/catch.o
	rm test
