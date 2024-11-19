# Compiler flags
FLAGS = -Wall -Wmaybe-uninitialized -Wextra -std=gnu++17 -march=native -Ofast

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Default target
all: $(OBJ_DIR) graph

# Tracy configuration
tracy: FLAGS += -DTRACY_ENABLE
tracy: $(OBJ_DIR) graph_tracy

# Create object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Regular build
graph: $(OBJS)
	g++ ${FLAGS} $^ -o $@

# Tracy build
graph_tracy: $(OBJS)
	g++ ${FLAGS} $^ /usr/lib/libTracyClient.a -o $@

# Generic rule for object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INC_DIR)/%.h
	g++ ${FLAGS} -I$(INC_DIR) -c $< -o $@

# Special case for main.o which might not have a header
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp
	g++ ${FLAGS} -I$(INC_DIR) -c $< -o $@

# Clean rule
clean:
	rm -rf $(OBJ_DIR) graph graph_tracy

# Dependencies
$(OBJ_DIR)/Graph.o: $(SRC_DIR)/Graph.cpp $(INC_DIR)/Graph.h
$(OBJ_DIR)/ColoringState.o: $(SRC_DIR)/ColoringState.cpp $(INC_DIR)/ColoringState.h $(INC_DIR)/Graph.h
$(OBJ_DIR)/DangerHeuristic.o: $(SRC_DIR)/DangerHeuristic.cpp $(INC_DIR)/DangerHeuristic.h $(INC_DIR)/ColoringState.h $(INC_DIR)/Graph.h
$(OBJ_DIR)/Bounds.o: $(SRC_DIR)/Bounds.cpp $(INC_DIR)/Bounds.h $(INC_DIR)/Graph.h $(INC_DIR)/ColoringState.h $(INC_DIR)/DangerHeuristic.h
$(OBJ_DIR)/BranchAndBound.o: $(SRC_DIR)/BranchAndBound.cpp $(INC_DIR)/BranchAndBound.h $(INC_DIR)/Graph.h $(INC_DIR)/ColoringState.h $(INC_DIR)/Bounds.h $(INC_DIR)/DangerHeuristic.h
$(OBJ_DIR)/Solve.o: $(SRC_DIR)/Solve.cpp $(INC_DIR)/Solve.h $(INC_DIR)/Graph.h $(INC_DIR)/ColoringState.h $(INC_DIR)/Bounds.h $(INC_DIR)/DangerHeuristic.h $(INC_DIR)/BranchAndBound.h
$(OBJ_DIR)/ColorDependency.o: $(SRC_DIR)/ColorDependency.cpp $(INC_DIR)/ColorDependency.h $(INC_DIR)/Graph.h $(INC_DIR)/ColoringState.h

.PHONY: all clean tracy
