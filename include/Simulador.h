#ifndef SIMULADOR_H
#define SIMULADOR_H

#include "Cola.h"
#include "GrafoSucursales.h"
#include "ListaSucursales.h"

// Simulador: Controla el flujo temporal de traslados entre sucursales.
// Procesa eventos por ticks y respeta colas FIFO de ingreso, preparación y salida.
class Simulador {
public:
    struct EventoTraslado {
        Producto* producto;
        int idSucursalOrigen;
        int idSucursalDestino;
        bool porTiempo;
        int etapa; // 0=creado, 1=ingreso, 2=preparacion, 3=salida/transito

        int* rutaIds;
        int longitudRuta;
        int indiceRutaActual;

        int ticksEtapa;
        bool enViaje;
        int duracionViaje;

        Sucursal* sucursalActual;
        Sucursal* sucursalDestinoFinal;
    };

private:
    GrafoSucursales* grafo;
    ListaSucursales* sucursales;
    Cola* eventosPendientes;
    long long tickActual;
    int factorAceleracion;

    // Busca una sucursal por id en la lista enlazada. Complejidad O(s).
    Sucursal* buscarSucursalPorId(int idSucursal) const;

    // Obtiene el peso de una arista (tiempo o costo) entre dos nodos adyacentes. O(grado).
    int obtenerPesoConexion(int origen, int destino, bool porTiempo) const;

    // Libera memoria interna de un evento sin liberar el producto asociado. O(r).
    void liberarEvento(EventoTraslado* evento);

    // Procesa un evento recién programado y lo mueve a cola de ingreso. O((V+E) log V).
    bool procesarEtapaCreacion(EventoTraslado* evento);

    // Procesa tiempo de cola de ingreso y transición a preparación o final. O(1).
    bool procesarEtapaIngreso(EventoTraslado* evento);

    // Procesa tiempo de preparación y transición a cola de salida. O(1).
    bool procesarEtapaPreparacion(EventoTraslado* evento);

    // Procesa despacho y tiempo de viaje hacia la siguiente sucursal. O(grado).
    bool procesarEtapaSalidaYTransito(EventoTraslado* evento);

    // Procesa una máquina de estados completa para un evento. O((V+E) log V) en etapa 0, O(1) en las demás.
    bool procesarEvento(EventoTraslado* evento);

public:
    // Inicializa el simulador con referencias al grafo y lista de sucursales. O(1).
    Simulador(GrafoSucursales* g, ListaSucursales* s);

    // Libera cola de eventos y memoria auxiliar de rutas, sin destruir productos. O(e + r).
    ~Simulador();

    // Ajusta el factor de aceleracion de la simulacion. Complejidad O(1).
    void establecerFactorAceleracion(int nuevoFactor);

    // Crea un evento de traslado y lo coloca en la cola de pendientes. O(1).
    void programarEnvio(Producto* producto, int origen, int destino, bool porTiempo);

    // Avanza la simulación un tick y procesa todos los eventos activos. O(e).
    void avanzar();
};

#endif
