#ifndef LISTA_SUCURSALES_H
#define LISTA_SUCURSALES_H

// ListaSucursales: Lista enlazada manual para gestionar sucursales sin STL.
// Mantiene puntero a cabeza y cola para inserción eficiente al final.

#include "Sucursal.h"

class ListaSucursales {
public:
    struct NodoSucursal {
        Sucursal* dato;
        NodoSucursal* siguiente;

        explicit NodoSucursal(Sucursal* sucursal);
    };

private:
    NodoSucursal* primero;
    NodoSucursal* ultimo;

public:
    // Construye una lista vacía. Complejidad O(1).
    ListaSucursales();

    // Libera solo nodos de la lista, no las sucursales. Complejidad O(n).
    ~ListaSucursales();

    // Agrega sucursal al final usando puntero a cola. Complejidad O(1).
    void agregar(Sucursal* s);

    // Elimina nodo por id de sucursal; no elimina Sucursal*. Complejidad O(n).
    bool eliminar(int id);

    // Busca sucursal por id y retorna su puntero. Complejidad O(n).
    Sucursal* buscar(int id) const;

    // Retorna el primer nodo para iteración manual. Complejidad O(1).
    NodoSucursal* obtenerPrimero() const;

    // Indica si la lista está vacía. Complejidad O(1).
    bool vacia() const;
};

#endif
