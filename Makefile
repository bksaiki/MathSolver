BUILD_DIR	:= ./build
SRC_DIR 	:= ./src
TEST_DIR	:= ./tests

SRCS 		:= $(shell find $(SRC_DIR) -name *.cpp)
OBJS 		:= $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS 		:= $(OBJS:.o=.d)

TESTS 		:= $(shell find $(TEST_DIR) -name *.cpp)
TEST_EXES 	:= $(TESTS:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/%)
TEST_DEPS	:= $(TEST_EXES:%=%.d)

CFLAGS 		:= -MMD -MP -g -m64 -Wall -std=c++17
LDFLAGS		:= 

.PRECIOUS: $(BUILD_DIR)/. $(BUILD_DIR)%/.
.SECONDEXPANSION: $(BUILD_DIR)/%.o
.PHONY: tests test-integer test-parser memcheck build clean clean-deps clean-all setup

# Specific rules

tests: $(TEST_EXES)
	$(TEST_DIR)/test.sh $(TEST_EXES)

memcheck: $(TEST_EXES)
	$(TEST_DIR)/memcheck.sh $(TEST_EXES)

build-tests: $(TEST_EXES);

build: $(OBJS);

clean:
	$(RM) $(OBJS) $(TEST_EXES)

clean-deps:
	$(RM) -r $(DEPS) $(TEST_DEPS)

clean-all:
	rm -r -f build

### Specific tests

test-integer: build/test-integer
	$(TEST_DIR)/test.sh build/test-integer

test-integermath: build/test-integermath
	$(TEST_DIR)/test.sh build/test-integermath

test-parser: build/test-parser
	$(TEST_DIR)/test.sh build/test-parser

# not tracked
test-sandbox: build/test-sandbox
	$(TEST_DIR)/test.sh build/test-sandbox

### General rules

$(BUILD_DIR)/.:
	mkdir -p $@

$(BUILD_DIR)%/.:
	mkdir -p $@


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $$(@D)/.
	$(CXX) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%: $(TEST_DIR)/%.cpp $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $(OBJS) $< 

-include $(DEPS) $(TEST_DEPS)


