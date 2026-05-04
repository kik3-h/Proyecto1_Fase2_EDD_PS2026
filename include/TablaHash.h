#ifndef TABLA_HASH_H
#define TABLA_HASH_H

#include <string>

#include "Producto.h"
//intente implementar esto pero me dijeron que no se pedia entonces lo deje
//y no lo borre pq si funciona algo no lo toques jajaja
class NodoHash {
public:
    Producto* producto;
    NodoHash* siguiente;

    explicit NodoHash(Producto* producto);
};

class TablaHash {
private:
    NodoHash** tabla;
    int capacidad;

public:
    explicit TablaHash(int capacidadInicial = 1009);
    ~TablaHash();

    int funcionHash(const std::string& clave) const;
    bool insertar(Producto* producto);
    Producto* buscarPorCodigoBarras(const std::string& codigo) const;
    bool eliminar(const std::string& codigo);
    bool eliminarPorCodigoBarras(const std::string& codigo);
};

#endif
