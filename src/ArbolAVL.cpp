#include "ArbolAVL.h"

#include <cstdlib> //esta libreria se encarga de manejar la asignación y liberación de memoria
#include <iostream>

//esta clase realiza la gestión de un árbol AVL 
NodoAVL::NodoAVL(Producto* producto)
    : producto(producto), izquierdo(nullptr), derecho(nullptr), altura(1) {}

ArbolAVL::ArbolAVL() : raiz(nullptr) {}

ArbolAVL::~ArbolAVL() {
    destruirRecursivo(raiz);
    raiz = nullptr;
}

void ArbolAVL::destruirRecursivo(NodoAVL* nodo) {
    if (nodo == nullptr) {
        return;
    }
    destruirRecursivo(nodo->izquierdo);
    destruirRecursivo(nodo->derecho);
    delete nodo;
}

int ArbolAVL::obtenerAltura(NodoAVL* nodo) const {
    if (nodo == nullptr) {
        return 0;
    }
    return nodo->altura;
}

int ArbolAVL::obtenerFactorEquilibrio(NodoAVL* nodo) const {
    if (nodo == nullptr) {
        return 0;
    }
    return obtenerAltura(nodo->izquierdo) - obtenerAltura(nodo->derecho);
}

int ArbolAVL::maximo(int a, int b) const {
    return (a > b) ? a : b;
}

NodoAVL* ArbolAVL::rotacionDerecha(NodoAVL* nodoDesbalanceado) {
    // Rotación simple derecha (caso LL)
    NodoAVL* nuevaRaiz = nodoDesbalanceado->izquierdo;
    NodoAVL* subArbolDerecho = nuevaRaiz->derecho;

    nuevaRaiz->derecho = nodoDesbalanceado;
    nodoDesbalanceado->izquierdo = subArbolDerecho;

    nodoDesbalanceado->altura = maximo(obtenerAltura(nodoDesbalanceado->izquierdo),
                                       obtenerAltura(nodoDesbalanceado->derecho)) + 1;
    nuevaRaiz->altura = maximo(obtenerAltura(nuevaRaiz->izquierdo),
                               obtenerAltura(nuevaRaiz->derecho)) + 1;

    return nuevaRaiz;
}

NodoAVL* ArbolAVL::rotacionIzquierda(NodoAVL* nodoDesbalanceado) {
    // Rotación simple izquierda (caso RR)
    NodoAVL* nuevaRaiz = nodoDesbalanceado->derecho;
    NodoAVL* subArbolIzquierdo = nuevaRaiz->izquierdo;

    nuevaRaiz->izquierdo = nodoDesbalanceado;
    nodoDesbalanceado->derecho = subArbolIzquierdo;

    nodoDesbalanceado->altura = maximo(obtenerAltura(nodoDesbalanceado->izquierdo),
                                       obtenerAltura(nodoDesbalanceado->derecho)) + 1;
    nuevaRaiz->altura = maximo(obtenerAltura(nuevaRaiz->izquierdo),
                               obtenerAltura(nuevaRaiz->derecho)) + 1;

    return nuevaRaiz;
}

NodoAVL* ArbolAVL::insertarRecursivo(NodoAVL* nodo, Producto* producto) {
    if (nodo == nullptr) {
        return new NodoAVL(producto);
    }

    // Comparación alfabética por nombre usando operadores relacionales
    if (producto->nombre < nodo->producto->nombre) {
        nodo->izquierdo = insertarRecursivo(nodo->izquierdo, producto);
    } else if (producto->nombre > nodo->producto->nombre) {
        nodo->derecho = insertarRecursivo(nodo->derecho, producto);
    } else {
        // Nombre duplicado: no insertar (o permitir según política)
        return nodo;
    }

    nodo->altura = maximo(obtenerAltura(nodo->izquierdo),
                          obtenerAltura(nodo->derecho)) + 1;

    int factorEquilibrio = obtenerFactorEquilibrio(nodo);

    // Caso LL: desbalance izquierdo-izquierdo
    if (factorEquilibrio > 1 && producto->nombre < nodo->izquierdo->producto->nombre) {
        return rotacionDerecha(nodo);
    }

    // Caso RR: desbalance derecho-derecho
    if (factorEquilibrio < -1 && producto->nombre > nodo->derecho->producto->nombre) {
        return rotacionIzquierda(nodo);
    }

    // Caso LR: desbalance izquierdo-derecho
    if (factorEquilibrio > 1 && producto->nombre > nodo->izquierdo->producto->nombre) {
        nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
        return rotacionDerecha(nodo);
    }

    // Caso RL: desbalance derecho-izquierdo
    if (factorEquilibrio < -1 && producto->nombre < nodo->derecho->producto->nombre) {
        nodo->derecho = rotacionDerecha(nodo->derecho);
        return rotacionIzquierda(nodo);
    }

    return nodo;
}

void ArbolAVL::insertar(Producto* producto) {
    if (producto == nullptr) {
        return;
    }
    raiz = insertarRecursivo(raiz, producto);
}

NodoAVL* ArbolAVL::obtenerNodoMinimo(NodoAVL* nodo) const {
    NodoAVL* actual = nodo;
    while (actual != nullptr && actual->izquierdo != nullptr) {
        actual = actual->izquierdo;
    }
    return actual;
}

NodoAVL* ArbolAVL::eliminarRecursivo(NodoAVL* nodo, const std::string& nombre) {
    if (nodo == nullptr) {
        return nullptr;
    }

    if (nombre < nodo->producto->nombre) {
        nodo->izquierdo = eliminarRecursivo(nodo->izquierdo, nombre);
    } else if (nombre > nodo->producto->nombre) {
        nodo->derecho = eliminarRecursivo(nodo->derecho, nombre);
    } else {
        // Nodo encontrado: proceder con eliminación
        if (nodo->izquierdo == nullptr || nodo->derecho == nullptr) {
            NodoAVL* temporal = (nodo->izquierdo != nullptr) ? nodo->izquierdo : nodo->derecho;

            if (temporal == nullptr) {
                temporal = nodo;
                nodo = nullptr;
            } else {
                *nodo = *temporal;
            }
            delete temporal;
        } else {
            // Nodo con dos hijos: obtener sucesor inorden
            NodoAVL* sucesor = obtenerNodoMinimo(nodo->derecho);
            nodo->producto = sucesor->producto;
            nodo->derecho = eliminarRecursivo(nodo->derecho, sucesor->producto->nombre);
        }
    }

    if (nodo == nullptr) {
        return nullptr;
    }

    nodo->altura = maximo(obtenerAltura(nodo->izquierdo),
                          obtenerAltura(nodo->derecho)) + 1;

    int factorEquilibrio = obtenerFactorEquilibrio(nodo);

    // Caso LL
    if (factorEquilibrio > 1 && obtenerFactorEquilibrio(nodo->izquierdo) >= 0) {
        return rotacionDerecha(nodo);
    }

    // Caso LR
    if (factorEquilibrio > 1 && obtenerFactorEquilibrio(nodo->izquierdo) < 0) {
        nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
        return rotacionDerecha(nodo);
    }

    // Caso RR
    if (factorEquilibrio < -1 && obtenerFactorEquilibrio(nodo->derecho) <= 0) {
        return rotacionIzquierda(nodo);
    }

    // Caso RL
    if (factorEquilibrio < -1 && obtenerFactorEquilibrio(nodo->derecho) > 0) {
        nodo->derecho = rotacionDerecha(nodo->derecho);
        return rotacionIzquierda(nodo);
    }

    return nodo;
}

bool ArbolAVL::eliminarPorNombre(const std::string& nombre) {
    if (buscarPorNombre(nombre) == nullptr) {
        return false;
    }
    raiz = eliminarRecursivo(raiz, nombre);
    return true;
}

// eliminar: Alias de eliminación por nombre en AVL.
// Complejidad: O(log n) promedio y peor caso garantizado por balance AVL.
bool ArbolAVL::eliminar(const std::string& nombre) {
    return eliminarPorNombre(nombre);
}

bool ArbolAVL::eliminarPorCodigoBarras(const std::string& codigoBarras) {
    // Busca por código y elimina por nombre (recorrido completo necesario)
    NodoAVL* actual = raiz;
    NodoAVL* pila[1000];
    int tope = 0;

    while (actual != nullptr || tope > 0) {
        while (actual != nullptr) {
            pila[tope++] = actual;
            actual = actual->izquierdo;
        }
        actual = pila[--tope];
        if (actual->producto != nullptr && actual->producto->codigoBarras == codigoBarras) {
            return eliminarPorNombre(actual->producto->nombre);
        }
        actual = actual->derecho;
    }
    return false;
}

Producto* ArbolAVL::buscarRecursivo(NodoAVL* nodo, const std::string& nombre) const {
    if (nodo == nullptr) {
        return nullptr;
    }

    if (nombre < nodo->producto->nombre) {
        return buscarRecursivo(nodo->izquierdo, nombre);
    } else if (nombre > nodo->producto->nombre) {
        return buscarRecursivo(nodo->derecho, nombre);
    } else {
        return nodo->producto;
    }
}

Producto* ArbolAVL::buscarPorNombre(const std::string& nombre) const {
    return buscarRecursivo(raiz, nombre);
}

void ArbolAVL::recorridoInOrdenRecursivo(NodoAVL* nodo) const {
    if (nodo == nullptr) {
        return;
    }
    recorridoInOrdenRecursivo(nodo->izquierdo);
    if (nodo->producto != nullptr) {
        std::cout << *(nodo->producto) << "\n\n";
    }
    recorridoInOrdenRecursivo(nodo->derecho);
}

void ArbolAVL::recorridoInOrden() const {
    if (estaVacio()) {
        std::cout << "El arbol AVL esta vacio.\n";
        return;
    }
    std::cout << "=== Productos ordenados alfabeticamente (AVL In-Orden) ===\n\n";
    recorridoInOrdenRecursivo(raiz);
}

bool ArbolAVL::estaVacio() const {
    return raiz == nullptr;
}

std::string ArbolAVL::escaparTexto(const std::string& texto) const {
    // Escapa caracteres especiales para Graphviz
    std::string resultado;
    for (char caracter : texto) {
        if (caracter == '"' || caracter == '\\') {
            resultado += '\\';
        }
        resultado += caracter;
    }
    return resultado;
}

void ArbolAVL::generarDotRecursivo(NodoAVL* nodo, std::ofstream& archivo, int& contador) const {
    if (nodo == nullptr) {
        return;
    }

    int idActual = contador;
    std::string nombreEscapado = escaparTexto(nodo->producto->nombre);

    archivo << "    nodo" << idActual << " [label=\"" << nombreEscapado
            << "\\nAltura: " << nodo->altura << "\"];\n";

    if (nodo->izquierdo != nullptr) {
        ++contador;
        int idIzquierdo = contador;
        archivo << "    nodo" << idActual << " -> nodo" << idIzquierdo << " [label=\"izq\"];\n";
        generarDotRecursivo(nodo->izquierdo, archivo, contador);
    }

    if (nodo->derecho != nullptr) {
        ++contador;
        int idDerecho = contador;
        archivo << "    nodo" << idActual << " -> nodo" << idDerecho << " [label=\"der\"];\n";
        generarDotRecursivo(nodo->derecho, archivo, contador);
    }
}

void ArbolAVL::generarDot(const std::string& rutaArchivo) const {
    // generarDot: Recorre el árbol actual en memoria y sobrescribe el archivo DOT.
    // Complejidad: O(n), donde n es el número de nodos AVL.
    std::ofstream archivo(rutaArchivo, std::ios::out | std::ios::trunc);
    if (!archivo.is_open()) {
        std::cout << "Error: no se pudo crear el archivo DOT.\n";
        return;
    }

    archivo << "digraph ArbolAVL {\n";
    archivo << "    node [shape=record, style=filled, fillcolor=lightblue];\n";
    archivo << "    rankdir=TB;\n";

    if (raiz != nullptr) {
        int contador = 0;
        generarDotRecursivo(raiz, archivo, contador);
    } else {
        archivo << "    vacio [label=\"Arbol vacio\", shape=plaintext];\n";
    }

    archivo << "}\n";
    archivo.close();

    std::cout << "Archivo DOT generado: " << rutaArchivo << '\n';
}

void ArbolAVL::generarImagen() const {
    generarDot("data/avl.dot");
    int resultado = std::system("dot -Tpng data/avl.dot -o data/avl.png");
    if (resultado == 0) {
        std::cout << "Imagen PNG generada: data/avl.png\n";
    } else {
        std::cout << "Error al generar imagen. Asegurese de tener Graphviz instalado.\n";
    }
}
