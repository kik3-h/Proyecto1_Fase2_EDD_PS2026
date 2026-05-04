#include "CargadorCSV.h"

#include <cerrno>
#include <cstdlib>
#include <iostream>

#include "Producto.h"
#include "Sucursal.h"

bool CargadorCSV::cargar(const std::string& rutaArchivo,
                         ListaEnlazada& listaNormal,
                         ListaEnlazadaOrdenada& listaOrdenada,
                         TablaHash& tabla,
                         ArbolAVL& arbolAVL,
                         ArbolB& arbolB,
                         ArbolBPlus& arbolBPlus,
                         int idSucursal) {
    std::ifstream archivoCSV(rutaArchivo);
    if (!archivoCSV.is_open()) {
        std::cout << "Error: no se pudo abrir el archivo '" << rutaArchivo << "'.\n";
        return false;
    }

    std::ofstream archivoErrores("errors.log", std::ios::app);
    if (!archivoErrores.is_open()) {
        std::cout << "Error: no se pudo abrir 'errors.log' para registrar errores.\n";
        return false;
    }

    std::string linea;
    int numeroLinea = 0;
    int totalInsertados = 0;
    int totalErrores = 0;

    while (std::getline(archivoCSV, linea)) {
        ++numeroLinea;

        if (linea.empty()) {
            registrarError(archivoErrores, numeroLinea, "Linea vacia", linea);
            ++totalErrores;
            continue;
        }

        std::string campos[8];
        int cantidadCampos = 0;
        if (!dividirLineaCSV(linea, campos, 8, cantidadCampos)) {
            registrarError(archivoErrores, numeroLinea, "Linea malformada", linea);
            ++totalErrores;
            continue;
        }

        if (cantidadCampos != 7 && cantidadCampos != 8) {
            registrarError(archivoErrores, numeroLinea, "Cantidad de columnas invalida", linea);
            ++totalErrores;
            continue;
        }

        if (esCabeceraProductos(campos, cantidadCampos)) {
            continue;
        }

        int idFilaSucursal = idSucursal;
        int desplazamiento = 0;
        if (cantidadCampos == 8) {
            if (!convertirEntero(campos[0], idFilaSucursal)) {
                registrarError(archivoErrores, numeroLinea, "SucursalID invalido", linea);
                ++totalErrores;
                continue;
            }
            desplazamiento = 1;
        }

        if (idFilaSucursal != idSucursal) {
            continue;
        }

        const std::string& nombre = campos[0 + desplazamiento];
        const std::string& codigo = campos[1 + desplazamiento];
        const std::string& categoria = campos[2 + desplazamiento];
        const std::string& fecha = campos[3 + desplazamiento];
        const std::string& marca = campos[4 + desplazamiento];
        const std::string& precioTexto = campos[5 + desplazamiento];
        const std::string& stockTexto = campos[6 + desplazamiento];

        if (tabla.buscarPorCodigoBarras(codigo) != nullptr) {
            registrarError(archivoErrores, numeroLinea, "Codigo de barras duplicado", linea);
            ++totalErrores;
            continue;
        }

        double precio = 0.0;
        int stock = 0;
        if (!convertirDecimal(precioTexto, precio) || !convertirEntero(stockTexto, stock)) {
            registrarError(archivoErrores, numeroLinea, "Precio o stock no numerico", linea);
            ++totalErrores;
            continue;
        }

        Producto* nuevoProducto = new Producto(nombre, codigo, categoria, fecha, marca, precio, stock);

        listaNormal.insertar(nuevoProducto);
        listaOrdenada.insertarOrdenado(nuevoProducto);

        if (!tabla.insertar(nuevoProducto)) {
            listaNormal.eliminarPorCodigoBarras(nuevoProducto->codigoBarras);
            listaOrdenada.eliminarPorCodigoBarras(nuevoProducto->codigoBarras);
            registrarError(archivoErrores, numeroLinea, "Fallo insercion en tabla hash", linea);
            delete nuevoProducto;
            ++totalErrores;
            continue;
        }

        arbolAVL.insertar(nuevoProducto);
        arbolB.insertar(nuevoProducto);
        arbolBPlus.insertar(nuevoProducto);

        ++totalInsertados;
    }

    std::cout << "Carga de productos finalizada. Insertados: " << totalInsertados
              << ", errores: " << totalErrores << ".\n";
    return true;
}

bool CargadorCSV::cargarSucursales(const std::string& ruta,
                                   ListaSucursales& lista,
                                   GrafoSucursales& grafo) {
    std::ifstream archivoCSV(ruta);
    if (!archivoCSV.is_open()) {
        std::cout << "Error: no se pudo abrir el archivo '" << ruta << "'.\n";
        return false;
    }

    std::ofstream archivoErrores("errors.log", std::ios::app);
    if (!archivoErrores.is_open()) {
        std::cout << "Error: no se pudo abrir 'errors.log' para registrar errores.\n";
        return false;
    }

    std::string linea;
    int numeroLinea = 0;
    int totalInsertadas = 0;
    int totalErrores = 0;

    while (std::getline(archivoCSV, linea)) {
        ++numeroLinea;

        if (linea.empty()) {
            registrarError(archivoErrores, numeroLinea, "Linea vacia", linea);
            ++totalErrores;
            continue;
        }

        std::string campos[6];
        int cantidadCampos = 0;
        if (!dividirLineaCSV(linea, campos, 6, cantidadCampos) || cantidadCampos != 6) {
            registrarError(archivoErrores, numeroLinea, "Linea malformada", linea);
            ++totalErrores;
            continue;
        }

        if (esCabeceraSucursales(campos, cantidadCampos)) {
            continue;
        }

        int id = -1;
        int tiempoIngreso = 0;
        int tiempoPreparacion = 0;
        int intervaloDespacho = 0;

        if (!convertirEntero(campos[0], id) ||
            !convertirEntero(campos[3], tiempoIngreso) ||
            !convertirEntero(campos[4], tiempoPreparacion) ||
            !convertirEntero(campos[5], intervaloDespacho)) {
            registrarError(archivoErrores, numeroLinea, "Campos numericos invalidos", linea);
            ++totalErrores;
            continue;
        }

        if (id < 0 || tiempoIngreso < 0 || tiempoPreparacion < 0 || intervaloDespacho < 0) {
            registrarError(archivoErrores, numeroLinea, "Valores negativos no permitidos", linea);
            ++totalErrores;
            continue;
        }

        if (lista.buscar(id) != nullptr) {
            registrarError(archivoErrores, numeroLinea, "Sucursal duplicada", linea);
            ++totalErrores;
            continue;
        }

        Sucursal* nuevaSucursal = new Sucursal(id,
                                               campos[1],
                                               campos[2],
                                               tiempoIngreso,
                                               tiempoPreparacion,
                                               intervaloDespacho);
        lista.agregar(nuevaSucursal);
        grafo.agregarSucursal(id);
        ++totalInsertadas;
    }

    std::cout << "Carga de sucursales finalizada. Insertadas: " << totalInsertadas
              << ", errores: " << totalErrores << ".\n";
    return true;
}

bool CargadorCSV::cargarConexiones(const std::string& ruta, GrafoSucursales& grafo) {
    std::ifstream archivoCSV(ruta);
    if (!archivoCSV.is_open()) {
        std::cout << "Error: no se pudo abrir el archivo '" << ruta << "'.\n";
        return false;
    }

    std::ofstream archivoErrores("errors.log", std::ios::app);
    if (!archivoErrores.is_open()) {
        std::cout << "Error: no se pudo abrir 'errors.log' para registrar errores.\n";
        return false;
    }

    std::string linea;
    int numeroLinea = 0;
    int totalInsertadas = 0;
    int totalErrores = 0;

    while (std::getline(archivoCSV, linea)) {
        ++numeroLinea;

        if (linea.empty()) {
            registrarError(archivoErrores, numeroLinea, "Linea vacia", linea);
            ++totalErrores;
            continue;
        }

        std::string campos[4];
        int cantidadCampos = 0;
        if (!dividirLineaCSV(linea, campos, 4, cantidadCampos) || cantidadCampos != 4) {
            registrarError(archivoErrores, numeroLinea, "Linea malformada", linea);
            ++totalErrores;
            continue;
        }

        if (esCabeceraConexiones(campos, cantidadCampos)) {
            continue;
        }

        int origen = -1;
        int destino = -1;
        int tiempo = -1;
        int costo = -1;
        if (!convertirEntero(campos[0], origen) ||
            !convertirEntero(campos[1], destino) ||
            !convertirEntero(campos[2], tiempo) ||
            !convertirEntero(campos[3], costo)) {
            registrarError(archivoErrores, numeroLinea, "Campos numericos invalidos", linea);
            ++totalErrores;
            continue;
        }

        if (origen < 0 || destino < 0 || tiempo < 0 || costo < 0) {
            registrarError(archivoErrores, numeroLinea, "Valores negativos no permitidos", linea);
            ++totalErrores;
            continue;
        }

        grafo.agregarConexion(origen, destino, tiempo, costo, true);
        ++totalInsertadas;
    }

    std::cout << "Carga de conexiones finalizada. Insertadas: " << totalInsertadas
              << ", errores: " << totalErrores << ".\n";
    return true;
}

bool CargadorCSV::dividirLineaCSV(const std::string& linea,
                                  std::string* campos,
                                  int maxCampos,
                                  int& cantidadCampos) const {
    if (campos == nullptr || maxCampos <= 0) {
        return false;
    }

    cantidadCampos = 0;
    std::string campoActual;
    bool enComillas = false;

    for (std::size_t i = 0; i < linea.size(); ++i) {
        const char caracter = linea[i];
        if (caracter == '"') {
            enComillas = !enComillas;
            campoActual += caracter;
            continue;
        }

        if (caracter == ',' && !enComillas) {
            if (cantidadCampos >= maxCampos) {
                return false;
            }
            campos[cantidadCampos] = limpiarCampo(campoActual);
            campoActual.clear();
            ++cantidadCampos;
            continue;
        }

        campoActual += caracter;
    }

    if (enComillas) {
        return false;
    }

    if (cantidadCampos >= maxCampos) {
        return false;
    }

    campos[cantidadCampos] = limpiarCampo(campoActual);
    ++cantidadCampos;
    return true;
}

bool CargadorCSV::convertirEntero(const std::string& texto, int& salida) const {
    if (texto.empty()) {
        return false;
    }

    char* fin = nullptr;
    errno = 0;
    const long valor = std::strtol(texto.c_str(), &fin, 10);
    if (errno != 0 || fin == texto.c_str() || *fin != '\0') {
        return false;
    }

    salida = static_cast<int>(valor);
    return true;
}

bool CargadorCSV::convertirDecimal(const std::string& texto, double& salida) const {
    if (texto.empty()) {
        return false;
    }

    char* fin = nullptr;
    errno = 0;
    const double valor = std::strtod(texto.c_str(), &fin);
    if (errno != 0 || fin == texto.c_str() || *fin != '\0') {
        return false;
    }

    salida = valor;
    return true;
}

std::string CargadorCSV::limpiarCampo(const std::string& campo) const {
    if (campo.empty()) {
        return campo;
    }

    std::size_t inicio = 0;
    std::size_t fin = campo.size();

    while (inicio < fin &&
           (campo[inicio] == ' ' || campo[inicio] == '\t' || campo[inicio] == '\r')) {
        ++inicio;
    }

    while (fin > inicio &&
           (campo[fin - 1] == ' ' || campo[fin - 1] == '\t' || campo[fin - 1] == '\r')) {
        --fin;
    }

    if (inicio >= fin) {
        return "";
    }

    std::string limpio = campo.substr(inicio, fin - inicio);
    if (limpio.size() >= 2 && limpio.front() == '"' && limpio.back() == '"') {
        limpio = limpio.substr(1, limpio.size() - 2);
    }
    return limpio;
}

std::string CargadorCSV::normalizarTexto(const std::string& texto) const {
    std::string resultado = texto;
    for (char& caracter : resultado) {
        if (caracter >= 'A' && caracter <= 'Z') {
            caracter = static_cast<char>(caracter + ('a' - 'A'));
        } else if (caracter == ' ') {
            caracter = '_';
        }
    }
    return resultado;
}

bool CargadorCSV::esCabeceraProductos(const std::string* campos, int cantidadCampos) const {
    if (campos == nullptr || (cantidadCampos != 7 && cantidadCampos != 8)) {
        return false;
    }

    int offset = 0;
    if (cantidadCampos == 8) {
        const std::string sucursal = normalizarTexto(campos[0]);
        if (sucursal != "sucursalid" && sucursal != "sucursal_id") {
            return false;
        }
        offset = 1;
    }

    const std::string nombre = normalizarTexto(campos[offset + 0]);
    const std::string codigo = normalizarTexto(campos[offset + 1]);
    const std::string categoria = normalizarTexto(campos[offset + 2]);
    const std::string fecha = normalizarTexto(campos[offset + 3]);
    const std::string marca = normalizarTexto(campos[offset + 4]);
    const std::string precio = normalizarTexto(campos[offset + 5]);
    const std::string stock = normalizarTexto(campos[offset + 6]);

    return (nombre == "nombre" &&
            (codigo == "codigobarra" || codigo == "codigo_barra") &&
            categoria == "categoria" &&
            (fecha == "fecha_caducidad" || fecha == "fechacaducidad") &&
            marca == "marca" &&
            precio == "precio" &&
            stock == "stock");
}

bool CargadorCSV::esCabeceraSucursales(const std::string* campos, int cantidadCampos) const {
    if (campos == nullptr || cantidadCampos != 6) {
        return false;
    }

    const std::string c0 = normalizarTexto(campos[0]);
    const std::string c1 = normalizarTexto(campos[1]);
    const std::string c2 = normalizarTexto(campos[2]);
    const std::string c3 = normalizarTexto(campos[3]);
    const std::string c4 = normalizarTexto(campos[4]);
    const std::string c5 = normalizarTexto(campos[5]);

    const bool columnaUbicacion = (c2 == "ubicacion" || c2.find("ubic") != std::string::npos);
    return (c0 == "id" &&
            c1 == "nombre" &&
            columnaUbicacion &&
            (c3 == "t_ingreso" || c3 == "tiempo_ingreso" || c3 == "tingreso") &&
            (c4 == "t_traspaso" || c4 == "tiempo_traspaso" || c4 == "tpreparacion") &&
            (c5 == "t_despacho" || c5 == "intervalo_despacho" || c5 == "tdespacho"));
}

bool CargadorCSV::esCabeceraConexiones(const std::string* campos, int cantidadCampos) const {
    if (campos == nullptr || cantidadCampos != 4) {
        return false;
    }

    const std::string c0 = normalizarTexto(campos[0]);
    const std::string c1 = normalizarTexto(campos[1]);
    const std::string c2 = normalizarTexto(campos[2]);
    const std::string c3 = normalizarTexto(campos[3]);

    return ((c0 == "origenid" || c0 == "origen_id") &&
            (c1 == "destinoid" || c1 == "destino_id") &&
            c2 == "tiempo" &&
            c3 == "costo");
}

void CargadorCSV::registrarError(std::ofstream& archivoErrores,
                                 int numeroLinea,
                                 const std::string& motivo,
                                 const std::string& lineaOriginal) const {
    archivoErrores << "Linea " << numeroLinea << " | " << motivo
                   << " | Contenido: " << lineaOriginal << '\n';
}
