#include "Pila.h"

// Implementación de pila manual LIFO con nodos encadenados.
// Esta estructura administra nodos propios y no destruye los Producto* almacenados.

Pila::NodoPila::NodoPila(Producto* producto) : dato(producto), siguiente(nullptr) {}

// Construye una pila vacía. Complejidad O(1).
Pila::Pila() : cima(nullptr) {}

// Libera todos los nodos de la pila sin liberar los productos apuntados. Complejidad O(n).
Pila::~Pila() {
    while (cima != nullptr) {
        NodoPila* temporal = cima;
        cima = cima->siguiente;
        delete temporal;
    }
}

// Apila un producto. Complejidad O(1).
void Pila::push(Producto* p) {
    if (p == nullptr) {
        return;
    }

    NodoPila* nuevo = new NodoPila(p);
    nuevo->siguiente = cima;
    cima = nuevo;
}

// Desapila el producto superior. O(1).
Producto* Pila::pop() {
    if (cima == nullptr) {
        return nullptr;
    }

    NodoPila* temporal = cima;
    Producto* resultado = temporal->dato;
    cima = cima->siguiente;
    delete temporal;
    return resultado;
}

// Retorna el producto superior sin removerlo. Complejidad O(1).
Producto* Pila::top() const {
    if (cima == nullptr) {
        return nullptr;
    }
    return cima->dato;
}

// Indica si la pila está vacía. Complejidad O(1).
bool Pila::vacia() const {
    return cima == nullptr;
}
