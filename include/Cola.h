#ifndef COLA_H
#define COLA_H

// Cola: Implementación manual tipo FIFO con nodos enlazados.
// No utiliza contenedores STL y maneja memoria con new/delete.

#include "Producto.h"

class Cola {
public:
    struct NodoCola {
        Producto* dato;
        NodoCola* siguiente;

        explicit NodoCola(Producto* producto);
    };

private:
    NodoCola* frente;
    NodoCola* final;

public:
    // Construye una cola vacía. Complejidad O(1).
    Cola();

    // Libera todos los nodos de la cola (no libera Producto*). Complejidad O(n).
    ~Cola();

    // Encola un producto. O(1).
    void encolar(Producto* p);

    // Desencola el producto del frente y lo retorna. O(1).
    Producto* desencolar();

    // Retorna el frente sin removerlo. O(1).
    Producto* verFrente() const;

    // Retorna el nodo frontal para iteracion sin modificar la cola. O(1).
    NodoCola* obtenerFrente() const;

    // Indica si la cola está vacía. O(1).
    bool vacia() const;
};

#endif
