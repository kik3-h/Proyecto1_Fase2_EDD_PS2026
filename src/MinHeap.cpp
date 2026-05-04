#include "MinHeap.h"

#include <climits> //esta libreria hace referencia a los valores máximos y mínimos de los tipos de datos enteros

MinHeap::MinHeap() : arreglo(new NodoHeap[16]), capacidad(16), tamano(0) {}

MinHeap::~MinHeap() {
    delete[] arreglo;
    arreglo = nullptr;
    capacidad = 0;
    tamano = 0;
}

void MinHeap::intercambiar(NodoHeap& a, NodoHeap& b) { // Intercambia dos nodos en el heap.
    NodoHeap temporal = a;
    a = b;
    b = temporal;
}

void MinHeap::redimensionar() { // Duplica la capacidad del arreglo interno cuando se alcanza el límite. Complejidad O(n) debido a la copia de elementos.
    const int nuevaCapacidad = capacidad * 2;
    NodoHeap* nuevoArreglo = new NodoHeap[nuevaCapacidad];

    for (int i = 0; i < tamano; ++i) {
        nuevoArreglo[i] = arreglo[i]; // Se copian los elementos existentes al nuevo arreglo. Complejidad O(n), donde n es el número de elementos actuales en el heap.
    }

    delete[] arreglo;
    arreglo = nuevoArreglo;
    capacidad = nuevaCapacidad;
}

void MinHeap::flotar(int indice) { // Reestablece la propiedad de heap hacia arriba desde un índice dado. O(log n) debido a la altura del heap.
    while (indice > 0) {
        const int padre = (indice - 1) / 2;
        if (arreglo[padre].distancia <= arreglo[indice].distancia) { // Si el padre es menor o igual al nodo actual, la propiedad de heap se cumple y se detiene el proceso.
            break;
        }

        intercambiar(arreglo[padre], arreglo[indice]); // Se intercambian el nodo actual con su padre para mantener la propiedad de heap. Complejidad O(1).
        indice = padre;
    }
}

void MinHeap::hundir(int indice) { // Reestablece la propiedad de heap hacia abajo desde un índice dado. O(log n) debido a la altura del heap.
    while (true) {
        const int hijoIzquierdo = (indice * 2) + 1;
        const int hijoDerecho = hijoIzquierdo + 1;
        int menor = indice;

        if (hijoIzquierdo < tamano &&
            arreglo[hijoIzquierdo].distancia < arreglo[menor].distancia) { // Si el hijo izquierdo es menor que el nodo actual, se marca como el menor. Complejidad O(1).
            menor = hijoIzquierdo;
        }

        if (hijoDerecho < tamano &&
            arreglo[hijoDerecho].distancia < arreglo[menor].distancia) { // Si el hijo derecho es menor que el nodo marcado como menor, se actualiza el menor. Complejidad O(1).
            menor = hijoDerecho;
        }

        if (menor == indice) {
            break;
        }

        intercambiar(arreglo[indice], arreglo[menor]); // Se intercambia el nodo actual con el nodo menor para mantener la propiedad de heap. Complejidad O(1).
        indice = menor;
    }
}

void MinHeap::insertar(int idSucursal, int distancia) { // Inserta un nuevo nodo con el idSucursal y distancia dada, y luego aplica heapify-up para mantener la propiedad de heap. O(log n) debido a la altura del heap.
    if (tamano >= capacidad) {
        redimensionar();
    }

    arreglo[tamano].idSucursal = idSucursal; // Se asigna el idSucursal al nuevo nodo en la posición de inserción. Complejidad O(1).
    arreglo[tamano].distancia = distancia;
    flotar(tamano);
    ++tamano;
}

MinHeap::NodoHeap MinHeap::extraerMinimo() { // Extrae el nodo con la menor distancia (el mínimo) del heap, lo reemplaza con el último nodo y luego aplica heapify-down para mantener la propiedad de heap. O(log n) debido a la altura del heap.
    if (tamano == 0) {
        NodoHeap vacio;
        vacio.idSucursal = -1;
        vacio.distancia = INT_MAX;
        return vacio;
    }

    NodoHeap minimo = arreglo[0]; // Se guarda el nodo mínimo (raíz del heap) para devolverlo al final. Complejidad O(1).
    --tamano;

    if (tamano > 0) {
        arreglo[0] = arreglo[tamano]; // Se reemplaza la raíz del heap con el último nodo para mantener la estructura completa del heap. Complejidad O(1).
        hundir(0);
    }

    return minimo;
}

bool MinHeap::vacia() const {
    return tamano == 0;
}
