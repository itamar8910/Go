# Declaration of variables
CC = g++
CC_FLAGS = -g -I include/ -Wall -std=c++11
 
# File names
EXEC = tests
SOURCES = $(wildcard src/*.cpp)
SOURCES_NAMES = $(patsubst src/%.cpp, %,$(SOURCES))
OBJECTS = $(patsubst %, %.o,$(SOURCES_NAMES))
OBJECTS_PATH = $(patsubst %, build/%.o,$(SOURCES_NAMES))
TEST_OBJECTS_PATH = $(filter-out build/test_performance.o, $(OBJECTS_PATH))
PERFORAMNCE_OBJECTS_PATH = $(filter-out build/tests.o, $(OBJECTS_PATH))
BUILD_DIR = build
SRC_DIR = src 

# Main target
all: tests test_performance

tests: $(OBJECTS)
	$(CC) $(CC_FLAGS) $(TEST_OBJECTS_PATH) -o $(BUILD_DIR)/tests

test_performance:
	$(CC) $(CC_FLAGS) $(PERFORAMNCE_OBJECTS_PATH) -o $(BUILD_DIR)/test_performance

%.o: src/%.cpp include/%.hpp
	$(CC) -c $(CC_FLAGS) $< -o $(BUILD_DIR)/$@

%.o: src/%.cpp
	$(CC) -c $(CC_FLAGS) $< -o $(BUILD_DIR)/$@

dbg:
	echo $(OBJECTS_PATH)
clean:
	rm -f $(BUILD_DIR)/$(EXEC) $(BUILD_DIR)/$(OBJECTS)