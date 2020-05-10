BUILD_DIR	:= build
LIB_DIR 	:= lib
SRC_DIR 	:= src
TEST_DIR	:= tests

ENTRY		:= main.cpp
EXE			:= msolve

SRCS 		:= $(shell find $(LIB_DIR) $(SRC_DIR) -name *.cpp ! -name $(ENTRY) )
OBJS 		:= $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS 		:= $(OBJS:.o=.d)

TESTS 		:= $(shell find $(TEST_DIR) -name *.cpp)
TEST_EXES 	:= $(TESTS:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/%)

DEPFLAGS 	:= -MMD -MP
CFLAGS 		:= -g -m64 -Wall -std=c++17

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.
.SECONDEXPANSION: $(BUILD_DIR)/%.o

# Specific rules

all: build-tests main;

main: $(OBJS)
	$(CXX) $(CFLAGS) $(OBJS) $(SRC_DIR)/$(ENTRY) -o $(EXE)

tests: $(OBJS) $(TEST_EXES)
	$(TEST_DIR)/test.sh $(TEST_EXES)

memcheck: $(OBJS) $(TEST_EXES)
	$(TEST_DIR)/memcheck.sh $(TEST_EXES)

build-tests: $(TEST_EXES);

build: $(OBJS);

clean:
	$(RM) $(OBJS) $(TEST_EXES) $(EXE)

clean-deps:
	$(RM) -r $(DEPS) $(TEST_DEPS)

clean-all:
	rm -r -f build $(EXE)

### Specific tests

test-integer: build/test-integer
	$(TEST_DIR)/test.sh build/test-integer

test-integermath: build/test-integermath
	$(TEST_DIR)/test.sh build/test-integermath

test-parser: build/test-parser
	$(TEST_DIR)/test.sh build/test-parser

test-arithmetic: build/test-arithmetic
	$(TEST_DIR)/test.sh build/test-arithmetic

# not tracked
test-sandbox: build/test-sandbox
	$(TEST_DIR)/test.sh build/test-sandbox

### General rules

$(BUILD_DIR)/.:
	mkdir -p $@

$(BUILD_DIR)%/.:
	mkdir -p $@

$(BUILD_DIR)/$(LIB_DIR)/%.o: $(LIB_DIR)/%.cpp | $$(@D)/.
	$(CXX) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp | $$(@D)/.
	$(CXX) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

$(BUILD_DIR)/%: $(TEST_DIR)/%.cpp $(OBJS)
	$(CXX) $(CFLAGS) $(DEPFLAGS) -o $@ $(OBJS) $<

-include $(DEPS)
.PHONY: tests test-integer test-parser test-sandbox test-integermath test-memcheck build clean clean-deps clean-all setup
