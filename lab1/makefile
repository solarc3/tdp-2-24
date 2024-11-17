FLAGS = -Wall -std=c++11 -march=native -Ofast

# para no llenar el directorio de los .o, generamos uno
OBJ_DIR = obj

# defecto sin tracy
all: $(OBJ_DIR) water_jugs

# si se compila con tracy
tracy: FLAGS += -DTRACY_ENABLE
tracy: $(OBJ_DIR) water_jugs_tracy

# mkdir directio para los .o
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# target sin tracy
water_jugs: $(OBJ_DIR)/State.o $(OBJ_DIR)/Search.o $(OBJ_DIR)/Heap.o $(OBJ_DIR)/HashTable.o $(OBJ_DIR)/Solver.o $(OBJ_DIR)/main.o
	clang ${FLAGS} $(OBJ_DIR)/*.o -o water_jugs

# target con tracy agregado
water_jugs_tracy: $(OBJ_DIR)/State.o $(OBJ_DIR)/Search.o $(OBJ_DIR)/Heap.o $(OBJ_DIR)/HashTable.o $(OBJ_DIR)/Solver.o $(OBJ_DIR)/main.o
	clang ${FLAGS} $(OBJ_DIR)/*.o /usr/lib/libTracyClient.a -o water_jugs

# compilacion para cada objecto y sus dependencias
# no hay problemas con los .h por los pragma once
$(OBJ_DIR)/main.o: src/main.cpp
	clang ${FLAGS} -I./include -c src/main.cpp -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/State.o: src/State.cpp include/State.h
	clang ${FLAGS} -I./include -c src/State.cpp -o $(OBJ_DIR)/State.o

$(OBJ_DIR)/Search.o: src/Search.cpp include/Search.h
	clang ${FLAGS} -I./include -c src/Search.cpp -o $(OBJ_DIR)/Search.o

$(OBJ_DIR)/Heap.o: src/Heap.cpp include/Heap.h
	clang ${FLAGS} -I./include -c src/Heap.cpp -o $(OBJ_DIR)/Heap.o

$(OBJ_DIR)/HashTable.o: src/HashTable.cpp include/HashTable.h
	clang ${FLAGS} -I./include -c src/HashTable.cpp -o $(OBJ_DIR)/HashTable.o

$(OBJ_DIR)/Solver.o: src/Solver.cpp include/Solver.h
	clang ${FLAGS} -I./include -c src/Solver.cpp -o $(OBJ_DIR)/Solver.o

# si es que se compilo, borramos la carpeta y el ejecutable
clean:
	rm -rf $(OBJ_DIR) water_jugs
