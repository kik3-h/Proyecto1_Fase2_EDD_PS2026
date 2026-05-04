#ifndef MIN_HEAP_H
#define MIN_HEAP_H

// MinHeap: Cola de prioridad mínima basada en montículo binario.
// Implementación manual sobre arreglo dinámico con redimensionamiento explícito.
class MinHeap {
public:
    struct NodoHeap {
        int idSucursal;
        int distancia;
    };

private:
    NodoHeap* arreglo;
    int capacidad;
    int tamano;

    // Duplica la capacidad del arreglo interno. Complejidad O(n).
    void redimensionar();

    // Reestablece la propiedad de heap hacia arriba desde un índice dado. O(log n).
    void flotar(int indice);

    // Reestablece la propiedad de heap hacia abajo desde un índice dado. O(log n).
    void hundir(int indice);

    // Intercambia dos nodos del arreglo. Complejidad O(1).
    void intercambiar(NodoHeap& a, NodoHeap& b);

public:
    // Inicializa heap vacío con capacidad inicial fija. Complejidad O(1).
    MinHeap();

    // Libera el arreglo interno del heap. Complejidad O(1).
    ~MinHeap();

    // Inserta un par (idSucursal, distancia) y aplica heapify-up. O(log n).
    void insertar(int idSucursal, int distancia);

    // Extrae el nodo con menor distancia y aplica heapify-down. O(log n).
    // Si el heap está vacío retorna { -1, INT_MAX }.
    NodoHeap extraerMinimo();

    // Indica si el heap está vacío. Complejidad O(1).
    bool vacia() const;
};

#endif
