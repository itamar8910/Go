
CC=g++
CFLAGS=-g -I include/ -Wall -std=c++11

all: tests

tests: tests.o
	$(CC) $(CFLAGS) -o build/tests build/tests.o build/GoLogic.o build/mcts.o

test_performance: build/test_performance.o
	$(CC) $(CFLAGS) -o build/test_performance build/test_performance.o build/GoLogic.o

build/test_performance.o: src/test_performance.cpp build/GoLogic.o
	$(CC) $(CFLAGS) -o build/test_performance.o -c src/test_performance.cpp

build/mcts.o: src/mcts.cpp include/mcts.hpp
	$(CC) $(CFLAGS) -o build/mcts.o -c src/mcts.cpp 

build/GoLogic.o: src/GoLogic.cpp include/GoLogic.hpp
	$(CC) $(CFLAGS) -o build/GoLogic.o -c src/GoLogic.cpp

tests.o: src/tests.cpp include/catch.hpp build/mcts.o build/GoLogic.o
	$(CC) $(CFLAGS) -o build/tests.o -c src/tests.cpp

clean:
	rm build/*.o
