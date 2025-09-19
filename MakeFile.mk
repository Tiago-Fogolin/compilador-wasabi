
SRC_DIR = lib
INC_DIR = lib/include
TEST_DIR = test
BUILD_DIR = build

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I$(INC_DIR)

ifeq ($(OS),Windows_NT)
    SHELL = C:/msys64/usr/bin/bash.exe
    RM = rmdir /s /q
    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    TARGET_EXT = .exe
    FIND_CMD = C:/msys64/usr/bin/find.exe
else
    SHELL = /bin/bash
    RM = rm -rf
    MKDIR = mkdir -p $(BUILD_DIR)
    TARGET_EXT =
    FIND_CMD = find
endif

SRC_FILES := $(shell $(FIND_CMD) $(SRC_DIR) -name "*.cpp")
TEST_FILES := $(shell $(FIND_CMD) $(TEST_DIR) -name "*.cpp")
ALL_SRC := $(SRC_FILES) $(TEST_FILES)

TARGET = $(BUILD_DIR)/run_tests$(TARGET_EXT)

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	$(MKDIR)

$(TARGET): $(ALL_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) $(BUILD_DIR)

.PHONY: all clean