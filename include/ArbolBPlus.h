#ifndef ARBOL_BPLUS_H
#define ARBOL_BPLUS_H

#include <fstream>
#include <string>
#include <vector>

#include "ListaEnlazada.h"
#include "Producto.h"

// Estructura para manejar claves con productos de la misma categoría
// Diseño: Una única clave por categoría, con ListaEnlazada* para agrupar duplicados
struct ClaveCategoria {
    std::string categoria;
    ListaEnlazada* productos;  // Solo se usa en hojas agrupa productos de misma categoría

    ClaveCategoria();
    ~ClaveCategoria();
};

// Nodo del Árbol B+
class NodoBPlus {
public:
    ClaveCategoria* claves;
    NodoBPlus** hijos;
    int numClaves;
    bool esHoja;
    int gradoMinimo;
    NodoBPlus* siguiente;  // Enlace secuencial entre hojas (para recorrido O(log N + K))

    explicit NodoBPlus(int t, bool hoja);
    ~NodoBPlus();
};

// Árbol B+ con grado mínimo t = 3
class ArbolBPlus {
private:
    NodoBPlus* raiz;
    int gradoMinimo;

    void insertarEnNodoNoLleno(NodoBPlus* nodo, Producto* producto);
    void dividirHijo(NodoBPlus* padre, int indice, NodoBPlus* hijo);

    NodoBPlus* buscarHoja(NodoBPlus* nodo, const std::string& categoria) const;
    ClaveCategoria* buscarClaveEnHoja(NodoBPlus* hoja, const std::string& categoria) const;
    
    // Búsqueda global en todas las hojas para evitar duplicados
    ClaveCategoria* buscarClaveGlobal(const std::string& categoria) const;
    void recolectarProductosDesdeHojas(std::vector<Producto*>& resultado) const;
    bool reconstruirDesdeProductos(const std::vector<Producto*>& productos);

    void destruirRecursivo(NodoBPlus* nodo);

    // Visualización con enlaces explícitos entre hojas
    void generarDotRecursivo(NodoBPlus* nodo, std::ofstream& archivo, int& contadorNodo) const;
    void generarEnlacesHojas(std::ofstream& archivo) const;
    std::string escaparTexto(const std::string& texto) const;
    
    // Obtener la primera hoja del árbol
    NodoBPlus* obtenerPrimeraHoja() const;

public:
    ArbolBPlus(int t = 3);
    ~ArbolBPlus();

    void insertar(Producto* producto);
    bool eliminar(const std::string& categoria, const std::string& codigoBarras);
    void buscarPorCategoria(const std::string& categoria) const;
    
    // Retorna productos de una categoría (para GUI)
    std::vector<Producto*> obtenerPorCategoria(const std::string& categoria) const;

    void generarDot(const std::string& rutaArchivo) const;
    void generarImagen() const;

    bool estaVacio() const;
};

#endif
