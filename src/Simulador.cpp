#include "Simulador.h"

#include <iostream>

Simulador::Simulador(GrafoSucursales* g, ListaSucursales* s)
    : grafo(g), sucursales(s), eventosPendientes(new Cola()), tickActual(0) {}

Simulador::~Simulador() {
    if (eventosPendientes != nullptr) {
        while (!eventosPendientes->vacia()) {
            EventoTraslado* evento =
                reinterpret_cast<EventoTraslado*>(eventosPendientes->desencolar());
            liberarEvento(evento);
        }

        delete eventosPendientes;
        eventosPendientes = nullptr;
    }

    grafo = nullptr;
    sucursales = nullptr;
}

void Simulador::programarEnvio(Producto* producto, int origen, int destino, bool porTiempo) {
    if (eventosPendientes == nullptr || producto == nullptr || origen < 0 || destino < 0) {
        return;
    }

    EventoTraslado* evento = new EventoTraslado();
    evento->producto = producto;
    evento->idSucursalOrigen = origen;
    evento->idSucursalDestino = destino;
    evento->porTiempo = porTiempo;
    evento->etapa = 0;
    evento->rutaIds = nullptr;
    evento->longitudRuta = 0;
    evento->indiceRutaActual = 0;
    evento->ticksEtapa = 0;
    evento->enViaje = false;
    evento->duracionViaje = 0;
    evento->sucursalActual = nullptr;
    evento->sucursalDestinoFinal = nullptr;

    // La cola existente almacena Producto*. Para reutilizarla como cola de eventos,
    // se encapsula el puntero del evento mediante cast explícito.
    eventosPendientes->encolar(reinterpret_cast<Producto*>(evento));

    std::cout << "[Simulador] Evento programado para producto "
              << producto->codigoBarras << " ("
              << origen << " -> " << destino << ")\n";
}

Sucursal* Simulador::buscarSucursalPorId(int idSucursal) const {
    if (sucursales == nullptr) {
        return nullptr;
    }

    return sucursales->buscar(idSucursal);
}

int Simulador::obtenerPesoConexion(int origen, int destino, bool porTiempo) const {
    if (grafo == nullptr) {
        return -1;
    }

    GrafoSucursales::Arista* arista = grafo->obtenerAristas(origen);
    while (arista != nullptr) {
        if (arista->destinoId == destino) {
            return porTiempo ? arista->tiempo : arista->costo;
        }
        arista = arista->siguiente;
    }

    return -1;
}

void Simulador::liberarEvento(EventoTraslado* evento) {
    if (evento == nullptr) {
        return;
    }

    delete[] evento->rutaIds;
    evento->rutaIds = nullptr;
    evento->longitudRuta = 0;
    delete evento;
}

bool Simulador::procesarEtapaCreacion(EventoTraslado* evento) {
    if (evento == nullptr || grafo == nullptr || evento->producto == nullptr) {
        return false;
    }

    GrafoSucursales::Ruta ruta =
        grafo->rutaMasCorta(evento->idSucursalOrigen, evento->idSucursalDestino, evento->porTiempo);

    if (ruta.nodosRuta == nullptr || ruta.longitudRuta <= 0) {
        std::cout << "[Simulador] No existe ruta para producto "
                  << evento->producto->codigoBarras << " ("
                  << evento->idSucursalOrigen << " -> "
                  << evento->idSucursalDestino << ")\n";
        return false;
    }

    Sucursal* origen = buscarSucursalPorId(evento->idSucursalOrigen);
    Sucursal* destino = buscarSucursalPorId(evento->idSucursalDestino);
    if (origen == nullptr || destino == nullptr) {
        std::cout << "[Simulador] Sucursal inexistente en evento de producto "
                  << evento->producto->codigoBarras << '\n';
        delete[] ruta.nodosRuta;
        return false;
    }

    evento->rutaIds = ruta.nodosRuta;
    evento->longitudRuta = ruta.longitudRuta;
    evento->indiceRutaActual = 0;
    evento->ticksEtapa = 0;
    evento->enViaje = false;
    evento->duracionViaje = 0;
    evento->sucursalActual = origen;
    evento->sucursalDestinoFinal = destino;
    evento->etapa = 1;

    origen->encolarIngreso(evento->producto);
    evento->producto->establecerEstado("EnIngreso");

    std::cout << "[Simulador] Tick " << tickActual
              << ": producto " << evento->producto->codigoBarras
              << " ingreso en sucursal " << origen->obtenerId() << '\n';
    return true;
}

bool Simulador::procesarEtapaIngreso(EventoTraslado* evento) {
    if (evento == nullptr || evento->sucursalActual == nullptr || evento->producto == nullptr) {
        return false;
    }

    Cola* colaIngreso = evento->sucursalActual->obtenerColaIngreso();
    if (colaIngreso == nullptr) {
        return false;
    }

    if (colaIngreso->verFrente() != evento->producto) {
        return true;
    }

    ++evento->ticksEtapa;
    int limiteIngreso = evento->sucursalActual->obtenerTiempoIngreso();
    if (limiteIngreso <= 0) {
        limiteIngreso = 1;
    }

    if (evento->ticksEtapa < limiteIngreso) {
        return true;
    }

    Producto* procesado = evento->sucursalActual->desencolarIngreso();
    if (procesado != evento->producto) {
        if (procesado != nullptr) {
            evento->sucursalActual->encolarIngreso(procesado);
        }
        return true;
    }

    const bool esDestinoFinal =
        (evento->indiceRutaActual >= evento->longitudRuta - 1);

    if (esDestinoFinal) {
        evento->producto->establecerEstado("Disponible");
        std::cout << "[Simulador] Tick " << tickActual
                  << ": producto " << evento->producto->codigoBarras
                  << " disponible en sucursal "
                  << evento->sucursalActual->obtenerId() << '\n';
        return false;
    }

    evento->sucursalActual->encolarPreparacion(evento->producto);
    evento->producto->establecerEstado("EnPreparacion");
    evento->etapa = 2;
    evento->ticksEtapa = 0;

    std::cout << "[Simulador] Tick " << tickActual
              << ": producto " << evento->producto->codigoBarras
              << " en preparacion en sucursal "
              << evento->sucursalActual->obtenerId() << '\n';
    return true;
}

bool Simulador::procesarEtapaPreparacion(EventoTraslado* evento) {
    if (evento == nullptr || evento->sucursalActual == nullptr || evento->producto == nullptr) {
        return false;
    }

    Cola* colaPreparacion = evento->sucursalActual->obtenerColaPreparacion();
    if (colaPreparacion == nullptr) {
        return false;
    }

    if (colaPreparacion->verFrente() != evento->producto) {
        return true;
    }

    ++evento->ticksEtapa;
    int limitePreparacion = evento->sucursalActual->obtenerTiempoPreparacion();
    if (limitePreparacion <= 0) {
        limitePreparacion = 1;
    }

    if (evento->ticksEtapa < limitePreparacion) {
        return true;
    }

    Producto* procesado = evento->sucursalActual->desencolarPreparacion();
    if (procesado != evento->producto) {
        if (procesado != nullptr) {
            evento->sucursalActual->encolarPreparacion(procesado);
        }
        return true;
    }

    evento->sucursalActual->encolarSalida(evento->producto);
    evento->producto->establecerEstado("EnSalida");
    evento->etapa = 3;
    evento->ticksEtapa = 0;
    evento->enViaje = false;
    evento->duracionViaje = 0;

    std::cout << "[Simulador] Tick " << tickActual
              << ": producto " << evento->producto->codigoBarras
              << " en salida en sucursal "
              << evento->sucursalActual->obtenerId() << '\n';
    return true;
}

bool Simulador::procesarEtapaSalidaYTransito(EventoTraslado* evento) {
    if (evento == nullptr || evento->sucursalActual == nullptr || evento->producto == nullptr) {
        return false;
    }

    if (!evento->enViaje) {
        Cola* colaSalida = evento->sucursalActual->obtenerColaSalida();
        if (colaSalida == nullptr) {
            return false;
        }

        if (colaSalida->verFrente() != evento->producto) {
            return true;
        }

        ++evento->ticksEtapa;
        int limiteDespacho = evento->sucursalActual->obtenerIntervaloDespacho();
        if (limiteDespacho <= 0) {
            limiteDespacho = 1;
        }

        if (evento->ticksEtapa < limiteDespacho) {
            return true;
        }

        Producto* despachado = evento->sucursalActual->desencolarSalida();
        if (despachado != evento->producto) {
            if (despachado != nullptr) {
                evento->sucursalActual->encolarSalida(despachado);
            }
            return true;
        }

        if (evento->indiceRutaActual + 1 >= evento->longitudRuta) {
            evento->producto->establecerEstado("Disponible");
            std::cout << "[Simulador] Tick " << tickActual
                      << ": producto " << evento->producto->codigoBarras
                      << " disponible sin salto adicional\n";
            return false;
        }

        const int origenActual = evento->rutaIds[evento->indiceRutaActual];
        const int siguienteNodo = evento->rutaIds[evento->indiceRutaActual + 1];
        int duracion = obtenerPesoConexion(origenActual, siguienteNodo, evento->porTiempo);
        if (duracion <= 0) {
            duracion = 1;
        }

        evento->enViaje = true;
        evento->ticksEtapa = 0;
        evento->duracionViaje = duracion;
        evento->producto->establecerEstado("EnTransito");

        std::cout << "[Simulador] Tick " << tickActual
                  << ": producto " << evento->producto->codigoBarras
                  << " en transito (" << origenActual << " -> " << siguienteNodo
                  << ", duracion " << duracion << ")\n";
        return true;
    }

    ++evento->ticksEtapa;
    if (evento->ticksEtapa < evento->duracionViaje) {
        return true;
    }

    ++evento->indiceRutaActual;
    const int idLlegada = evento->rutaIds[evento->indiceRutaActual];
    Sucursal* sucursalLlegada = buscarSucursalPorId(idLlegada);
    if (sucursalLlegada == nullptr) {
        std::cout << "[Simulador] Error: sucursal de llegada inexistente para producto "
                  << evento->producto->codigoBarras << '\n';
        return false;
    }

    evento->sucursalActual = sucursalLlegada;
    evento->enViaje = false;
    evento->ticksEtapa = 0;
    evento->duracionViaje = 0;
    evento->etapa = 1;

    sucursalLlegada->encolarIngreso(evento->producto);
    evento->producto->establecerEstado("EnIngreso");

    std::cout << "[Simulador] Tick " << tickActual
              << ": producto " << evento->producto->codigoBarras
              << " llego a sucursal " << idLlegada
              << " y entro a cola de ingreso\n";
    return true;
}

bool Simulador::procesarEvento(EventoTraslado* evento) {
    if (evento == nullptr) {
        return false;
    }

    switch (evento->etapa) {
        case 0:
            return procesarEtapaCreacion(evento);
        case 1:
            return procesarEtapaIngreso(evento);
        case 2:
            return procesarEtapaPreparacion(evento);
        case 3:
            return procesarEtapaSalidaYTransito(evento);
        default:
            return false;
    }
}

void Simulador::avanzar() {
    if (eventosPendientes == nullptr) {
        return;
    }

    ++tickActual;

    if (eventosPendientes->vacia()) {
        return;
    }

    Cola* eventosSiguienteTick = new Cola();

    while (!eventosPendientes->vacia()) {
        EventoTraslado* evento =
            reinterpret_cast<EventoTraslado*>(eventosPendientes->desencolar());

        const bool sigueActivo = procesarEvento(evento);
        if (sigueActivo) {
            eventosSiguienteTick->encolar(reinterpret_cast<Producto*>(evento));
        } else {
            liberarEvento(evento);
        }
    }

    delete eventosPendientes;
    eventosPendientes = eventosSiguienteTick;
}
