#include "Cola.h"

// Implementación de cola manual FIFO con nodos encadenados.
// Esta estructura administra nodos propios y no destruye los Producto* almacenados.

Cola::NodoCola::NodoCola(Producto* producto) : dato(producto), siguiente(nullptr) {}

// Construye una cola vacía. Complejidad O(1).
Cola::Cola() : frente(nullptr), final(nullptr) {}

// Libera todos los nodos de la cola sin liberar los productos apuntados. Complejidad O(n).
Cola::~Cola() {
    while (frente != nullptr) {
        NodoCola* temporal = frente;
        frente = frente->siguiente;
        delete temporal;
    }
    final = nullptr;
}

// Encola un producto. O(1).
void Cola::encolar(Producto* p) {
    if (p == nullptr) {
        return;
    }

    NodoCola* nuevo = new NodoCola(p);

    if (final == nullptr) {
        frente = nuevo;
        final = nuevo;
        return;
    }

    final->siguiente = nuevo;
    final = nuevo;
}

// Desencola el producto del frente y lo retorna. O(1).
Producto* Cola::desencolar() {
    if (frente == nullptr) {
        return nullptr;
    }

    NodoCola* temporal = frente;
    Producto* resultado = temporal->dato;
    frente = frente->siguiente;
    if (frente == nullptr) {
        final = nullptr;
    }
    delete temporal;
    return resultado;
}

// Retorna el frente sin removerlo. O(1).
Producto* Cola::verFrente() const {
    if (frente == nullptr) {
        return nullptr;
    }
    return frente->dato;
}

// Indica si la cola está vacía. O(1).
bool Cola::vacia() const {
    return frente == nullptr;
}
