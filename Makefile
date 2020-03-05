BUILD_DIR	?= ./build
SRC_DIR 	?= ./src
TEST_DIR	?= ./tests

SRCS 		:= $(shell find $(SRC_DIR) -name *.cpp)
OBJS 		:= $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS 		:= $(OBJS:.o=.d)

TESTS 		:= $(shell find $(TEST_DIR) -name *.cpp)
TEST_EXES 	:= $(TESTS:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/%)
TEST_DEPS	:= $(TEST_EXES:%=%.d)

CFLAGS 		?= -MMD -MP -g -m64 -Wall -std=c++17
LDFLAGS		?= 

test-all: tests memcheck;

tests: build-tests
	$(TEST_DIR)/test.sh $(TEST_EXES)

memcheck: build-tests
	$(TEST_DIR)/memcheck.sh $(TEST_EXES)

build-tests: $(TEST_EXES);

build: setup $(OBJS);

setup:
	mkdir -p build/test
	mkdir -p build/types
	mkdir -p build/expression

clean:
	$(RM) $(OBJS) $(TEST_EXES)

clean-deps:
	$(RM) -r $(DEPS) $(TEST_DEPS)

clean-all:
	rm -r build

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%: $(TEST_DIR)/%.cpp $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $< $(OBJS)

-include $(DEPS) $(TEST_DEPS)
.PHONY: test memcheck build clean clean-deps clean-all setup