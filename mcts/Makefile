# Declaration of variables
CC = g++
CC_FLAGS = -g -I include/ -Wall -std=c++11
 
# File names
EXEC = tests
SOURCES = $(wildcard src/*.cpp)
SOURCES_NAMES = $(patsubst src/%.cpp, %,$(SOURCES))
OBJECTS = $(patsubst %, %.o,$(SOURCES_NAMES))
OBJECTS_PATH = $(patsubst %, build/%.o,$(SOURCES_NAMES))
TEST_OBJECTS_PATH = $(filter-out build/playground.o, $(filter-out build/test_performance.o, $(OBJECTS_PATH)))
PERFORAMNCE_OBJECTS_PATH = $(filter-out build/playground.o, $(filter-out build/tests.o, $(OBJECTS_PATH)))
PLAYGROUND_OBJECTS_PATH = $(filter-out build/tests.o, $(filter-out build/test_performance.o, $(OBJECTS_PATH)))
BUILD_DIR = build
SRC_DIR = src 
ROOT_DIR := $(shell pwd)

# Main target
all: tests test_performance playground

tests: $(OBJECTS_PATH)
	$(CC) $(CC_FLAGS) $(TEST_OBJECTS_PATH) -o $(BUILD_DIR)/tests

test_performance: $(OBJECTS_PATH)
	$(CC) $(CC_FLAGS) $(PERFORAMNCE_OBJECTS_PATH) -o $(BUILD_DIR)/test_performance

playground: $(OBJECTS_PATH)
	$(CC) $(CC_FLAGS) $(PLAYGROUND_OBJECTS_PATH) -o $(BUILD_DIR)/playground

build/%.o: $(ROOT_DIR)/src/%.cpp $(ROOT_DIR)/include/%.hpp
	$(CC) -c $(CC_FLAGS) $< -o $@

build/%.o: $(ROOT_DIR)/src/%.cpp
	$(CC) -c $(CC_FLAGS) $< -o $@

dbg:
	echo $(TEST_OBJECTS_PATH)

clean:
	rm -f $(BUILD_DIR)/*