#include "VisorGrafo.h"

#include <QBrush>
#include <QColor>
#include <QGraphicsTextItem>
#include <QPainter>
#include <QPen>
#include <QWheelEvent>

#include <cmath>

VisorGrafo::VisorGrafo(QWidget* parent)
    : QFrame(parent),
      layoutPrincipal(nullptr),
      vistaGrafo(nullptr),
      escenaGrafo(nullptr) {
    setObjectName("frameGrafo");
    setMinimumHeight(260);

    layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(8, 8, 8, 8);
    layoutPrincipal->setSpacing(0);

    escenaGrafo = new QGraphicsScene(this);
    vistaGrafo = new QGraphicsView(escenaGrafo, this);
    vistaGrafo->setObjectName("vistaGrafo");
    vistaGrafo->setRenderHint(QPainter::Antialiasing);
    vistaGrafo->setRenderHint(QPainter::TextAntialiasing);
    vistaGrafo->setMinimumHeight(210);

    // Habilitar navegación con arrastre y transformación bajo el ratón
    vistaGrafo->setDragMode(QGraphicsView::ScrollHandDrag);
    vistaGrafo->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    // Instalar filtro de eventos en vistaGrafo para capturar zoom con rueda
    vistaGrafo->installEventFilter(this);

    layoutPrincipal->addWidget(vistaGrafo, 1);
}

VisorGrafo::~VisorGrafo() {
    limpiar();
}

void VisorGrafo::limpiar() {
    if (escenaGrafo != nullptr) {
        escenaGrafo->clear();
    }
}

// eventFilter: Captura eventos de rueda del ratón para implementar zoom.
// Complejidad: O(1) - solo realiza transformación de escala.
bool VisorGrafo::eventFilter(QObject* obj, QEvent* event) {
    if (obj == vistaGrafo && event->type() == QEvent::Wheel) {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        const double scaleFactor = 1.15;
        
        if (wheelEvent->angleDelta().y() > 0) {
            vistaGrafo->scale(scaleFactor, scaleFactor);
        } else {
            vistaGrafo->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
        }
        return true;
    }
    return QFrame::eventFilter(obj, event);
}

// dibujarGrafo: Renderiza nodos y aristas del grafo con una distribución circular.
// Complejidad: O(V + E), donde V es la cantidad de nodos activos y E la cantidad de aristas.
void VisorGrafo::dibujarGrafo(GrafoSucursales* grafo, int idSeleccionado) {
    limpiar();

    if (grafo == nullptr || escenaGrafo == nullptr || vistaGrafo == nullptr) {
        return;
    }

    const int capacidad = grafo->obtenerCapacidadActual();
    if (capacidad <= 0) {
        escenaGrafo->addText("No hay sucursales registradas.");
        return;
    }

    int cantidadNodos = 0;
    for (int id = 0; id < capacidad; ++id) {
        if (grafo->existeSucursal(id)) {
            ++cantidadNodos;
        }
    }

    if (cantidadNodos == 0) {
        escenaGrafo->addText("No hay sucursales registradas.");
        return;
    }

    int* ids = new int[cantidadNodos];
    int* indicePorId = new int[capacidad];
    qreal* posicionesX = new qreal[cantidadNodos];
    qreal* posicionesY = new qreal[cantidadNodos];

    for (int i = 0; i < capacidad; ++i) {
        indicePorId[i] = -1;
    }

    int indice = 0;
    for (int id = 0; id < capacidad; ++id) {
        if (grafo->existeSucursal(id)) {
            ids[indice] = id;
            indicePorId[id] = indice;
            ++indice;
        }
    }

    const qreal anchoVista = vistaGrafo->viewport()->width() > 0
                                 ? static_cast<qreal>(vistaGrafo->viewport()->width())
                                 : 760.0;
    const qreal altoVista = vistaGrafo->viewport()->height() > 0
                                ? static_cast<qreal>(vistaGrafo->viewport()->height())
                                : 300.0;
    const qreal centroX = 0.0;
    const qreal centroY = 0.0;
    const qreal radioNodo = 18.0;
    const qreal separacionMinima = 18.0;

    const qreal pi = 3.14159265358979323846;
    const qreal circunferenciaMinima =
        static_cast<qreal>(cantidadNodos) * ((radioNodo * 2.0) + separacionMinima);
    const qreal radioPorCantidad = circunferenciaMinima / (2.0 * pi);
    qreal radioBase = ((anchoVista < altoVista ? anchoVista : altoVista) / 2.0) - 120.0;
    if (radioBase < 90.0) {
        radioBase = 90.0;
    }
    // El radio final privilegia una distribución amplia para evitar que las etiquetas
    // de las aristas queden superpuestas con los nodos.
    const qreal radioLayout = (radioPorCantidad > radioBase) ? radioPorCantidad : radioBase;

    for (int i = 0; i < cantidadNodos; ++i) {
        if (cantidadNodos == 1) {
            posicionesX[i] = centroX;
            posicionesY[i] = centroY;
            continue;
        }
        const qreal angulo = (2.0 * pi * static_cast<qreal>(i)) /
                             static_cast<qreal>(cantidadNodos);
        posicionesX[i] = centroX + radioLayout * std::cos(angulo);
        posicionesY[i] = centroY + radioLayout * std::sin(angulo);
    }

    QPen penArista(QColor("#B0BEC5"));
    penArista.setWidth(2);
    penArista.setCapStyle(Qt::RoundCap);

    for (int i = 0; i < cantidadNodos; ++i) {
        const int origenId = ids[i];
        GrafoSucursales::Arista* arista = grafo->obtenerAristas(origenId);

        while (arista != nullptr) {
            if (arista->destinoId >= 0 && arista->destinoId < capacidad) {
                const int indiceDestino = indicePorId[arista->destinoId];
                if (indiceDestino >= 0) {
                    const qreal x1 = posicionesX[i];
                    const qreal y1 = posicionesY[i];
                    const qreal x2 = posicionesX[indiceDestino];
                    const qreal y2 = posicionesY[indiceDestino];

                    escenaGrafo->addLine(x1, y1, x2, y2, penArista);

                    const qreal medioX = (x1 + x2) / 2.0;
                    const qreal medioY = (y1 + y2) / 2.0;
                    QString etiqueta = QString("t:%1 c:%2")
                                           .arg(arista->tiempo)
                                           .arg(arista->costo);
                    QGraphicsTextItem* texto = escenaGrafo->addText(etiqueta);
                    texto->setDefaultTextColor(QColor("#424242"));
                    texto->setPos(medioX - 26.0, medioY - 18.0);
                }
            }
            arista = arista->siguiente;
        }
    }

    QPen penNodo(QColor("#102A43"));
    penNodo.setWidth(2);

    for (int i = 0; i < cantidadNodos; ++i) {
        QColor colorRelleno = (ids[i] == idSeleccionado)
                                  ? QColor("#A50044")
                                  : QColor("#004D98");

        escenaGrafo->addEllipse(posicionesX[i] - radioNodo,
                                posicionesY[i] - radioNodo,
                                radioNodo * 2.0,
                                radioNodo * 2.0,
                                penNodo,
                                QBrush(colorRelleno));

        QGraphicsTextItem* textoNodo = escenaGrafo->addText(QString::number(ids[i]));
        textoNodo->setDefaultTextColor(Qt::white);
        textoNodo->setPos(posicionesX[i] - 8.0, posicionesY[i] - 12.0);
    }

    const QRectF limites = escenaGrafo->itemsBoundingRect();
    escenaGrafo->setSceneRect(limites.adjusted(-80.0, -80.0, 80.0, 80.0));
    vistaGrafo->fitInView(escenaGrafo->sceneRect(), Qt::KeepAspectRatio);

    delete[] posicionesY;
    delete[] posicionesX;
    delete[] indicePorId;
    delete[] ids;
}
