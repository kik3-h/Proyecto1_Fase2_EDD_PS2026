#ifndef PILA_H
#define PILA_H

// Pila: Implementación manual tipo LIFO con nodos enlazados.
// No utiliza contenedores STL y maneja memoria con new/delete.

#include "Producto.h"

class Pila {
private:
    struct NodoPila {
        Producto* dato;
        NodoPila* siguiente;

        explicit NodoPila(Producto* producto);
    };

    NodoPila* cima;

public:
    // Construye una pila vacía. Complejidad O(1).
    Pila();

    // Libera todos los nodos de la pila (no libera Producto*). Complejidad O(n).
    ~Pila();

    // Apila un producto. Complejidad O(1).
    void push(Producto* p);

    // Desapila el producto superior. O(1).
    Producto* pop();

    // Retorna el producto superior sin removerlo. Complejidad O(1).
    Producto* top() const;

    // Indica si la pila está vacía. Complejidad O(1).
    bool vacia() const;
};

#endif
