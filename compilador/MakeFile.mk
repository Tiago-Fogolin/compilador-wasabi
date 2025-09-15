# Diretórios
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BUILD_DIR = build


CXX = g++
CXXFLAGS = -std=c++17 -Wall -I$(INC_DIR)


SRC_FILES := $(shell dir /b /s $(SRC_DIR)\*.cpp)
TEST_FILES := $(shell dir /b /s $(TEST_DIR)\*.cpp)
ALL_SRC := $(SRC_FILES) $(TEST_FILES)


TARGET = $(BUILD_DIR)/run_tests.exe

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

$(TARGET): $(ALL_SRC)
	$(CXX) $(CXXFLAGS) $^ -o "$(TARGET)"

clean:
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

.PHONY: all clean