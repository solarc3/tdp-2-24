FLAGS = -Wall -std=gnu++17 -march=native -Ofast

# Directory for object files
OBJ_DIR = obj

all: $(OBJ_DIR) graph

tracy: FLAGS += -DTRACY_ENABLE
tracy: $(OBJ_DIR) graph_tracy

# directorio para los obj
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# sin tracy
graph: $(OBJ_DIR)/Graph.o $(OBJ_DIR)/main.o
	g++ ${FLAGS} $(OBJ_DIR)/*.o -o graph

# con tracy
graph_tracy: $(OBJ_DIR)/Graph.o $(OBJ_DIR)/main.o
	g++ ${FLAGS} $(OBJ_DIR)/*.o /usr/lib/libTracyClient.a -o graph

# rules para cada object
$(OBJ_DIR)/main.o: src/main.cpp
	g++ ${FLAGS} -I./include -c src/main.cpp -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/Graph.o: src/Graph.cpp include/Graph.h
	g++ ${FLAGS} -I./include -c src/Graph.cpp -o $(OBJ_DIR)/Graph.o

clean:
	rm -rf $(OBJ_DIR) graph
