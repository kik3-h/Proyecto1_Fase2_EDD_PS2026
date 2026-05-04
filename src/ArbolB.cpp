#include "ArbolB.h"

#include <cstdlib>
#include <iostream>

// ClaveFecha 

ClaveFecha::ClaveFecha() : fecha(""), productos(nullptr) {}

ClaveFecha::~ClaveFecha() {
    if (productos != nullptr) {
        delete productos;
        productos = nullptr;
    }
}

// NodoB 

NodoB::NodoB(int t, bool hoja)
    : claves(nullptr), hijos(nullptr), numClaves(0), esHoja(hoja), gradoMinimo(t) {
    // Máximo de claves: 2t - 1, máximo de hijos: 2t
    int maxClaves = 2 * t - 1;
    int maxHijos = 2 * t;

    claves = new ClaveFecha[maxClaves];
    hijos = new NodoB*[maxHijos];

    for (int i = 0; i < maxHijos; ++i) {
        hijos[i] = nullptr;
    }
}

NodoB::~NodoB() {
    delete[] claves;
    claves = nullptr;

    delete[] hijos;
    hijos = nullptr;
}

//ArbolB 

ArbolB::ArbolB(int t) : raiz(nullptr), gradoMinimo(t) {}

ArbolB::~ArbolB() {
    destruirRecursivo(raiz);
    raiz = nullptr;
}

void ArbolB::destruirRecursivo(NodoB* nodo) {
    if (nodo == nullptr) {
        return;
    }

    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; ++i) {
            destruirRecursivo(nodo->hijos[i]);
        }
    }

    delete nodo;
}

bool ArbolB::estaVacio() const {
    return raiz == nullptr;
}

ClaveFecha* ArbolB::buscarClaveRecursivo(NodoB* nodo, const std::string& fecha) const {
    if (nodo == nullptr) {
        return nullptr;
    }

    int i = 0;
    while (i < nodo->numClaves && fecha > nodo->claves[i].fecha) {
        ++i;
    }

    if (i < nodo->numClaves && fecha == nodo->claves[i].fecha) {
        return &(nodo->claves[i]);
    }

    if (nodo->esHoja) {
        return nullptr;
    }

    return buscarClaveRecursivo(nodo->hijos[i], fecha);
}

void ArbolB::insertar(Producto* producto) {
    if (producto == nullptr) {
        return;
    }

    const std::string& fecha = producto->fechaCaducidad;

    // Buscar si ya existe la fecha en el árbol
    ClaveFecha* claveExistente = buscarClaveRecursivo(raiz, fecha);
    if (claveExistente != nullptr) {
        // La fecha ya existe: agregar producto a su lista
        claveExistente->productos->insertar(producto);
        return;
    }

    // La fecha no existe: insertar nueva clave
    if (raiz == nullptr) {
        raiz = new NodoB(gradoMinimo, true);
        raiz->claves[0].fecha = fecha;
        raiz->claves[0].productos = new ListaEnlazada();
        raiz->claves[0].productos->insertar(producto);
        raiz->numClaves = 1;
        return;
    }

    // Si la raíz está llena, dividirla
    if (raiz->numClaves == 2 * gradoMinimo - 1) {
        NodoB* nuevaRaiz = new NodoB(gradoMinimo, false);
        nuevaRaiz->hijos[0] = raiz;
        dividirHijo(nuevaRaiz, 0, raiz);

        // Decidir en qué hijo insertar
        int i = 0;
        if (nuevaRaiz->claves[0].fecha < fecha) {
            ++i;
        }
        insertarEnNodoNoLleno(nuevaRaiz->hijos[i], producto);
        raiz = nuevaRaiz;
    } else {
        insertarEnNodoNoLleno(raiz, producto);
    }
}

void ArbolB::insertarEnNodoNoLleno(NodoB* nodo, Producto* producto) {
    const std::string& fecha = producto->fechaCaducidad;
    int i = nodo->numClaves - 1;

    if (nodo->esHoja) {
        // Mover claves mayores hacia la derecha
        while (i >= 0 && nodo->claves[i].fecha > fecha) {
            nodo->claves[i + 1] = nodo->claves[i];
            // Limpiar la clave original para evitar doble delete
            nodo->claves[i].fecha = "";
            nodo->claves[i].productos = nullptr;
            --i;
        }

        // Verificar si la fecha ya existe (por si acaso)
        if (i >= 0 && nodo->claves[i].fecha == fecha) {
            nodo->claves[i].productos->insertar(producto);
            return;
        }

        // Insertar nueva clave
        nodo->claves[i + 1].fecha = fecha;
        nodo->claves[i + 1].productos = new ListaEnlazada();
        nodo->claves[i + 1].productos->insertar(producto);
        ++(nodo->numClaves);
    } else {
        // Encontrar el hijo donde insertar
        while (i >= 0 && nodo->claves[i].fecha > fecha) {
            --i;
        }

        // Verificar si la clave existe en este nodo
        if (i >= 0 && nodo->claves[i].fecha == fecha) {
            nodo->claves[i].productos->insertar(producto);
            return;
        }

        ++i;

        // Si el hijo está lleno, dividirlo
        if (nodo->hijos[i]->numClaves == 2 * gradoMinimo - 1) {
            dividirHijo(nodo, i, nodo->hijos[i]);

            // Después de dividir, decidir cuál hijo usar
            if (nodo->claves[i].fecha < fecha) {
                ++i;
            } else if (nodo->claves[i].fecha == fecha) {
                nodo->claves[i].productos->insertar(producto);
                return;
            }
        }

        insertarEnNodoNoLleno(nodo->hijos[i], producto);
    }
}

void ArbolB::dividirHijo(NodoB* padre, int indice, NodoB* hijo) {
    int t = gradoMinimo;

    // Crear nuevo nodo que contendrá las claves derechas del hijo
    NodoB* nuevoNodo = new NodoB(t, hijo->esHoja);
    nuevoNodo->numClaves = t - 1;

    // Copiar las últimas (t-1) claves del hijo al nuevo nodo
    for (int j = 0; j < t - 1; ++j) {
        nuevoNodo->claves[j].fecha = hijo->claves[j + t].fecha;
        nuevoNodo->claves[j].productos = hijo->claves[j + t].productos;
        // Limpiar referencia en hijo para evitar doble delete
        hijo->claves[j + t].fecha = "";
        hijo->claves[j + t].productos = nullptr;
    }

    // Si no es hoja, copiar los últimos t hijos
    if (!hijo->esHoja) {
        for (int j = 0; j < t; ++j) {
            nuevoNodo->hijos[j] = hijo->hijos[j + t];
            hijo->hijos[j + t] = nullptr;
        }
    }

    hijo->numClaves = t - 1;

    // Mover hijos del padre para hacer espacio al nuevo hijo
    for (int j = padre->numClaves; j >= indice + 1; --j) {
        padre->hijos[j + 1] = padre->hijos[j];
    }
    padre->hijos[indice + 1] = nuevoNodo;

    // Mover claves del padre para hacer espacio a la clave mediana
    for (int j = padre->numClaves - 1; j >= indice; --j) {
        padre->claves[j + 1].fecha = padre->claves[j].fecha;
        padre->claves[j + 1].productos = padre->claves[j].productos;
        padre->claves[j].fecha = "";
        padre->claves[j].productos = nullptr;
    }

    // Subir la clave mediana al padre
    padre->claves[indice].fecha = hijo->claves[t - 1].fecha;
    padre->claves[indice].productos = hijo->claves[t - 1].productos;
    hijo->claves[t - 1].fecha = "";
    hijo->claves[t - 1].productos = nullptr;

    ++(padre->numClaves);
}

// recolectarProductosRecursivo: Recorre el árbol B en in-orden y acumula los
// punteros Producto* almacenados en cada clave.
// Complejidad: O(n), donde n es el total de productos indexados.
void ArbolB::recolectarProductosRecursivo(NodoB* nodo,
                                          std::vector<Producto*>& resultado,
                                          const std::string* fechaExcluida) const {
    if (nodo == nullptr) {
        return;
    }

    for (int i = 0; i < nodo->numClaves; ++i) {
        if (!nodo->esHoja) {
            recolectarProductosRecursivo(nodo->hijos[i], resultado, fechaExcluida);
        }

        const bool fechaFiltrada = (fechaExcluida != nullptr &&
                                    nodo->claves[i].fecha == *fechaExcluida);
        if (!fechaFiltrada && nodo->claves[i].productos != nullptr) {
            std::vector<Producto*> productosClave = nodo->claves[i].productos->obtenerTodos();
            for (Producto* producto : productosClave) {
                if (producto != nullptr) {
                    resultado.push_back(producto);
                }
            }
        }
    }

    if (!nodo->esHoja) {
        recolectarProductosRecursivo(nodo->hijos[nodo->numClaves], resultado, fechaExcluida);
    }
}

// reconstruirDesdeProductos: Reinicia el árbol y reinserta todos los productos
// restantes para mantener claves sin listas vacías y estructura consistente.
// Complejidad: O(n log_m n), con m = grado del árbol B.
bool ArbolB::reconstruirDesdeProductos(const std::vector<Producto*>& productos) {
    destruirRecursivo(raiz);
    raiz = nullptr;

    for (Producto* producto : productos) {
        if (producto != nullptr) {
            insertar(producto);
        }
    }

    return true;
}

// eliminar: Elimina la clave completa de una fecha y reconstruye el árbol para
// conservar invariantes de ocupación mínima y balance del índice.
// Complejidad: O(n log_m n), por reindexación de productos restantes.
bool ArbolB::eliminar(const std::string& fecha) {
    if (fecha.empty()) {
        return false;
    }

    ClaveFecha* clave = buscarClaveRecursivo(raiz, fecha);
    if (clave == nullptr) {
        return false;
    }

    std::vector<Producto*> productosRestantes;
    recolectarProductosRecursivo(raiz, productosRestantes, &fecha);
    return reconstruirDesdeProductos(productosRestantes);
}

// eliminar: Quita un producto por (fecha, código). Si la fecha queda sin
// productos, reconstruye el árbol para remover la clave vacía y preservar
// invariantes estructurales del índice.
// Complejidad: O(log_m n) cuando la fecha conserva elementos; O(n log_m n)
// cuando requiere reconstrucción por clave vacía.
bool ArbolB::eliminar(const std::string& fecha, const std::string& codigoBarras) {
    if (fecha.empty() || codigoBarras.empty()) {
        return false;
    }

    ClaveFecha* clave = buscarClaveRecursivo(raiz, fecha);
    if (clave == nullptr || clave->productos == nullptr) {
        return false;
    }

    const bool eliminado = clave->productos->eliminarPorCodigoBarras(codigoBarras);
    if (!eliminado) {
        return false;
    }

    if (!clave->productos->estaVacia()) {
        return true;
    }

    std::vector<Producto*> productosRestantes;
    recolectarProductosRecursivo(raiz, productosRestantes);
    return reconstruirDesdeProductos(productosRestantes);
}

void ArbolB::buscarPorRango(const std::string& fechaInicio, const std::string& fechaFin) const {
    if (raiz == nullptr) {
        std::cout << "El arbol B esta vacio.\n";
        return;
    }

    std::cout << "=== Productos con fecha de caducidad entre "
              << fechaInicio << " y " << fechaFin << " ===\n\n";

    int contador = 0;
    buscarRangoRecursivo(raiz, fechaInicio, fechaFin, contador);

    if (contador == 0) {
        std::cout << "No se encontraron productos en el rango especificado.\n";
    } else {
        std::cout << "Total de fechas encontradas: " << contador << "\n";
    }
}

void ArbolB::buscarRangoRecursivo(NodoB* nodo,
                                   const std::string& fechaInicio,
                                   const std::string& fechaFin,
                                   int& contador) const {
    if (nodo == nullptr) {
        return;
    }

    int i = 0;

    // Recorrido in-orden para obtener claves ordenadas
    for (i = 0; i < nodo->numClaves; ++i) {
        // Visitar hijo izquierdo si la fecha puede estar ahí
        if (!nodo->esHoja && nodo->claves[i].fecha >= fechaInicio) {
            buscarRangoRecursivo(nodo->hijos[i], fechaInicio, fechaFin, contador);
        }

        // Verificar si la clave actual está en el rango
        if (nodo->claves[i].fecha >= fechaInicio && nodo->claves[i].fecha <= fechaFin) {
            ++contador;
            std::cout << "--- Fecha: " << nodo->claves[i].fecha << " ---\n";
            if (nodo->claves[i].productos != nullptr) {
                nodo->claves[i].productos->mostrarTodos();
            }
        }

        // Si ya pasamos el rango, no seguir
        if (nodo->claves[i].fecha > fechaFin) {
            return;
        }
    }

    // Visitar el último hijo si es necesario
    if (!nodo->esHoja) {
        buscarRangoRecursivo(nodo->hijos[i], fechaInicio, fechaFin, contador);
    }
}


// Retorna productos en rango de fechas (para GUI) - versión recursiva auxiliar

void ArbolB::obtenerRangoRecursivo(NodoB* nodo,
                                    const std::string& fechaInicio,
                                    const std::string& fechaFin,
                                    std::vector<Producto*>& resultado) const {
    if (nodo == nullptr) {
        return;
    }

    int i = 0;

    for (i = 0; i < nodo->numClaves; ++i) {
        // Visitar hijo izquierdo si la fecha puede estar ahí
        if (!nodo->esHoja && nodo->claves[i].fecha >= fechaInicio) {
            obtenerRangoRecursivo(nodo->hijos[i], fechaInicio, fechaFin, resultado);
        }

        // Si la clave está en el rango, agregar sus productos
        if (nodo->claves[i].fecha >= fechaInicio && nodo->claves[i].fecha <= fechaFin) {
            if (nodo->claves[i].productos != nullptr) {
                std::vector<Producto*> productos = nodo->claves[i].productos->obtenerTodos();
                for (Producto* p : productos) {
                    resultado.push_back(p);
                }
            }
        }

        // Si ya pasamos el rango, parar
        if (nodo->claves[i].fecha > fechaFin) {
            return;
        }
    }

    // Visitar último hijo
    if (!nodo->esHoja) {
        obtenerRangoRecursivo(nodo->hijos[i], fechaInicio, fechaFin, resultado);
    }
}

// Retorna productos en un rango de fechas en un vector (para GUI)
std::vector<Producto*> ArbolB::obtenerPorRango(const std::string& fechaInicio, 
                                                const std::string& fechaFin) const {
    std::vector<Producto*> resultado;
    
    if (raiz != nullptr) {
        obtenerRangoRecursivo(raiz, fechaInicio, fechaFin, resultado);
    }
    
    return resultado;
}

std::string ArbolB::escaparTexto(const std::string& texto) const {
    std::string resultado;
    for (char c : texto) {
        if (c == '"' || c == '\\' || c == '<' || c == '>' || c == '|') {
            resultado += '\\';
        }
        resultado += c;
    }
    return resultado;
}

void ArbolB::generarDotRecursivo(NodoB* nodo, std::ofstream& archivo, int& contadorNodo) const {
    if (nodo == nullptr) {
        return;
    }

    int idActual = contadorNodo;

    // Generar etiqueta del nodo tipo registro (múltiples claves)
    archivo << "    nodo" << idActual << " [label=\"";
    for (int i = 0; i < nodo->numClaves; ++i) {
        if (i > 0) {
            archivo << "|";
        }
        archivo << "<f" << i << "> " << escaparTexto(nodo->claves[i].fecha);
    }
    archivo << "\"];\n";

    // Generar conexiones a hijos
    if (!nodo->esHoja) {
        for (int i = 0; i <= nodo->numClaves; ++i) {
            if (nodo->hijos[i] != nullptr) {
                ++contadorNodo;
                int idHijo = contadorNodo;
                archivo << "    nodo" << idActual << " -> nodo" << idHijo << ";\n";
                generarDotRecursivo(nodo->hijos[i], archivo, contadorNodo);
            }
        }
    }
}

void ArbolB::generarDot(const std::string& rutaArchivo) const {
    // generarDot: Recorre el árbol B vigente en memoria y regenera el DOT.
    // Complejidad: O(k), donde k es la cantidad de claves almacenadas.
    std::ofstream archivo(rutaArchivo, std::ios::out | std::ios::trunc);
    if (!archivo.is_open()) {
        std::cout << "Error: no se pudo crear el archivo DOT.\n";
        return;
    }

    archivo << "digraph ArbolB {\n";
    archivo << "    node [shape=record, style=filled, fillcolor=lightyellow];\n";
    archivo << "    rankdir=TB;\n";

    if (raiz != nullptr) {
        int contador = 0;
        generarDotRecursivo(raiz, archivo, contador);
    } else {
        archivo << "    vacio [label=\"Arbol B vacio\", shape=plaintext];\n";
    }

    archivo << "}\n";
    archivo.close();

    std::cout << "Archivo DOT generado: " << rutaArchivo << '\n';
}

void ArbolB::generarImagen() const {
    generarDot("data/arbol_b.dot");
    int resultado = std::system("dot -Tpng data/arbol_b.dot -o data/arbol_b.png");
    if (resultado == 0) {
        std::cout << "Imagen PNG generada: data/arbol_b.png\n";
    } else {
        std::cout << "Error al generar imagen. Asegurese de tener Graphviz instalado.\n";
    }
}
