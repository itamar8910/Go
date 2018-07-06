tests: build/GoLogic.o
	g++ -g -Wall -std=c++11 -o build/tests.o tests.cpp build/GoLogic.o

build/GoLogic.o: GoLogic.cpp
	g++ -g -c -Wall --std=c++11 GoLogic.cpp -o build/GoLogic.o
clean:
	rm build/GoLogic.o
