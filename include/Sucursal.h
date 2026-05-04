#ifndef SUCURSAL_H
#define SUCURSAL_H

// Sucursal: Encapsula inventario local, colas de flujo y pila de rollback.
// Implementación manual con punteros crudos, sin contenedores STL de negocio.

#include <string>
#include <vector>

#include "ArbolAVL.h"
#include "ArbolB.h"
#include "ArbolBPlus.h"
#include "Cola.h"
#include "ListaEnlazada.h"
#include "ListaEnlazadaOrdenada.h"
#include "Pila.h"
#include "TablaHash.h"

class Sucursal {
private: //instancio las constantes para cada sucursal, como su id, nombre, ubicación, tiempos de ingreso, preparación y despacho, así como las estructuras de datos para el inventario y las colas de flujo.
    int id;
    std::string nombre;
    std::string ubicacion;
    int tiempoIngreso;
    int tiempoPreparacion;
    int intervaloDespacho;

    ListaEnlazada* listaGeneral;
    ListaEnlazadaOrdenada* listaOrdenada;
    ArbolAVL* arbolAVL;
    ArbolB* arbolB;
    ArbolBPlus* arbolBPlus;
    TablaHash* tablaHash;

    Cola* colaIngreso;
    Cola* colaPreparacion;
    Cola* colaSalida;
    Pila* pilaRollback;

public:
    // Construye una sucursal con estructuras locales propias. Complejidad O(1).
    Sucursal(int id,
             const std::string& nombre,
             const std::string& ubicacion,
             int tiempoIngreso,
             int tiempoPreparacion,
             int intervaloDespacho);

    // Libera estructuras auxiliares de la sucursal, no los Producto*. Complejidad O(n).
    ~Sucursal();

    // Inserta el producto en todos los índices de inventario con rollback local si falla.
    // Complejidad esperada: O(log n) promedio por estructuras balanceadas.
    bool agregarProducto(Producto* p);

    // Elimina el producto del inventario local de forma transaccional por código.
    // Complejidad esperada: O(log n) promedio.
    bool eliminarProducto(const std::string& codigoBarra);

    // Busca por código de barras usando hash local. Complejidad O(1) promedio.
    Producto* buscarPorCodigo(const std::string& codigo);

    // Busca por nombre usando AVL local. Complejidad O(log n).
    Producto* buscarPorNombre(const std::string& nombre);

    // Busca por categoría usando B+ local. Complejidad O(log n + k).
    std::vector<Producto*> buscarPorCategoria(const std::string& categoria);

    // Busca por rango de fechas usando B local. Complejidad O(log n + k).
    std::vector<Producto*> buscarPorRangoFecha(const std::string& inicio, const std::string& fin);

    // Encola en cola de ingreso. Complejidad O(1).
    void encolarIngreso(Producto* p);

    // Encola en cola de preparación. Complejidad O(1).
    void encolarPreparacion(Producto* p);

    // Encola en cola de salida. Complejidad O(1).
    void encolarSalida(Producto* p);

    // Desencola desde cola de ingreso. Complejidad O(1).
    Producto* desencolarIngreso();

    // Desencola desde cola de preparación. Complejidad O(1).
    Producto* desencolarPreparacion();

    // Desencola desde cola de salida. Complejidad O(1).
    Producto* desencolarSalida();

    // Apila producto para rollback. Complejidad O(1).
    void pushRollback(Producto* p);

    // Desapila producto de rollback. Complejidad O(1).
    Producto* popRollback();

    // Getters de metadatos de sucursal. Complejidad O(1).
    int obtenerId() const;
    const std::string& obtenerNombre() const;
    const std::string& obtenerUbicacion() const;
    int obtenerTiempoIngreso() const;
    int obtenerTiempoPreparacion() const;
    int obtenerIntervaloDespacho() const;

    // Getters de estructuras internas para UI y reportes. Complejidad O(1).
    ListaEnlazada* obtenerListaGeneral() const;
    ListaEnlazadaOrdenada* obtenerListaOrdenada() const;
    ArbolAVL* obtenerArbolAVL() const;
    ArbolB* obtenerArbolB() const;
    ArbolBPlus* obtenerArbolBPlus() const;
    TablaHash* obtenerTablaHash() const;
    Cola* obtenerColaIngreso() const;
    Cola* obtenerColaPreparacion() const;
    Cola* obtenerColaSalida() const;
    Pila* obtenerPilaRollback() const;
};

#endif
