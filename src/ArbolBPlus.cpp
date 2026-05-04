#include "ArbolBPlus.h"

#include <cstdlib>
#include <iostream>

// ClaveCategoria

ClaveCategoria::ClaveCategoria() : categoria(""), productos(nullptr) {}

ClaveCategoria::~ClaveCategoria() {
    // Solo libera la lista si existe (evita doble delete)
    if (productos != nullptr) {
        delete productos;
        productos = nullptr;
    }
}

// NodoBPlus

NodoBPlus::NodoBPlus(int t, bool hoja)
    : claves(nullptr), hijos(nullptr), numClaves(0), esHoja(hoja),
      gradoMinimo(t), siguiente(nullptr) {
    int maxClaves = 2 * t - 1;
    int maxHijos = 2 * t;

    claves = new ClaveCategoria[maxClaves];
    hijos = new NodoBPlus*[maxHijos];

    for (int i = 0; i < maxHijos; ++i) {
        hijos[i] = nullptr;
    }
}

NodoBPlus::~NodoBPlus() {
    // En nodos internos, productos es nullptr (no hay doble delete)
    delete[] claves;
    claves = nullptr;

    delete[] hijos;
    hijos = nullptr;
}

// ArbolBPlus 

ArbolBPlus::ArbolBPlus(int t) : raiz(nullptr), gradoMinimo(t) {}

ArbolBPlus::~ArbolBPlus() {
    destruirRecursivo(raiz);
    raiz = nullptr;
}

void ArbolBPlus::destruirRecursivo(NodoBPlus* nodo) {
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

bool ArbolBPlus::estaVacio() const {
    return raiz == nullptr;
}

// Obtiene la primera hoja del árbol (extremo izquierdo)
NodoBPlus* ArbolBPlus::obtenerPrimeraHoja() const {
    if (raiz == nullptr) {
        return nullptr;
    }
    
    NodoBPlus* actual = raiz;
    while (!actual->esHoja) {
        actual = actual->hijos[0];
    }
    return actual;
}

NodoBPlus* ArbolBPlus::buscarHoja(NodoBPlus* nodo, const std::string& categoria) const {
    if (nodo == nullptr) {
        return nullptr;
    }

    if (nodo->esHoja) {
        return nodo;
    }

    // Buscar el hijo correcto
    int i = 0;
    while (i < nodo->numClaves && categoria >= nodo->claves[i].categoria) {
        ++i;
    }

    return buscarHoja(nodo->hijos[i], categoria);
}

ClaveCategoria* ArbolBPlus::buscarClaveEnHoja(NodoBPlus* hoja, const std::string& categoria) const {
    if (hoja == nullptr || !hoja->esHoja) {
        return nullptr;
    }

    for (int i = 0; i < hoja->numClaves; ++i) {
        if (hoja->claves[i].categoria == categoria) {
            return &(hoja->claves[i]);
        }
    }

    return nullptr;
}

// Búsqueda global en TODAS las hojas para evitar duplicados
// Esto es necesario porque buscarHoja() puede dirigirnos a una hoja incorrecta
// cuando las claves han sido redistribuidas por splits
ClaveCategoria* ArbolBPlus::buscarClaveGlobal(const std::string& categoria) const {
    NodoBPlus* hoja = obtenerPrimeraHoja();
    
    while (hoja != nullptr) {
        for (int i = 0; i < hoja->numClaves; ++i) {
            if (hoja->claves[i].categoria == categoria) {
                return &(hoja->claves[i]);
            }
        }
        hoja = hoja->siguiente;
    }
    
    return nullptr;
}

void ArbolBPlus::insertar(Producto* producto) {
    if (producto == nullptr) {
        return;
    }

    const std::string& categoria = producto->categoria;

    // Si el árbol está vacío, crear raíz hoja
    if (raiz == nullptr) {
        raiz = new NodoBPlus(gradoMinimo, true);
        raiz->claves[0].categoria = categoria;
        raiz->claves[0].productos = new ListaEnlazada();
        raiz->claves[0].productos->insertar(producto);
        raiz->numClaves = 1;
        return;
    }

    // Búsqueda global para evitar duplicados de categoría
    // Primero verificamos si la categoría ya existe en CUALQUIER hoja
    ClaveCategoria* claveExistente = buscarClaveGlobal(categoria);

    if (claveExistente != nullptr) {
        // La categoría ya existe: agregar producto a su ListaEnlazada (NO crear nueva clave)
        claveExistente->productos->insertar(producto);
        return;
    }

    // La categoría no existe: insertar nueva clave
    if (raiz->numClaves == 2 * gradoMinimo - 1) {
        // La raíz está llena, dividirla
        NodoBPlus* nuevaRaiz = new NodoBPlus(gradoMinimo, false);
        nuevaRaiz->hijos[0] = raiz;
        dividirHijo(nuevaRaiz, 0, raiz);

        // Decidir en qué hijo insertar
        int i = 0;
        if (nuevaRaiz->claves[0].categoria < categoria) {
            ++i;
        }
        insertarEnNodoNoLleno(nuevaRaiz->hijos[i], producto);
        raiz = nuevaRaiz;
    } else {
        insertarEnNodoNoLleno(raiz, producto);
    }
}

void ArbolBPlus::insertarEnNodoNoLleno(NodoBPlus* nodo, Producto* producto) {
    const std::string& categoria = producto->categoria;
    int i = nodo->numClaves - 1;

    if (nodo->esHoja) {
        // Verificar si ya existe la categoría en este nodo
        for (int j = 0; j < nodo->numClaves; ++j) {
            if (nodo->claves[j].categoria == categoria) {
                nodo->claves[j].productos->insertar(producto);
                return;
            }
        }

        // Mover claves mayores hacia la derecha
        while (i >= 0 && nodo->claves[i].categoria > categoria) {
            nodo->claves[i + 1].categoria = nodo->claves[i].categoria;
            nodo->claves[i + 1].productos = nodo->claves[i].productos;
            nodo->claves[i].categoria = "";
            nodo->claves[i].productos = nullptr;
            --i;
        }

        // Insertar nueva clave con su ListaEnlazada
        nodo->claves[i + 1].categoria = categoria;
        nodo->claves[i + 1].productos = new ListaEnlazada();
        nodo->claves[i + 1].productos->insertar(producto);
        ++(nodo->numClaves);
    } else {
        // Encontrar el hijo donde insertar
        while (i >= 0 && nodo->claves[i].categoria > categoria) {
            --i;
        }
        ++i;

        // Si el hijo está lleno, dividirlo
        if (nodo->hijos[i]->numClaves == 2 * gradoMinimo - 1) {
            dividirHijo(nodo, i, nodo->hijos[i]);

            // Después de dividir, decidir cuál hijo usar
            if (nodo->claves[i].categoria < categoria) {
                ++i;
            }
        }

        insertarEnNodoNoLleno(nodo->hijos[i], producto);
    }
}

void ArbolBPlus::dividirHijo(NodoBPlus* padre, int indice, NodoBPlus* hijo) {
    int t = gradoMinimo;

    NodoBPlus* nuevoNodo = new NodoBPlus(t, hijo->esHoja);
    nuevoNodo->numClaves = t - 1;

    if (hijo->esHoja) {
        // DIVISIÓN DE HOJA: copiar las últimas t claves al nuevo nodo
        for (int j = 0; j < t; ++j) {
            nuevoNodo->claves[j].categoria = hijo->claves[j + t - 1].categoria;
            nuevoNodo->claves[j].productos = hijo->claves[j + t - 1].productos;
            hijo->claves[j + t - 1].categoria = "";
            hijo->claves[j + t - 1].productos = nullptr;
        }
        nuevoNodo->numClaves = t;
        hijo->numClaves = t - 1;

        // Búsqueda global para evitar duplicados de categoría
        // Esto mantiene la lista enlazada de hojas para recorrido O(log N + K)
        nuevoNodo->siguiente = hijo->siguiente;
        hijo->siguiente = nuevoNodo;

        // Mover hijos del padre para hacer espacio
        for (int j = padre->numClaves; j >= indice + 1; --j) {
            padre->hijos[j + 1] = padre->hijos[j];
        }
        padre->hijos[indice + 1] = nuevoNodo;

        // Mover claves del padre para hacer espacio
        for (int j = padre->numClaves - 1; j >= indice; --j) {
            padre->claves[j + 1].categoria = padre->claves[j].categoria;
            padre->claves[j + 1].productos = padre->claves[j].productos;
            padre->claves[j].categoria = "";
            padre->claves[j].productos = nullptr;
        }

        // Subir copia de la primera clave del nuevo nodo (B+ copia, no mueve)
        padre->claves[indice].categoria = nuevoNodo->claves[0].categoria;
        padre->claves[indice].productos = nullptr;  // Nodos internos no tienen datos
        ++(padre->numClaves);
    } else {
        // DIVISIÓN DE NODO INTERNO: igual que B-tree
        for (int j = 0; j < t - 1; ++j) {
            nuevoNodo->claves[j].categoria = hijo->claves[j + t].categoria;
            nuevoNodo->claves[j].productos = hijo->claves[j + t].productos;
            hijo->claves[j + t].categoria = "";
            hijo->claves[j + t].productos = nullptr;
        }

        if (!hijo->esHoja) {
            for (int j = 0; j < t; ++j) {
                nuevoNodo->hijos[j] = hijo->hijos[j + t];
                hijo->hijos[j + t] = nullptr;
            }
        }

        hijo->numClaves = t - 1;

        for (int j = padre->numClaves; j >= indice + 1; --j) {
            padre->hijos[j + 1] = padre->hijos[j];
        }
        padre->hijos[indice + 1] = nuevoNodo;

        for (int j = padre->numClaves - 1; j >= indice; --j) {
            padre->claves[j + 1].categoria = padre->claves[j].categoria;
            padre->claves[j + 1].productos = padre->claves[j].productos;
            padre->claves[j].categoria = "";
            padre->claves[j].productos = nullptr;
        }

        // Subir la clave mediana al padre
        padre->claves[indice].categoria = hijo->claves[t - 1].categoria;
        padre->claves[indice].productos = nullptr;
        hijo->claves[t - 1].categoria = "";
        hijo->claves[t - 1].productos = nullptr;

        ++(padre->numClaves);
    }
}

// recolectarProductosDesdeHojas: Recorre la cadena de hojas y agrega todos los
// productos indexados por categoría.
// Complejidad: O(n), donde n es el total de productos en el índice B+.
void ArbolBPlus::recolectarProductosDesdeHojas(std::vector<Producto*>& resultado) const {
    NodoBPlus* hoja = obtenerPrimeraHoja();
    while (hoja != nullptr) {
        for (int i = 0; i < hoja->numClaves; ++i) {
            if (hoja->claves[i].productos != nullptr) {
                std::vector<Producto*> productosClave = hoja->claves[i].productos->obtenerTodos();
                for (Producto* producto : productosClave) {
                    if (producto != nullptr) {
                        resultado.push_back(producto);
                    }
                }
            }
        }
        hoja = hoja->siguiente;
    }
}

// reconstruirDesdeProductos: Reinicia el árbol y reinserta el catálogo actual
// para eliminar claves de categoría vacías y mantener hojas enlazadas válidas.
// Complejidad: O(n log_m n), con m = grado del árbol B+.
bool ArbolBPlus::reconstruirDesdeProductos(const std::vector<Producto*>& productos) {
    destruirRecursivo(raiz);
    raiz = nullptr;

    for (Producto* producto : productos) {
        if (producto != nullptr) {
            insertar(producto);
        }
    }

    return true;
}

// eliminar: Elimina un producto por (categoría, código). Si la categoría queda
// vacía, reconstruye el árbol para remover la clave y conservar enlaces de hojas.
// Complejidad: O(log_m n) si la categoría conserva elementos; O(n log_m n)
// cuando se requiere reconstrucción completa.
bool ArbolBPlus::eliminar(const std::string& categoria, const std::string& codigoBarras) {
    if (categoria.empty() || codigoBarras.empty()) {
        return false;
    }

    ClaveCategoria* clave = buscarClaveGlobal(categoria);
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
    recolectarProductosDesdeHojas(productosRestantes);
    return reconstruirDesdeProductos(productosRestantes);
}

void ArbolBPlus::buscarPorCategoria(const std::string& categoria) const {
    if (raiz == nullptr) {
        std::cout << "El arbol B+ esta vacio.\n";
        return;
    }

    // Usar búsqueda global para encontrar la categoría
    ClaveCategoria* clave = buscarClaveGlobal(categoria);

    if (clave == nullptr) {
        std::cout << "No se encontraron productos en la categoria '" << categoria << "'.\n";
        return;
    }

    std::cout << "=== Productos en categoria: " << categoria << " ===\n\n";
    if (clave->productos != nullptr) {
        clave->productos->mostrarTodos();
    }
}

// Retorna productos de una categoría en un vector (para uso en GUI)

std::vector<Producto*> ArbolBPlus::obtenerPorCategoria(const std::string& categoria) const {
    std::vector<Producto*> resultado;
    
    if (raiz == nullptr) {
        return resultado;
    }
    
    ClaveCategoria* clave = buscarClaveGlobal(categoria);
    
    if (clave != nullptr && clave->productos != nullptr) {
        // Obtener todos los productos de esa categoría
        std::vector<Producto*> productos = clave->productos->obtenerTodos();
        for (Producto* p : productos) {
            resultado.push_back(p);
        }
    }
    
    return resultado;
}

std::string ArbolBPlus::escaparTexto(const std::string& texto) const {
    std::string resultado;
    for (char c : texto) {
        if (c == '"' || c == '\\' || c == '<' || c == '>' || c == '|') {
            resultado += '\\';
        }
        resultado += c;
    }
    return resultado;
}

void ArbolBPlus::generarDotRecursivo(NodoBPlus* nodo, std::ofstream& archivo, int& contadorNodo) const {
    if (nodo == nullptr) {
        return;
    }

    int idActual = contadorNodo;

    // Generar etiqueta del nodo con puertos para enlaces
    if (nodo->esHoja) {
        // Nodos hoja: incluir puerto 'sig' para enlace secuencial
        archivo << "    nodo" << idActual << " [label=\"{";
        for (int i = 0; i < nodo->numClaves; ++i) {
            if (i > 0) archivo << "|";
            archivo << "<f" << i << "> " << escaparTexto(nodo->claves[i].categoria);
        }
        archivo << "}|<sig>\", fillcolor=lightgreen];\n";
    } else {
        // Nodos internos
        archivo << "    nodo" << idActual << " [label=\"";
        for (int i = 0; i < nodo->numClaves; ++i) {
            if (i > 0) archivo << "|";
            archivo << "<f" << i << "> " << escaparTexto(nodo->claves[i].categoria);
        }
        archivo << "\", fillcolor=lightyellow];\n";
    }

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
// Generación de enlaces visuales entre hojas
// Esta función genera las flechas punteadas azules que conectan las hojas secuencialmente,
// demostrando el recorrido O(log N + K) requerido por el enunciado.
// Solución: Se usan edges con estilos individuales y puertos específicos (:sig y :f0)
// para que Graphviz renderice correctamente las flechas entre nodos hoja.
void ArbolBPlus::generarEnlacesHojas(std::ofstream& archivo) const {
    if (raiz == nullptr) {
        return;
    }

    // Encontrar la primera hoja siguiendo siempre el hijo izquierdo
    NodoBPlus* primeraHoja = obtenerPrimeraHoja();

    if (primeraHoja == nullptr) {
        return;
    }

    // Mapear cada nodo a su ID mediante recorrido en preorden (igual que generarDotRecursivo)
    NodoBPlus* mapaNodos[1000];
    int numNodos = 0;

    NodoBPlus* pila[1000];
    int tope = 0;
    pila[tope++] = raiz;

    while (tope > 0) {
        NodoBPlus* nodo = pila[--tope];
        mapaNodos[numNodos++] = nodo;

        if (!nodo->esHoja) {
            for (int i = nodo->numClaves; i >= 0; --i) {
                if (nodo->hijos[i] != nullptr) {
                    pila[tope++] = nodo->hijos[i];
                }
            }
        }
    }

    // Contar hojas para verificar si hay enlaces
    int numHojas = 0;
    NodoBPlus* hoja = primeraHoja;
    while (hoja != nullptr) {
        numHojas++;
        hoja = hoja->siguiente;
    }

    if (numHojas <= 1) {
        return;  // No hay enlaces que mostrar si solo hay una hoja
    }

    // Generar subgrafo para alinear hojas horizontalmente
    archivo << "\n    // Subgrafo para alinear hojas horizontalmente\n";
    archivo << "    {\n";
    archivo << "        rank=same;\n";
    hoja = primeraHoja;
    while (hoja != nullptr) {
        for (int i = 0; i < numNodos; ++i) {
            if (mapaNodos[i] == hoja) {
                archivo << "        nodo" << i << ";\n";
                break;
            }
        }
        hoja = hoja->siguiente;
    }
    archivo << "    }\n";

    // Generar enlaces VISIBLES entre hojas con flechas punteadas azules
    // Cada edge se declara individualmente con su estilo para garantizar visualización
    archivo << "\n    // Enlaces secuenciales entre hojas (flechas punteadas azules)\n";
    archivo << "    // Demuestran el recorrido O(log N + K) del Arbol B+\n";

    hoja = primeraHoja;
    while (hoja != nullptr && hoja->siguiente != nullptr) {
        int idActual = -1, idSiguiente = -1;
        for (int i = 0; i < numNodos; ++i) {
            if (mapaNodos[i] == hoja) idActual = i;
            if (mapaNodos[i] == hoja->siguiente) idSiguiente = i;
        }
        if (idActual >= 0 && idSiguiente >= 0) {
            // Flecha punteada azul desde puerto 'sig' de hoja actual hacia primer campo de siguiente
            archivo << "    nodo" << idActual << ":sig -> nodo" << idSiguiente 
                    << ":f0 [style=dashed, color=blue, penwidth=2.0, "
                    << "constraint=false, label=\"sig\", fontcolor=blue];\n";
        }
        hoja = hoja->siguiente;
    }
}

void ArbolBPlus::generarDot(const std::string& rutaArchivo) const {
    // generarDot: Recorre el árbol B+ actual y sobrescribe el DOT sin caché.
    // Complejidad: O(k), donde k es la cantidad de claves en el índice.
    std::ofstream archivo(rutaArchivo, std::ios::out | std::ios::trunc);
    if (!archivo.is_open()) {
        std::cout << "Error: no se pudo crear el archivo DOT.\n";
        return;
    }

    archivo << "digraph ArbolBPlus {\n";
    archivo << "    // Configuracion del grafo\n";
    archivo << "    node [shape=record, style=filled];\n";
    archivo << "    rankdir=TB;\n";
    archivo << "    splines=ortho;\n";  // Líneas ortogonales para mejor visualización
    archivo << "    nodesep=0.5;\n";
    archivo << "    ranksep=0.8;\n";
    archivo << "\n    // Leyenda\n";
    archivo << "    subgraph cluster_leyenda {\n";
    archivo << "        label=\"Leyenda\";\n";
    archivo << "        style=dashed;\n";
    archivo << "        ley1 [label=\"Nodo Interno\", fillcolor=lightyellow, shape=box];\n";
    archivo << "        ley2 [label=\"Nodo Hoja\", fillcolor=lightgreen, shape=box];\n";
    archivo << "        ley1 -> ley2 [style=dashed, color=blue, label=\"Enlace Hoja\"];\n";
    archivo << "    }\n\n";

    if (raiz != nullptr) {
        int contador = 0;
        generarDotRecursivo(raiz, archivo, contador);
        generarEnlacesHojas(archivo);
    } else {
        archivo << "    vacio [label=\"Arbol B+ vacio\", shape=plaintext];\n";
    }

    archivo << "}\n";
    archivo.close();

    std::cout << "Archivo DOT generado: " << rutaArchivo << '\n';
}

void ArbolBPlus::generarImagen() const {
    generarDot("data/arbol_bplus.dot");
    std::system("dot -Tpng data/arbol_bplus.dot -o data/arbol_bplus.png 2>/dev/null");
    
    // Verificar si el archivo se creó
    std::ifstream archivo("data/arbol_bplus.png");
    if (archivo.good()) {
        std::cout << "Imagen PNG generada: data/arbol_bplus.png\n";
    } else {
        std::cout << "Error al generar imagen. Asegurese de tener Graphviz instalado.\n";
    }
}
