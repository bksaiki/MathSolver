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
CXXFLAGS 	:= -g -O0 -Wall -std=c++17
LDFLAGS 	:= -lmpfr -lgmp

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.
.SECONDEXPANSION: $(BUILD_DIR)/%.o

# Specific rules

all: build-tests main;

main: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(SRC_DIR)/$(ENTRY) $(LDFLAGS) -o $(EXE)

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
	rm -r -f $(BUILD_DIR) tmp $(EXE)

### Specific tests

test-integer: build/test-integer
	$(TEST_DIR)/test.sh build/test-integer

test-float: build/test-float
	$(TEST_DIR)/test.sh build/test-float

test-integermath: build/test-integermath
	$(TEST_DIR)/test.sh build/test-integermath

test-floatmath: build/test-floatmath
	$(TEST_DIR)/test.sh build/test-floatmath

test-range: build/test-range
	$(TEST_DIR)/test.sh build/test-range

test-parser: build/test-parser
	$(TEST_DIR)/test.sh build/test-parser

test-arithmetic: build/test-arithmetic
	$(TEST_DIR)/test.sh build/test-arithmetic

test-inequality: build/test-inequality
	$(TEST_DIR)/test.sh build/test-inequality

test-boolean: build/test-boolean
	$(TEST_DIR)/test.sh build/test-boolean

# not tracked
test-sandbox: build/test-sandbox
	$(TEST_DIR)/test.sh build/test-sandbox

### General rules

$(BUILD_DIR)/.:
	mkdir -p $@

$(BUILD_DIR)%/.:
	mkdir -p $@

$(BUILD_DIR)/$(LIB_DIR)/%.o: $(LIB_DIR)/%.cpp | $$(@D)/.
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp | $$(@D)/.
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $< $(LDFLAGS)

$(BUILD_DIR)/%: $(TEST_DIR)/%.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -o $@ $(OBJS) $< $(LDFLAGS)

-include $(DEPS)
.PHONY: build clean clean-deps clean-all setup tests test-integer test-float test-parser test-sandbox test-integermath test-memcheck \
		test-boolean
