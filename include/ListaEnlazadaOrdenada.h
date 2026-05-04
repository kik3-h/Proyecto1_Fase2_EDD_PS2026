#ifndef LISTA_ENLAZADA_ORDENADA_H
#define LISTA_ENLAZADA_ORDENADA_H

#include <string>

#include "Producto.h"
//esta clase realiza la gestión de una lista enlazada ordenada de productos
class NodoOrdenado {
public:
    Producto* producto;
    NodoOrdenado* siguiente;

    explicit NodoOrdenado(Producto* producto);
};

class ListaEnlazadaOrdenada {
private:
    NodoOrdenado* cabeza;

public:
    ListaEnlazadaOrdenada();
    ~ListaEnlazadaOrdenada();

    void insertarOrdenado(Producto* producto);
    Producto* buscarPorNombre(const std::string& nombre) const;
    bool eliminar(const std::string& nombre);
    bool eliminarPorNombre(const std::string& nombre);
    bool eliminarPorCodigoBarras(const std::string& codigoBarras);
    void mostrarTodos() const;
    bool estaVacia() const;
};

#endif
