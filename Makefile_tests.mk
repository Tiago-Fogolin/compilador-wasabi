SRC_DIR = lib
INC_DIR = lib/include
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

# Inclui todos os .cpp da lib, exceto compilador.cpp
SRC_FILES := $(shell $(FIND_CMD) $(SRC_DIR) -name "*.cpp" ! -name "compilador.cpp")

TARGET = $(BUILD_DIR)/testes$(TARGET_EXT)

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	$(MKDIR)

$(TARGET): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	$(RM) $(BUILD_DIR)

.PHONY: all clean
