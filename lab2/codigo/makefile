# Compiler flags
FLAGS = -Wall -std=gnu++17 -march=native -Ofast

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
TEST_DIR = test

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJS = $(TEST_SRCS:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)

all: $(OBJ_DIR) graph

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

graph: $(OBJS) $(TEST_OBJS)
	g++ ${FLAGS} $^ -o $@


# generico objects excepto main
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/%.h
	g++ ${FLAGS} -I$(INC_DIR) -I$(TEST_DIR) -c $< -o $@

#test
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp $(TEST_DIR)/%.h
	g++ ${FLAGS} -I$(INC_DIR) -I$(TEST_DIR) -c $< -o $@


$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp
	g++ ${FLAGS} -I$(INC_DIR) -I$(TEST_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) graph

# Dependencies
$(OBJ_DIR)/Graph.o: $(SRC_DIR)/Graph.cpp $(INC_DIR)/Graph.h
$(OBJ_DIR)/ColoringState.o: $(SRC_DIR)/ColoringState.cpp $(INC_DIR)/ColoringState.h $(INC_DIR)/Graph.h
$(OBJ_DIR)/DangerHeuristic.o: $(SRC_DIR)/DangerHeuristic.cpp $(INC_DIR)/DangerHeuristic.h $(INC_DIR)/ColoringState.h $(INC_DIR)/Graph.h
$(OBJ_DIR)/Bounds.o: $(SRC_DIR)/Bounds.cpp $(INC_DIR)/Bounds.h $(INC_DIR)/Graph.h $(INC_DIR)/ColoringState.h $(INC_DIR)/DangerHeuristic.h
$(OBJ_DIR)/BranchAndBound.o: $(SRC_DIR)/BranchAndBound.cpp $(INC_DIR)/BranchAndBound.h $(INC_DIR)/Graph.h $(INC_DIR)/ColoringState.h $(INC_DIR)/Bounds.h $(INC_DIR)/DangerHeuristic.h
$(OBJ_DIR)/Solve.o: $(SRC_DIR)/Solve.cpp $(INC_DIR)/Solve.h $(INC_DIR)/Graph.h $(INC_DIR)/ColoringState.h $(INC_DIR)/Bounds.h $(INC_DIR)/DangerHeuristic.h $(INC_DIR)/BranchAndBound.h
