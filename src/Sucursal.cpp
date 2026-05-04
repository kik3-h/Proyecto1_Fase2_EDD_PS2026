#include "Sucursal.h"

// Sucursal: implementación de inventario local con sincronización entre índices.
// Todas las operaciones usan estructuras manuales y gestión explícita de memoria.

Sucursal::Sucursal(int idSucursal,
                   const std::string& nombreSucursal,
                   const std::string& ubicacionSucursal,
                   int tiempoIngresoSucursal,
                   int tiempoPreparacionSucursal,
                   int intervaloDespachoSucursal)
    : id(idSucursal),
      nombre(nombreSucursal),
      ubicacion(ubicacionSucursal),
      tiempoIngreso(tiempoIngresoSucursal),
      tiempoPreparacion(tiempoPreparacionSucursal),
      intervaloDespacho(intervaloDespachoSucursal),
      listaGeneral(new ListaEnlazada()),
      listaOrdenada(new ListaEnlazadaOrdenada()),
      arbolAVL(new ArbolAVL()),
      arbolB(new ArbolB()),
      arbolBPlus(new ArbolBPlus()),
      tablaHash(new TablaHash()),
      colaIngreso(new Cola()),
      colaPreparacion(new Cola()),
      colaSalida(new Cola()),
      pilaRollback(new Pila()) {}

Sucursal::~Sucursal() {
    delete listaGeneral;
    delete listaOrdenada;
    delete arbolAVL;
    delete arbolB;
    delete arbolBPlus;
    delete tablaHash;
    delete colaIngreso;
    delete colaPreparacion;
    delete colaSalida;
    delete pilaRollback;
}

bool Sucursal::agregarProducto(Producto* p) {
    if (p == nullptr) {
        return false;
    }

    bool insertadoListaGeneral = false;
    bool insertadoListaOrdenada = false;
    bool insertadoAVL = false;
    bool insertadoB = false;
    bool insertadoBPlus = false;
    bool insertadoHash = false;

    listaGeneral->insertar(p);
    insertadoListaGeneral = true;

    listaOrdenada->insertarOrdenado(p);
    insertadoListaOrdenada = true;

    arbolAVL->insertar(p);
    insertadoAVL = true;

    arbolB->insertar(p);
    insertadoB = true;

    arbolBPlus->insertar(p);
    insertadoBPlus = true;

    insertadoHash = tablaHash->insertar(p);
    if (insertadoHash) {
        return true;
    }

    // Rollback local en orden inverso de inserción.
    if (insertadoBPlus) {
        arbolBPlus->eliminar(p->categoria, p->codigoBarras);
    }
    if (insertadoB) {
        arbolB->eliminar(p->fechaCaducidad, p->codigoBarras);
    }
    if (insertadoAVL) {
        if (!arbolAVL->eliminar(p->nombre)) {
            arbolAVL->eliminarPorCodigoBarras(p->codigoBarras);
        }
    }
    if (insertadoListaOrdenada) {
        if (!listaOrdenada->eliminar(p->nombre)) {
            listaOrdenada->eliminarPorCodigoBarras(p->codigoBarras);
        }
    }
    if (insertadoListaGeneral) {
        listaGeneral->eliminar(p);
    }

    return false;
}

bool Sucursal::eliminarProducto(const std::string& codigoBarra) {
    if (codigoBarra.empty()) {
        return false;
    }

    Producto* productoOriginal = tablaHash->buscarPorCodigoBarras(codigoBarra);
    if (productoOriginal == nullptr) {
        return false;
    }

    Producto* respaldo = new Producto(productoOriginal->nombre,
                                      productoOriginal->codigoBarras,
                                      productoOriginal->categoria,
                                      productoOriginal->fechaCaducidad,
                                      productoOriginal->marca,
                                      productoOriginal->precio,
                                      productoOriginal->stock);

    bool eliminadoHash = false;
    bool eliminadoAVL = false;
    bool eliminadoB = false;
    bool eliminadoBPlus = false;
    bool eliminadoLista = false;
    bool eliminadoListaOrdenada = false;

    auto restaurarDesdeRespaldo = [&]() {
        productoOriginal->nombre = respaldo->nombre;
        productoOriginal->codigoBarras = respaldo->codigoBarras;
        productoOriginal->categoria = respaldo->categoria;
        productoOriginal->fechaCaducidad = respaldo->fechaCaducidad;
        productoOriginal->marca = respaldo->marca;
        productoOriginal->precio = respaldo->precio;
        productoOriginal->stock = respaldo->stock;
    };

    auto rollback = [&]() {
        restaurarDesdeRespaldo();

        if (eliminadoListaOrdenada) {
            listaOrdenada->insertarOrdenado(productoOriginal);
        }
        if (eliminadoLista) {
            listaGeneral->insertar(productoOriginal);
        }
        if (eliminadoBPlus) {
            arbolBPlus->insertar(productoOriginal);
        }
        if (eliminadoB) {
            arbolB->insertar(productoOriginal);
        }
        if (eliminadoAVL) {
            arbolAVL->insertar(productoOriginal);
        }
        if (eliminadoHash) {
            tablaHash->insertar(productoOriginal);
        }
    };

    eliminadoHash = tablaHash->eliminar(codigoBarra);
    if (!eliminadoHash) {
        delete respaldo;
        return false;
    }

    eliminadoAVL = arbolAVL->eliminar(productoOriginal->nombre);
    if (!eliminadoAVL) {
        eliminadoAVL = arbolAVL->eliminarPorCodigoBarras(codigoBarra);
    }
    if (!eliminadoAVL) {
        rollback();
        delete respaldo;
        return false;
    }

    eliminadoB = arbolB->eliminar(productoOriginal->fechaCaducidad, codigoBarra);
    if (!eliminadoB) {
        rollback();
        delete respaldo;
        return false;
    }

    eliminadoBPlus = arbolBPlus->eliminar(productoOriginal->categoria, codigoBarra);
    if (!eliminadoBPlus) {
        rollback();
        delete respaldo;
        return false;
    }

    eliminadoLista = listaGeneral->eliminar(productoOriginal);
    if (!eliminadoLista) {
        rollback();
        delete respaldo;
        return false;
    }

    eliminadoListaOrdenada = listaOrdenada->eliminar(productoOriginal->nombre);
    if (!eliminadoListaOrdenada) {
        eliminadoListaOrdenada = listaOrdenada->eliminarPorCodigoBarras(codigoBarra);
    }
    if (!eliminadoListaOrdenada) {
        rollback();
        delete respaldo;
        return false;
    }

    delete productoOriginal;
    delete respaldo;
    return true;
}

Producto* Sucursal::buscarPorCodigo(const std::string& codigo) {
    return tablaHash->buscarPorCodigoBarras(codigo);
}

Producto* Sucursal::buscarPorNombre(const std::string& nombreProducto) {
    return arbolAVL->buscarPorNombre(nombreProducto);
}

std::vector<Producto*> Sucursal::buscarPorCategoria(const std::string& categoriaProducto) {
    return arbolBPlus->obtenerPorCategoria(categoriaProducto);
}

std::vector<Producto*> Sucursal::buscarPorRangoFecha(const std::string& inicio,
                                                     const std::string& fin) {
    return arbolB->obtenerPorRango(inicio, fin);
}

void Sucursal::encolarIngreso(Producto* p) {
    colaIngreso->encolar(p);
}

void Sucursal::encolarPreparacion(Producto* p) {
    colaPreparacion->encolar(p);
}

void Sucursal::encolarSalida(Producto* p) {
    colaSalida->encolar(p);
}

Producto* Sucursal::desencolarIngreso() {
    return colaIngreso->desencolar();
}

Producto* Sucursal::desencolarPreparacion() {
    return colaPreparacion->desencolar();
}

Producto* Sucursal::desencolarSalida() {
    return colaSalida->desencolar();
}

void Sucursal::pushRollback(Producto* p) {
    pilaRollback->push(p);
}

Producto* Sucursal::popRollback() {
    return pilaRollback->pop();
}

int Sucursal::obtenerId() const {
    return id;
}

const std::string& Sucursal::obtenerNombre() const {
    return nombre;
}

const std::string& Sucursal::obtenerUbicacion() const {
    return ubicacion;
}

int Sucursal::obtenerTiempoIngreso() const {
    return tiempoIngreso;
}

int Sucursal::obtenerTiempoPreparacion() const {
    return tiempoPreparacion;
}

int Sucursal::obtenerIntervaloDespacho() const {
    return intervaloDespacho;
}

ListaEnlazada* Sucursal::obtenerListaGeneral() const {
    return listaGeneral;
}

ListaEnlazadaOrdenada* Sucursal::obtenerListaOrdenada() const {
    return listaOrdenada;
}

ArbolAVL* Sucursal::obtenerArbolAVL() const {
    return arbolAVL;
}

ArbolB* Sucursal::obtenerArbolB() const {
    return arbolB;
}

ArbolBPlus* Sucursal::obtenerArbolBPlus() const {
    return arbolBPlus;
}

TablaHash* Sucursal::obtenerTablaHash() const {
    return tablaHash;
}

Cola* Sucursal::obtenerColaIngreso() const {
    return colaIngreso;
}

Cola* Sucursal::obtenerColaPreparacion() const {
    return colaPreparacion;
}

Cola* Sucursal::obtenerColaSalida() const {
    return colaSalida;
}

Pila* Sucursal::obtenerPilaRollback() const {
    return pilaRollback;
}
