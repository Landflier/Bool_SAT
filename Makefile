CC = gcc
CFLAGS = -Wall -Wextra -g -O2
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.c)
SRC_NO_TEST = $(filter-out $(SRC_DIR)/test_main.c, $(SRC))
SRC_NO_MAIN = $(filter-out $(SRC_DIR)/main.c, $(SRC))

OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_NO_TEST))
OBJ_TEST = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_NO_MAIN))

TARGET = $(BIN_DIR)/sat_solver
TARGET_TEST = $(BIN_DIR)/sat_solver_test

.PHONY: all clean run test tests

all: $(TARGET)

$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(TARGET_TEST): $(OBJ_TEST) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

run: $(TARGET)
	$(TARGET)

# Simple tests with example files
test: $(TARGET)
	@echo "Running tests..."
	@echo "\nTest 1: Simple satisfiable formula"
	$(TARGET) examples/simple.cnf
	@echo "\nTest 2: Unsatisfiable formula"
	$(TARGET) examples/unsatisfiable.cnf
	@echo "\nTest 3: 3-SAT formula"
	$(TARGET) examples/3sat.cnf
	@echo "\nTest 4: Pigeonhole principle"
	$(TARGET) examples/pigeonhole.cnf
	@echo "\nAll tests completed."

test-SATLIB: $(TARGET)
	$(TARGET) examples/CBS_k3_n100_m423_b50/CBS_k3_n100_m423_b50_8.cnf
# Comprehensive test suite
tests: $(TARGET_TEST)
	$(TARGET_TEST) 