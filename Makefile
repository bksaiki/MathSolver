BUILD_DIR	?= ./build
SRC_DIR 	?= ./src
TEST_DIR	?= ./tests

SRCS 		:= $(shell find $(SRC_DIR) -name *.cpp)
OBJS 		:= $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS 		:= $(OBJS:.o=.d)

TESTS 		:= $(shell find $(TEST_DIR) -name *.cpp)
TEST_EXES 	:= $(TESTS:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/%)
TEST_DEPS	:= $(TEST_EXES:%=%.d)

CFLAGS 		?= -MMD -MP -g -m64 -Wall
LDFLAGS		?= 

test: $(TEST_EXES)
	$(TEST_DIR)/test.sh $(TEST_EXES)

build: $(OBJS);

clean:
	$(RM) $(OBJS) $(TEST_EXES)

clean-deps:
	$(RM) -r $(DEPS) $(TEST_DEPS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%: $(TEST_DIR)/%.cpp $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $< $(OBJS)

-include $(DEPS) $(TEST_DEPS)
.PHONY: test build clean clean-deps