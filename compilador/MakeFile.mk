# Diretórios
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BUILD_DIR = build

# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I$(INC_DIR)

# Encontrar todos os arquivos .cpp do projeto e testes (recursivamente)
# SRC_FILES -> todos os .cpp em src
SRC_FILES := $(shell dir /b /s $(SRC_DIR)\*.cpp)
# TEST_FILES -> todos os .cpp dentro de tests e subpastas
TEST_FILES := $(shell dir /b /s $(TEST_DIR)\*.cpp)
# Combina os dois
ALL_SRC := $(SRC_FILES) $(TEST_FILES)

# Executável
TARGET = $(BUILD_DIR)/run_tests.exe

# Regra principal
all: $(BUILD_DIR) $(TARGET)

# Criar pasta build se não existir
$(BUILD_DIR):
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)

# Compilar todos os .cpp em um executável
$(TARGET): $(ALL_SRC)
	$(CXX) $(CXXFLAGS) $^ -o "$(TARGET)"

# Limpar build
clean:
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)

.PHONY: all clean
