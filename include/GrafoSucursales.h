#ifndef GRAFO_SUCURSALES_H
#define GRAFO_SUCURSALES_H

#include "MinHeap.h"

// GrafoSucursales: Grafo ponderado con lista de adyacencia manual.
// Cada índice del arreglo representa el id de una sucursal.
class GrafoSucursales {
public:
    struct NodoArista {
        int destinoId;
        int tiempo;
        int costo;
        NodoArista* siguiente;
    };

    typedef NodoArista Arista;

    struct Ruta {
        int costoTotal;
        int* nodosRuta;
        int longitudRuta;
    };

private:
    struct ListaAdyacencia {
        NodoArista* cabeza;

        // Construye una lista de adyacencia vacía. Complejidad O(1).
        ListaAdyacencia();

        // Libera todos los nodos de arista de la lista. Complejidad O(k).
        ~ListaAdyacencia();

        // Inserta arista al inicio de la lista de adyacencia. Complejidad O(1).
        void insertarArista(int destinoId, int tiempo, int costo);
    };

    int maxNodos;
    int numNodos;
    ListaAdyacencia** adyacencia;

    // Redimensiona el arreglo para cubrir un id requerido. Complejidad O(V).
    void redimensionarParaId(int idRequerido);

    // Verifica si existe una sucursal cargada en el grafo para el id. O(1).
    bool nodoValido(int id) const;

public:
    // Inicializa el grafo con capacidad base para ids de sucursal. O(V).
    explicit GrafoSucursales(int capacidadInicial = 16);

    // Libera todas las listas de adyacencia y el arreglo principal. O(V + E).
    ~GrafoSucursales();

    // Registra una sucursal por id; crea su lista vacía si no existe. O(1) amortizado.
    void agregarSucursal(int id);

    // Agrega conexión ponderada entre dos sucursales. O(1).
    void agregarConexion(int origen, int destino, int tiempo, int costo, bool bidireccional = true);

    // Calcula ruta óptima con Dijkstra por tiempo o costo. O((V + E) log V).
    // El arreglo nodosRuta retornado debe liberarse con delete[] por quien consume la ruta.
    Ruta rutaMasCorta(int origen, int destino, bool porTiempo) const;

    // Retorna el primer nodo de arista para iterar vecinos de un id. O(1).
    Arista* obtenerAristas(int nodoId) const;
};

#endif
