#ifndef ARBOL_AVL_H
#define ARBOL_AVL_H

#include <fstream>
#include <string>

#include "Producto.h"


//esta clase representa un nodo del arbol AVL, cada nodo contiene un producto y punteros a los nodos izquierdo y derecho, ademas de la altura del nodo para mantener el equilibrio del arbol
class NodoAVL {
public:
    Producto* producto;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura;

    explicit NodoAVL(Producto* producto);
};

class ArbolAVL {
private:
    NodoAVL* raiz;

    int obtenerAltura(NodoAVL* nodo) const;
    int obtenerFactorEquilibrio(NodoAVL* nodo) const;
    int maximo(int a, int b) const;

    NodoAVL* rotacionDerecha(NodoAVL* nodoDesbalanceado);
    NodoAVL* rotacionIzquierda(NodoAVL* nodoDesbalanceado);

    NodoAVL* insertarRecursivo(NodoAVL* nodo, Producto* producto);
    NodoAVL* eliminarRecursivo(NodoAVL* nodo, const std::string& nombre);
    NodoAVL* obtenerNodoMinimo(NodoAVL* nodo) const;
    Producto* buscarRecursivo(NodoAVL* nodo, const std::string& nombre) const;

    void recorridoInOrdenRecursivo(NodoAVL* nodo) const;
    void destruirRecursivo(NodoAVL* nodo);

    void generarDotRecursivo(NodoAVL* nodo, std::ofstream& archivo, int& contador) const;
    std::string escaparTexto(const std::string& texto) const;

public:
    ArbolAVL();
    ~ArbolAVL();

    void insertar(Producto* producto);
    Producto* buscarPorNombre(const std::string& nombre) const;
    bool eliminar(const std::string& nombre);
    bool eliminarPorNombre(const std::string& nombre);
    bool eliminarPorCodigoBarras(const std::string& codigoBarras);

    void recorridoInOrden() const;
    bool estaVacio() const;

    void generarDot(const std::string& rutaArchivo) const;
    void generarImagen() const;
};

#endif
