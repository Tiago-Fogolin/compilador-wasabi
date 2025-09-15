# Diretórios
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BUILD_DIR = build

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I$(INC_DIR)

# Detecta SO
ifeq ($(OS),Windows_NT)
    RM = rmdir /s /q
    MKDIR = if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
    FIND_SRC = $(shell dir /b /s $(SRC_DIR)\*.cpp)
    FIND_TEST = $(shell dir /b /s $(TEST_DIR)\*.cpp)
    TARGET_EXT = .exe
else
    RM = rm -rf
    MKDIR = mkdir -p $(BUILD_DIR)
    FIND_SRC = $(shell find $(SRC_DIR) -type f -name "*.cpp")
    FIND_TEST = $(shell find $(TEST_DIR) -type f -name "*.cpp")
    TARGET_EXT =
endif

SRC_FILES := $(FIND_SRC)
TEST_FILES := $(FIND_TEST)
ALL_SRC := $(SRC_FILES) $(TEST_FILES)

TARGET = $(BUILD_DIR)/run_tests$(TARGET_EXT)

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	$(MKDIR)

$(TARGET): $(ALL_SRC)
	$(CXX) $(CXXFLAGS) $^ -o $(TARGET)

clean:
	$(RM) $(BUILD_DIR)

.PHONY: all clean
