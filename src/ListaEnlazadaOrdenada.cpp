#include "ListaEnlazadaOrdenada.h"

#include <iostream>
//esta clase hace la gestión de una lista enlazada ordenada de productos 
NodoOrdenado::NodoOrdenado(Producto* producto) : producto(producto), siguiente(nullptr) {}

ListaEnlazadaOrdenada::ListaEnlazadaOrdenada() : cabeza(nullptr) {}

ListaEnlazadaOrdenada::~ListaEnlazadaOrdenada() {
    NodoOrdenado* actual = cabeza;
    while (actual != nullptr) {
        NodoOrdenado* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
    cabeza = nullptr;
}

void ListaEnlazadaOrdenada::insertarOrdenado(Producto* producto) {
    if (producto == nullptr) {
        return;
    }

    NodoOrdenado* nuevoNodo = new NodoOrdenado(producto);

    if (cabeza == nullptr ||
        (cabeza->producto != nullptr && producto->nombre < cabeza->producto->nombre)) {
        nuevoNodo->siguiente = cabeza;
        cabeza = nuevoNodo;
        return;
    }

    NodoOrdenado* actual = cabeza;
    while (actual->siguiente != nullptr &&
           actual->siguiente->producto != nullptr &&
           actual->siguiente->producto->nombre <= producto->nombre) {
        actual = actual->siguiente;
    }

    nuevoNodo->siguiente = actual->siguiente;
    actual->siguiente = nuevoNodo;
}

Producto* ListaEnlazadaOrdenada::buscarPorNombre(const std::string& nombre) const {
    NodoOrdenado* actual = cabeza;
    while (actual != nullptr) {
        if (actual->producto != nullptr) {
            if (actual->producto->nombre == nombre) {
                return actual->producto;
            }
            if (actual->producto->nombre > nombre) {
                return nullptr;
            }
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

// eliminar: Alias semántico de eliminación por nombre para mantener API uniforme.
// Complejidad: O(n), por recorrido lineal con corte temprano por orden.
bool ListaEnlazadaOrdenada::eliminar(const std::string& nombre) {
    return eliminarPorNombre(nombre);
}

bool ListaEnlazadaOrdenada::eliminarPorNombre(const std::string& nombre) {
    NodoOrdenado* actual = cabeza;
    NodoOrdenado* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->producto != nullptr && actual->producto->nombre == nombre) {
            if (anterior == nullptr) {
                cabeza = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            delete actual;
            return true;
        }

        if (actual->producto != nullptr && actual->producto->nombre > nombre) {
            return false;
        }

        anterior = actual;
        actual = actual->siguiente;
    }

    return false;
}

bool ListaEnlazadaOrdenada::eliminarPorCodigoBarras(const std::string& codigoBarras) {
    NodoOrdenado* actual = cabeza;
    NodoOrdenado* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->producto != nullptr && actual->producto->codigoBarras == codigoBarras) {
            if (anterior == nullptr) {
                cabeza = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            delete actual;
            return true;
        }

        anterior = actual;
        actual = actual->siguiente;
    }

    return false;
}

void ListaEnlazadaOrdenada::mostrarTodos() const {
    if (estaVacia()) {
        std::cout << "La lista ordenada esta vacia.\n";
        return;
    }

    NodoOrdenado* actual = cabeza;
    int indice = 1;
    while (actual != nullptr) {
        if (actual->producto != nullptr) {
            std::cout << "Producto " << indice << ":\n";
            std::cout << *(actual->producto) << "\n\n";
            ++indice;
        }
        actual = actual->siguiente;
    }
}

bool ListaEnlazadaOrdenada::estaVacia() const {
    return cabeza == nullptr;
}
