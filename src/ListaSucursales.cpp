#include "ListaSucursales.h"

// Lista enlazada simple para sucursales con inserción O(1) al final.
// La propiedad de las sucursales se administra externamente.

ListaSucursales::NodoSucursal::NodoSucursal(Sucursal* sucursal)
    : dato(sucursal), siguiente(nullptr) {}

ListaSucursales::ListaSucursales() : primero(nullptr), ultimo(nullptr) {}

ListaSucursales::~ListaSucursales() {
    NodoSucursal* actual = primero;
    while (actual != nullptr) {
        NodoSucursal* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
    primero = nullptr;
    ultimo = nullptr;
}

void ListaSucursales::agregar(Sucursal* s) {
    if (s == nullptr) {
        return;
    }

    NodoSucursal* nuevo = new NodoSucursal(s);
    if (primero == nullptr) {
        primero = nuevo;
        ultimo = nuevo;
        return;
    }

    ultimo->siguiente = nuevo;
    ultimo = nuevo;
}

bool ListaSucursales::eliminar(int id) {
    NodoSucursal* actual = primero;
    NodoSucursal* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->dato != nullptr && actual->dato->obtenerId() == id) {
            if (anterior == nullptr) {
                primero = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }

            if (actual == ultimo) {
                ultimo = anterior;
            }

            delete actual;
            return true;
        }

        anterior = actual;
        actual = actual->siguiente;
    }

    return false;
}

Sucursal* ListaSucursales::buscar(int id) const {
    NodoSucursal* actual = primero;
    while (actual != nullptr) {
        if (actual->dato != nullptr && actual->dato->obtenerId() == id) {
            return actual->dato;
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

ListaSucursales::NodoSucursal* ListaSucursales::obtenerPrimero() const {
    return primero;
}

bool ListaSucursales::vacia() const {
    return primero == nullptr;
}
