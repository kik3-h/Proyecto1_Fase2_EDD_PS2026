#ifndef VISOR_GRAFO_H
#define VISOR_GRAFO_H

#include <QFrame>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>

#include "GrafoSucursales.h"

// VisorGrafo: Componente visual para dibujar la red de sucursales en una escena 2D.
class VisorGrafo : public QFrame {
public:
    // Construye el visor y configura la escena gráfica base. Complejidad O(1).
    explicit VisorGrafo(QWidget* parent = nullptr);

    // Libera recursos asociados a escena y vista del grafo. Complejidad O(1).
    ~VisorGrafo();

    // Limpia todos los elementos gráficos dibujados actualmente. Complejidad O(k).
    void limpiar();

    // Dibuja el grafo completo usando disposición circular. Complejidad O(V + E).
    void dibujarGrafo(GrafoSucursales* grafo, int idSeleccionado = -1);

private:
    QVBoxLayout* layoutPrincipal;
    QGraphicsView* vistaGrafo;
    QGraphicsScene* escenaGrafo;
};

#endif
