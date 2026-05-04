#include "Producto.h"
//esta clase hace representacion de un producto en el catálogo 
Producto::Producto(const std::string& nombre,
                   const std::string& codigoBarras,
                   const std::string& categoria,
                   const std::string& fechaCaducidad,
                   const std::string& marca,
                   double precio,
                   int stock)
    : nombre(nombre),
      codigoBarras(codigoBarras),
      categoria(categoria),
      fechaCaducidad(fechaCaducidad),
      marca(marca),
      estado("Disponible"),
      precio(precio),
      stock(stock) {}

const std::string& Producto::obtenerEstado() const {
    return estado;
}

void Producto::establecerEstado(const std::string& nuevoEstado) {
    estado = nuevoEstado;
}

std::ostream& operator<<(std::ostream& salida, const Producto& producto) {
    salida << "Nombre: " << producto.nombre << '\n'
           << "Codigo de barras: " << producto.codigoBarras << '\n'
           << "Categoria: " << producto.categoria << '\n'
           << "Fecha de caducidad: " << producto.fechaCaducidad << '\n'
           << "Marca: " << producto.marca << '\n'
           << "Precio: Q" << producto.precio << '\n'
           << "Stock: " << producto.stock;
    return salida;
}
