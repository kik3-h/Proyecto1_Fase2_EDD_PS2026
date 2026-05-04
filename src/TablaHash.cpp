#include "TablaHash.h"

NodoHash::NodoHash(Producto* producto) : producto(producto), siguiente(nullptr) {}
//en teoria funcionaria pero no lose pq ya no continue con su desarrollo jsjs
TablaHash::TablaHash(int capacidadInicial)
    : tabla(nullptr), capacidad(capacidadInicial > 0 ? capacidadInicial : 1009) {
    tabla = new NodoHash*[capacidad];
    for (int i = 0; i < capacidad; ++i) {
        tabla[i] = nullptr;
    }
}

TablaHash::~TablaHash() {
    if (tabla == nullptr) {
        return;
    }

    for (int i = 0; i < capacidad; ++i) {
        NodoHash* actual = tabla[i];
        while (actual != nullptr) {
            NodoHash* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
        tabla[i] = nullptr;
    }

    delete[] tabla;
    tabla = nullptr;
}

int TablaHash::funcionHash(const std::string& clave) const {
    // Hash polinomial base 31: combina orden y valor de caracteres con
    // buena distribución para claves tipo texto como codigo de barras.
    unsigned long acumulado = 0;
    for (char caracter : clave) {
        acumulado = (acumulado * 31UL + static_cast<unsigned long>(static_cast<unsigned char>(caracter))) %
                    static_cast<unsigned long>(capacidad);
    }
    return static_cast<int>(acumulado);
}

bool TablaHash::insertar(Producto* producto) {
    if (producto == nullptr) {
        return false;
    }

    const std::string& codigo = producto->codigoBarras;
    if (codigo.empty()) {
        return false;
    }

    if (buscarPorCodigoBarras(codigo) != nullptr) {
        return false;
    }

    int indice = funcionHash(codigo);
    NodoHash* nuevoNodo = new NodoHash(producto);
    nuevoNodo->siguiente = tabla[indice];
    tabla[indice] = nuevoNodo;
    return true;
}

Producto* TablaHash::buscarPorCodigoBarras(const std::string& codigo) const {
    if (codigo.empty() || tabla == nullptr) {
        return nullptr;
    }

    int indice = funcionHash(codigo);
    NodoHash* actual = tabla[indice];

    while (actual != nullptr) {
        if (actual->producto != nullptr && actual->producto->codigoBarras == codigo) {
            return actual->producto;
        }
        actual = actual->siguiente;
    }

    return nullptr;
}

// eliminar: Alias de borrado por código de barras.
// Complejidad promedio: O(1), peor caso O(n) por colisiones.
bool TablaHash::eliminar(const std::string& codigo) {
    return eliminarPorCodigoBarras(codigo);
}

bool TablaHash::eliminarPorCodigoBarras(const std::string& codigo) {
    if (codigo.empty() || tabla == nullptr) {
        return false;
    }

    int indice = funcionHash(codigo);
    NodoHash* actual = tabla[indice];
    NodoHash* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->producto != nullptr && actual->producto->codigoBarras == codigo) {
            if (anterior == nullptr) {
                tabla[indice] = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            delete actual;
            return true;
        }

        anterior = actual;
        actual = actual->siguiente;
    }

    return false;
}
