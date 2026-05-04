#ifndef PRODUCTO_H
#define PRODUCTO_H

#include <iostream>
#include <string>

class Producto {
public:
    std::string nombre;
    std::string codigoBarras;
    std::string categoria;
    std::string fechaCaducidad;
    std::string marca;
    std::string estado;
    double precio;
    int stock;

    Producto(const std::string& nombre,
             const std::string& codigoBarras,
             const std::string& categoria,
             const std::string& fechaCaducidad,
             const std::string& marca,
             double precio,
             int stock);

    // Retorna el estado logístico actual del producto. Complejidad O(1).
    const std::string& obtenerEstado() const;

    // Actualiza el estado logístico del producto. Complejidad O(1).
    void establecerEstado(const std::string& nuevoEstado);
};

std::ostream& operator<<(std::ostream& salida, const Producto& producto);

#endif
