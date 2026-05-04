#ifndef LISTA_ENLAZADA_H
#define LISTA_ENLAZADA_H

#include <string>
#include <vector>

#include "Producto.h"

class Nodo {
public:
    Producto* producto;
    Nodo* siguiente;

    explicit Nodo(Producto* producto);
};

class ListaEnlazada {
private:
    Nodo* cabeza;

public:
    ListaEnlazada();
    ~ListaEnlazada();

    void insertar(Producto* producto);
    Producto* buscarPorNombre(const std::string& nombre) const;
    Producto* buscarPorCodigoBarras(const std::string& codigo) const;
    bool eliminar(Producto* producto);
    bool eliminarPorCodigoBarras(const std::string& codigo);
    int obtenerPrimerosProductos(Producto** productos, int cantidadMaxima) const;
    void mostrarTodos() const;
    bool estaVacia() const;
    
    // Método para obtener todos los productos como vector (buffer temporal para UI)
    std::vector<Producto*> obtenerTodos() const;
};

#endif
