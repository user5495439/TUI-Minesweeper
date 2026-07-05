CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -Isrc/include

DEBUG ?= 0

ifeq ($(DEBUG),1)
    CXXFLAGS += -g -DDEBUG
else
    CXXFLAGS += -O2
endif

SRC_DIR = src
BUILD_DIR = build

SRC = $(shell find $(SRC_DIR) -name "*.cpp")
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEP = $(OBJ:.o=.d)

TARGET = minesweeper

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEP)

clean:
	rm -rf $(BUILD_DIR)
