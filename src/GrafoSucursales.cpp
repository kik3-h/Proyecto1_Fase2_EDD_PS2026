#include "GrafoSucursales.h"

#include <climits> //esta librería se incluye para usar INT_MAX y evitar problemas de overflow al sumar pesos en Dijkstra.

GrafoSucursales::ListaAdyacencia::ListaAdyacencia() : cabeza(nullptr) {} // Constructor que inicializa la lista de adyacencia vacía.

GrafoSucursales::ListaAdyacencia::~ListaAdyacencia() {
    NodoArista* actual = cabeza; // Se recorre la lista de adyacencia y se liberan todos los nodos de arista.
    while (actual != nullptr) { // Complejidad O(k), donde k es el número de aristas en la lista.
        NodoArista* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
    cabeza = nullptr; // Se asegura que la cabeza quede en nullptr después de liberar los nodos.
}

void GrafoSucursales::ListaAdyacencia::insertarArista(int destinoId, int tiempo, int costo) { // Inserta una nueva arista al inicio de la lista de adyacencia. Complejidad O(1).
    NodoArista* nuevaArista = new NodoArista();
    nuevaArista->destinoId = destinoId; // Se asignan los valores de destinoId, tiempo y costo a la nueva arista.
    nuevaArista->tiempo = tiempo;
    nuevaArista->costo = costo;
    nuevaArista->siguiente = cabeza; // La nueva arista apunta a la antigua cabeza de la lista, y luego se actualiza la cabeza para que apunte a la nueva arista.
    cabeza = nuevaArista; // La nueva arista se convierte en la nueva cabeza de la lista de adyacencia.
}

GrafoSucursales::GrafoSucursales(int capacidadInicial) // Constructor que inicializa el grafo con una capacidad base para los ids de sucursal. Complejidad O(V), donde V es la capacidad inicial.
    : maxNodos(capacidadInicial > 0 ? capacidadInicial : 16), // Se establece la capacidad máxima de nodos, asegurando que sea al menos 16. 
      numNodos(0),
      adyacencia(new ListaAdyacencia*[maxNodos]) { // Se crea un arreglo de punteros a ListaAdyacencia con la capacidad máxima definida.
    for (int i = 0; i < maxNodos; ++i) { // Se inicializan todos los punteros del arreglo de adyacencia a nullptr, indicando que no hay sucursales registradas inicialmente.
        adyacencia[i] = nullptr;
    }
}

GrafoSucursales::~GrafoSucursales() { // Destructor que libera todas las listas de adyacencia y el arreglo principal. Complejidad O(V + E), donde V es el número de nodos y E es el número total de aristas en el grafo.
    for (int i = 0; i < maxNodos; ++i) {
        delete adyacencia[i];
        adyacencia[i] = nullptr;
    }

    delete[] adyacencia;
    adyacencia = nullptr;
    maxNodos = 0;
    numNodos = 0;
}

void GrafoSucursales::redimensionarParaId(int idRequerido) { // Redimensiona el arreglo de adyacencia para cubrir un id requerido. Complejidad O(V), donde V es la capacidad actual del arreglo.
    if (idRequerido < maxNodos) {
        return;
    }

    int nuevaCapacidad = maxNodos;
    while (nuevaCapacidad <= idRequerido) {
        nuevaCapacidad *= 2;
    }

    ListaAdyacencia** nuevoArreglo = new ListaAdyacencia*[nuevaCapacidad]; // Se crea un nuevo arreglo de punteros a ListaAdyacencia con la nueva capacidad.
    for (int i = 0; i < nuevaCapacidad; ++i) {
        nuevoArreglo[i] = nullptr;
    }

    for (int i = 0; i < maxNodos; ++i) {
        nuevoArreglo[i] = adyacencia[i];
    }

    delete[] adyacencia;
    adyacencia = nuevoArreglo;
    maxNodos = nuevaCapacidad;
}

bool GrafoSucursales::nodoValido(int id) const {
    return id >= 0 && id < maxNodos && adyacencia[id] != nullptr;
}

void GrafoSucursales::agregarSucursal(int id) {
    if (id < 0) {
        return;
    }

    redimensionarParaId(id);

    if (adyacencia[id] == nullptr) {
        adyacencia[id] = new ListaAdyacencia();
        ++numNodos;
    }
}

void GrafoSucursales::agregarConexion(int origen,
                                      int destino,
                                      int tiempo,
                                      int costo,
                                      bool bidireccional) {
    if (origen < 0 || destino < 0 || tiempo < 0 || costo < 0) {
        return;
    }

    agregarSucursal(origen);
    agregarSucursal(destino);

    adyacencia[origen]->insertarArista(destino, tiempo, costo);
    if (bidireccional && origen != destino) {
        adyacencia[destino]->insertarArista(origen, tiempo, costo);
    }
}

GrafoSucursales::Ruta GrafoSucursales::rutaMasCorta(int origen, int destino, bool porTiempo) const { // Calcula la ruta óptima entre dos sucursales utilizando el algoritmo de Dijkstra. Complejidad O((V + E) log V), donde V es el número de nodos y E es el número de aristas en el grafo.
    Ruta resultado;
    resultado.costoTotal = -1;
    resultado.nodosRuta = nullptr;
    resultado.longitudRuta = 0;

    if (!nodoValido(origen) || !nodoValido(destino)) {
        return resultado;
    }

    int* distancias = new int[maxNodos];
    int* predecesores = new int[maxNodos];

    for (int i = 0; i < maxNodos; ++i) {
        distancias[i] = INT_MAX;
        predecesores[i] = -1;
    }

    MinHeap heap;
    distancias[origen] = 0;
    heap.insertar(origen, 0);

    while (!heap.vacia()) { // Se extrae el nodo con la menor distancia acumulada desde el origen. Complejidad O(log V).
        MinHeap::NodoHeap extraido = heap.extraerMinimo();
        const int nodoActual = extraido.idSucursal;

        if (nodoActual < 0 || nodoActual >= maxNodos) {
            continue;
        }

        if (extraido.distancia != distancias[nodoActual]) {
            continue;
        }

        if (nodoActual == destino) {
            break;
        }

        NodoArista* arista = adyacencia[nodoActual]->cabeza; // Se recorren las aristas adyacentes al nodo actual. Complejidad O(k), donde k es el número de aristas adyacentes.
        while (arista != nullptr) {
            const int vecino = arista->destinoId;
            const int peso = porTiempo ? arista->tiempo : arista->costo;

            if (vecino >= 0 &&
                vecino < maxNodos &&
                peso >= 0 &&
                distancias[nodoActual] != INT_MAX &&
                distancias[nodoActual] <= INT_MAX - peso) {
                const int nuevaDistancia = distancias[nodoActual] + peso;
                if (nuevaDistancia < distancias[vecino]) {
                    distancias[vecino] = nuevaDistancia;
                    predecesores[vecino] = nodoActual;
                    heap.insertar(vecino, nuevaDistancia);
                }
            }

            arista = arista->siguiente;
        }
    }

    if (distancias[destino] == INT_MAX) {
        delete[] distancias;
        delete[] predecesores;
        return resultado;
    }

    int longitudRuta = 1;
    int cursor = destino;
    while (cursor != origen) {
        cursor = predecesores[cursor];
        if (cursor == -1) {
            delete[] distancias;
            delete[] predecesores;
            return resultado;
        }
        ++longitudRuta;
    }

    int* rutaInversa = new int[longitudRuta]; // Se construye la ruta desde el destino hasta el origen utilizando el arreglo de predecesores. Complejidad O(longitudRuta).
    cursor = destino;
    for (int i = 0; i < longitudRuta; ++i) { // Se llena el arreglo rutaInversa con los nodos de la ruta en orden inverso.
        rutaInversa[i] = cursor;
        cursor = predecesores[cursor];
    }

    resultado.nodosRuta = new int[longitudRuta]; // Se invierte el orden de los nodos para que la ruta esté en el orden correcto desde el origen hasta el destino. Complejidad O(longitudRuta).
    for (int i = 0; i < longitudRuta; ++i) {
        resultado.nodosRuta[i] = rutaInversa[longitudRuta - 1 - i];
    }
    resultado.longitudRuta = longitudRuta;
    resultado.costoTotal = distancias[destino]; // El costo total de la ruta es la distancia acumulada desde el origen hasta el destino.

    delete[] rutaInversa;
    delete[] distancias;
    delete[] predecesores;

    return resultado;
}

GrafoSucursales::Arista* GrafoSucursales::obtenerAristas(int nodoId) const {
    if (!nodoValido(nodoId)) {
        return nullptr;
    }

    return adyacencia[nodoId]->cabeza;
}
