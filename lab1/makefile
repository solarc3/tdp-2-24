FLAGS = -Wall -std=c++11 -march=native -Ofast

# Directorios
OBJ_DIR = obj

# Target por defecto (sin tracy)
all: $(OBJ_DIR) water_jugs

# Target con tracy
tracy: FLAGS += -DTRACY_ENABLE
tracy: $(OBJ_DIR) water_jugs_tracy

# Crear directorio obj si no existe
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Ejecutable principal sin tracy
water_jugs: $(OBJ_DIR)/State.o $(OBJ_DIR)/Search.o $(OBJ_DIR)/Heap.o $(OBJ_DIR)/HashTable.o $(OBJ_DIR)/Solver.o $(OBJ_DIR)/main.o
	g++ ${FLAGS} $(OBJ_DIR)/*.o -o water_jugs

# Ejecutable con tracy
water_jugs_tracy: $(OBJ_DIR)/State.o $(OBJ_DIR)/Search.o $(OBJ_DIR)/Heap.o $(OBJ_DIR)/HashTable.o $(OBJ_DIR)/Solver.o $(OBJ_DIR)/main.o
	g++ ${FLAGS} $(OBJ_DIR)/*.o /usr/lib/libTracyClient.a -o water_jugs

# Reglas de compilacion para cada objeto
$(OBJ_DIR)/main.o: src/main.cpp
	g++ ${FLAGS} -I./include -c src/main.cpp -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/State.o: src/State.cpp include/State.h
	g++ ${FLAGS} -I./include -c src/State.cpp -o $(OBJ_DIR)/State.o

$(OBJ_DIR)/Search.o: src/Search.cpp include/Search.h
	g++ ${FLAGS} -I./include -c src/Search.cpp -o $(OBJ_DIR)/Search.o

$(OBJ_DIR)/Heap.o: src/Heap.cpp include/Heap.h
	g++ ${FLAGS} -I./include -c src/Heap.cpp -o $(OBJ_DIR)/Heap.o

$(OBJ_DIR)/HashTable.o: src/HashTable.cpp include/HashTable.h
	g++ ${FLAGS} -I./include -c src/HashTable.cpp -o $(OBJ_DIR)/HashTable.o

$(OBJ_DIR)/Solver.o: src/Solver.cpp include/Solver.h
	g++ ${FLAGS} -I./include -c src/Solver.cpp -o $(OBJ_DIR)/Solver.o





# Limpiar
clean:
	rm -rf $(OBJ_DIR) water_jugs

.PHONY: all clean tracy
