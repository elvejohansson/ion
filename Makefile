TARGET := ion

CXX = g++
CXXFLAGS := -Wall -Werror -Wshadow -pedantic -std=c++11

SRC_DIR := src
OUT_DIR := build

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:%=$(OUT_DIR)/%.o)

.PHONY: run
run: $(OUT_DIR)/$(TARGET)
	./$(OUT_DIR)/$(TARGET) test.ion
	@mv program.s ./$(OUT_DIR)/program.s
	@echo "\ncompiling output assembly...\n"
	@as ./$(OUT_DIR)/program.s -o $(OUT_DIR)/program.o
	@ld $(OUT_DIR)/program.o -o $(OUT_DIR)/program -e _start
	./$(OUT_DIR)/program

$(OUT_DIR)/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

$(OUT_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OUT_DIR)
