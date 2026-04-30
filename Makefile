CXX = g++
FLAGS_COMPILACION = -Wall -Wextra -std=c++17 -O2
RUTA_FUENTES = src
RUTA_CABECERAS = include
EJECUTABLE = catalogoEdp1kike

ARCHIVOS_CPP = $(wildcard $(RUTA_FUENTES)/*.cpp)
ARCHIVOS_OBJ = $(ARCHIVOS_CPP:.cpp=.o)

.PHONY: all clean run

all: build
	cmake --build build

build:
	@mkdir -p build
	cd build && cmake ..

clean:
	rm -rf build

run: all
	./build/UICatalogoP2EddKike