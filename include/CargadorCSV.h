#ifndef CARGADOR_CSV_H
#define CARGADOR_CSV_H

#include <fstream>
#include <string>

#include "ArbolAVL.h"
#include "ArbolB.h"
#include "ArbolBPlus.h"
#include "GrafoSucursales.h"
#include "ListaEnlazada.h"
#include "ListaEnlazadaOrdenada.h"
#include "ListaSucursales.h"
#include "TablaHash.h"

class CargadorCSV {
public:
    // Carga productos en estructuras de una sucursal específica. Complejidad O(n).
    bool cargar(const std::string& rutaArchivo,
                ListaEnlazada& listaNormal,
                ListaEnlazadaOrdenada& listaOrdenada,
                TablaHash& tabla,
                ArbolAVL& arbolAVL,
                ArbolB& arbolB,
                ArbolBPlus& arbolBPlus,
                int idSucursal);

    // Carga sucursales y registra nodos en el grafo. Complejidad O(n).
    bool cargarSucursales(const std::string& ruta,
                          ListaSucursales& lista,
                          GrafoSucursales& grafo);

    // Carga conexiones ponderadas entre sucursales. Complejidad O(m).
    bool cargarConexiones(const std::string& ruta, GrafoSucursales& grafo);

private:
    // Divide una línea CSV considerando comillas y cantidad máxima de columnas. O(L).
    bool dividirLineaCSV(const std::string& linea,
                         std::string* campos,
                         int maxCampos,
                         int& cantidadCampos) const;

    // Convierte texto a entero sin excepciones. O(L).
    bool convertirEntero(const std::string& texto, int& salida) const;

    // Convierte texto a decimal sin excepciones. O(L).
    bool convertirDecimal(const std::string& texto, double& salida) const;

    std::string limpiarCampo(const std::string& campo) const;
    std::string normalizarTexto(const std::string& texto) const;
    bool esCabeceraProductos(const std::string* campos, int cantidadCampos) const;
    bool esCabeceraSucursales(const std::string* campos, int cantidadCampos) const;
    bool esCabeceraConexiones(const std::string* campos, int cantidadCampos) const;
    void registrarError(std::ofstream& archivoErrores,
                        int numeroLinea,
                        const std::string& motivo,
                        const std::string& lineaOriginal) const;
};

#endif
