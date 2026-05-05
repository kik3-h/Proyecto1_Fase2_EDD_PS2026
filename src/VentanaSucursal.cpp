#include "VentanaSucursal.h"

#include <QAbstractItemView>
#include <QDialog>
#include <QEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QStringList>
#include <QTableWidget>
#include <QTabWidget>
#include <QTimer>
#include <QWheelEvent>
#include <QVBoxLayout>
#include <QWidget>
#include <iostream>

#include "Cola.h"
#include "Simulador.h"
#include "Sucursal.h"

namespace {
// Crea tabla estandar para representar colas logisticas. Complejidad O(1).
QTableWidget* crearTablaCola(QWidget* parent) {
    QTableWidget* tabla = new QTableWidget(parent);
    tabla->setColumnCount(3);
    QStringList encabezados;
    encabezados << "Codigo" << "Nombre" << "Estado";
    tabla->setHorizontalHeaderLabels(encabezados);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionMode(QAbstractItemView::NoSelection);
    tabla->setAlternatingRowColors(true);
    tabla->horizontalHeader()->setStretchLastSection(true);
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->verticalHeader()->setVisible(false);
    return tabla;
}
}  // namespace

VentanaSucursal::VentanaSucursal(Sucursal* sucursalPtr, Simulador* simuladorPtr, QWidget* parent)
    : QDialog(parent),
      sucursal(sucursalPtr),
      simulador(simuladorPtr),
      pestanas(nullptr),
      lblInfoSucursal(nullptr),
      tablaColaIngreso(nullptr),
      tablaColaPreparacion(nullptr),
      tablaColaSalida(nullptr),
      vistaArbolAVL(nullptr),
      vistaArbolB(nullptr),
      vistaArbolBPlus(nullptr),
      escenaArbolAVL(nullptr),
      escenaArbolB(nullptr),
      escenaArbolBPlus(nullptr),
      btnTransferir(nullptr),
      btnRefrescarColas(nullptr) {
    setWindowTitle("Vista de Sucursal");
    setMinimumSize(1100, 700);
    setModal(true);

    configurarInterfaz();
    aplicarEstilos();
    cargarDatosSucursal();
    refrescarColas();
    cargarArboles();
}

void VentanaSucursal::configurarInterfaz() {
    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(12, 12, 12, 12);
    layoutPrincipal->setSpacing(10);

    lblInfoSucursal = new QLabel(this);
    lblInfoSucursal->setObjectName("lblInfoSucursal");
    lblInfoSucursal->setWordWrap(true);
    layoutPrincipal->addWidget(lblInfoSucursal);

    pestanas = new QTabWidget(this);
    pestanas->setObjectName("pestanasSucursal");
    layoutPrincipal->addWidget(pestanas, 1);

    QWidget* tabColas = new QWidget(pestanas);
    QHBoxLayout* layoutColas = new QHBoxLayout(tabColas);
    layoutColas->setContentsMargins(8, 8, 8, 8);
    layoutColas->setSpacing(10);

    QGroupBox* grupoIngreso = new QGroupBox("Cola de Ingreso", tabColas);
    QVBoxLayout* layoutIngreso = new QVBoxLayout(grupoIngreso);
    tablaColaIngreso = crearTablaCola(grupoIngreso);
    layoutIngreso->addWidget(tablaColaIngreso);

    QGroupBox* grupoPreparacion = new QGroupBox("Cola de Preparacion", tabColas);
    QVBoxLayout* layoutPreparacion = new QVBoxLayout(grupoPreparacion);
    tablaColaPreparacion = crearTablaCola(grupoPreparacion);
    layoutPreparacion->addWidget(tablaColaPreparacion);

    QGroupBox* grupoSalida = new QGroupBox("Cola de Salida", tabColas);
    QVBoxLayout* layoutSalida = new QVBoxLayout(grupoSalida);
    tablaColaSalida = crearTablaCola(grupoSalida);
    layoutSalida->addWidget(tablaColaSalida);

    layoutColas->addWidget(grupoIngreso, 1);
    layoutColas->addWidget(grupoPreparacion, 1);
    layoutColas->addWidget(grupoSalida, 1);
    pestanas->addTab(tabColas, "Colas");

    QWidget* tabArboles = new QWidget(pestanas);
    QVBoxLayout* layoutArboles = new QVBoxLayout(tabArboles);
    layoutArboles->setContentsMargins(8, 8, 8, 8);
    layoutArboles->setSpacing(8);

    QTabWidget* pestanasArboles = new QTabWidget(tabArboles);
    vistaArbolAVL = new QGraphicsView(pestanasArboles);
    vistaArbolB = new QGraphicsView(pestanasArboles);
    vistaArbolBPlus = new QGraphicsView(pestanasArboles);
    escenaArbolAVL = new QGraphicsScene(vistaArbolAVL);
    escenaArbolB = new QGraphicsScene(vistaArbolB);
    escenaArbolBPlus = new QGraphicsScene(vistaArbolBPlus);

    vistaArbolAVL->setRenderHint(QPainter::Antialiasing);
    vistaArbolB->setRenderHint(QPainter::Antialiasing);
    vistaArbolBPlus->setRenderHint(QPainter::Antialiasing);
    vistaArbolAVL->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    vistaArbolB->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    vistaArbolBPlus->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    vistaArbolAVL->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    vistaArbolB->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    vistaArbolBPlus->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    vistaArbolAVL->setDragMode(QGraphicsView::ScrollHandDrag);
    vistaArbolB->setDragMode(QGraphicsView::ScrollHandDrag);
    vistaArbolBPlus->setDragMode(QGraphicsView::ScrollHandDrag);
    vistaArbolAVL->setScene(escenaArbolAVL);
    vistaArbolB->setScene(escenaArbolB);
    vistaArbolBPlus->setScene(escenaArbolBPlus);
    vistaArbolAVL->viewport()->installEventFilter(this);
    vistaArbolB->viewport()->installEventFilter(this);
    vistaArbolBPlus->viewport()->installEventFilter(this);

    pestanasArboles->addTab(vistaArbolAVL, "AVL");
    pestanasArboles->addTab(vistaArbolB, "B");
    pestanasArboles->addTab(vistaArbolBPlus, "B+");
    layoutArboles->addWidget(pestanasArboles);
    pestanas->addTab(tabArboles, "Arboles");

    QWidget* tabDetalles = new QWidget(pestanas);
    QVBoxLayout* layoutDetalles = new QVBoxLayout(tabDetalles);
    layoutDetalles->setContentsMargins(12, 12, 12, 12);
    layoutDetalles->setSpacing(10);

    QLabel* lblTiempos = new QLabel(tabDetalles);
    lblTiempos->setObjectName("lblTiemposSucursal");
    if (sucursal != nullptr) {
        lblTiempos->setText(
            QString("Tiempos configurados\nIngreso: %1 s\nPreparacion: %2 s\nDespacho: %3 s")
                .arg(sucursal->obtenerTiempoIngreso())
                .arg(sucursal->obtenerTiempoPreparacion())
                .arg(sucursal->obtenerIntervaloDespacho()));
    } else {
        lblTiempos->setText("Sucursal no disponible.");
    }

    btnTransferir = new QPushButton("Transferir Producto", tabDetalles);
    btnTransferir->setObjectName("btnTransferirProducto");
    btnTransferir->setCursor(Qt::PointingHandCursor);

    btnRefrescarColas = new QPushButton("Refrescar Colas", tabDetalles);
    btnRefrescarColas->setObjectName("btnRefrescarColas");
    btnRefrescarColas->setCursor(Qt::PointingHandCursor);

    layoutDetalles->addWidget(lblTiempos);
    layoutDetalles->addWidget(btnTransferir, 0, Qt::AlignLeft);
    layoutDetalles->addWidget(btnRefrescarColas, 0, Qt::AlignLeft);
    layoutDetalles->addStretch();
    pestanas->addTab(tabDetalles, "Detalles");

    QPushButton* btnCerrar = new QPushButton("Cerrar", this);
    btnCerrar->setObjectName("btnCerrarSucursal");
    btnCerrar->setCursor(Qt::PointingHandCursor);
    layoutPrincipal->addWidget(btnCerrar, 0, Qt::AlignRight);

    connect(btnTransferir, &QPushButton::clicked, this, &VentanaSucursal::onTransferirProducto);
    connect(btnRefrescarColas, &QPushButton::clicked, this, &VentanaSucursal::refrescarColas);
    connect(btnCerrar, &QPushButton::clicked, this, &QDialog::accept);
}

void VentanaSucursal::aplicarEstilos() {
    const QString estilos = R"(
        QDialog {
            background-color: #f6f8fb;
        }
        #lblInfoSucursal {
            color: #004D98;
            font-size: 18px;
            font-weight: bold;
            padding: 6px 4px;
        }
        QGroupBox {
            border: 2px solid #004D98;
            border-radius: 6px;
            margin-top: 8px;
            font-weight: bold;
            color: #004D98;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 4px 0 4px;
        }
        QTableWidget {
            background-color: white;
            border: 1px solid #d5dbe3;
            gridline-color: #e6ebf2;
        }
        QHeaderView::section {
            background-color: #004D98;
            color: white;
            font-weight: bold;
            padding: 6px;
            border: none;
        }
        #btnTransferirProducto, #btnRefrescarColas, #btnCerrarSucursal {
            background-color: #004D98;
            color: white;
            font-weight: bold;
            border: none;
            border-radius: 4px;
            padding: 8px 14px;
        }
        #btnTransferirProducto:hover, #btnRefrescarColas:hover, #btnCerrarSucursal:hover {
            background-color: #A50044;
        }
        #lblTiemposSucursal {
            color: #1f2937;
            font-size: 14px;
            line-height: 1.4;
        }
    )";
    setStyleSheet(estilos);
}

void VentanaSucursal::cargarDatosSucursal() {
    if (sucursal == nullptr || lblInfoSucursal == nullptr) {
        if (lblInfoSucursal != nullptr) {
            lblInfoSucursal->setText("Sucursal no disponible.");
        }
        return;
    }

    lblInfoSucursal->setText(
        QString("Sucursal %1 - %2 | Ubicacion: %3 | Ingreso: %4 s | Preparacion: %5 s | Despacho: %6 s")
            .arg(sucursal->obtenerId())
            .arg(QString::fromStdString(sucursal->obtenerNombre()))
            .arg(QString::fromStdString(sucursal->obtenerUbicacion()))
            .arg(sucursal->obtenerTiempoIngreso())
            .arg(sucursal->obtenerTiempoPreparacion())
            .arg(sucursal->obtenerIntervaloDespacho()));
}

// Recorre una cola con un puntero temporal y copia su contenido en una tabla.
// La operación es O(n) por cola, ya que visita cada nodo una sola vez sin mutar la estructura.
void VentanaSucursal::llenarTablaDesdeCola(QTableWidget* tabla, Cola* cola) {
    if (tabla == nullptr) {
        return;
    }

    tabla->setRowCount(0);
    if (cola == nullptr) {
        return;
    }

    Cola::NodoCola* nodo = cola->obtenerFrente();
    while (nodo != nullptr) {
        Producto* producto = nodo->dato;
        if (producto != nullptr) {
            const int fila = tabla->rowCount();
            tabla->insertRow(fila);
            tabla->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(producto->codigoBarras)));
            tabla->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(producto->nombre)));
            tabla->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(producto->obtenerEstado())));
        }
        nodo = nodo->siguiente;
    }
}

// Actualiza las tres tablas de colas de la sucursal sin desencolar elementos.
// Complejidad O(n) por cada cola, determinada por el recorrido lineal de sus nodos.
void VentanaSucursal::refrescarColas() {
    if (sucursal == nullptr) {
        return;
    }

    llenarTablaDesdeCola(tablaColaIngreso, sucursal->obtenerColaIngreso());
    llenarTablaDesdeCola(tablaColaPreparacion, sucursal->obtenerColaPreparacion());
    llenarTablaDesdeCola(tablaColaSalida, sucursal->obtenerColaSalida());
}

// Captura la rueda del raton para aplicar zoom sobre las vistas de arboles.
// Complejidad O(1), ya que solo ajusta la transformacion de la vista activa.
bool VentanaSucursal::eventFilter(QObject* obj, QEvent* event) {
    QGraphicsView* vistaObjetivo = nullptr;

    if (vistaArbolAVL != nullptr && obj == vistaArbolAVL->viewport()) {
        vistaObjetivo = vistaArbolAVL;
    } else if (vistaArbolB != nullptr && obj == vistaArbolB->viewport()) {
        vistaObjetivo = vistaArbolB;
    } else if (vistaArbolBPlus != nullptr && obj == vistaArbolBPlus->viewport()) {
        vistaObjetivo = vistaArbolBPlus;
    }

    if (vistaObjetivo != nullptr && event->type() == QEvent::Wheel) {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        const qreal factorZoom = 1.15;

        if (wheelEvent->angleDelta().y() > 0) {
            vistaObjetivo->scale(factorZoom, factorZoom);
        } else {
            vistaObjetivo->scale(1.0 / factorZoom, 1.0 / factorZoom);
        }
        return true;
    }

    return QDialog::eventFilter(obj, event);
}

// Carga una imagen PNG en la escena asociada y ajusta la vista para mostrarla completa.
// La insercion visual es O(1) respecto a la estructura del arbol; el costo depende del archivo imagen.
void VentanaSucursal::cargarImagenEnVista(QGraphicsView* vista, QGraphicsScene* escena, const QString& rutaImagen) {
    if (vista == nullptr) {
        return;
    }

    if (escena == nullptr) {
        return;
    }

    escena->clear();

    QPixmap imagen(rutaImagen);
    if (imagen.isNull()) {
        escena->addText("No se pudo cargar la imagen del arbol.");
        vista->resetTransform();
        return;
    }

    escena->addPixmap(imagen);
    escena->setSceneRect(imagen.rect());
    vista->resetTransform();
    vista->fitInView(escena->sceneRect(), Qt::KeepAspectRatio);
}

void VentanaSucursal::cargarArboles() {
    if (sucursal == nullptr) {
        return;
    }

    if (sucursal->obtenerArbolAVL() != nullptr) {
        sucursal->obtenerArbolAVL()->generarImagen();
    }
    if (sucursal->obtenerArbolB() != nullptr) {
        sucursal->obtenerArbolB()->generarImagen();
    }
    if (sucursal->obtenerArbolBPlus() != nullptr) {
        sucursal->obtenerArbolBPlus()->generarImagen();
    }

    cargarImagenEnVista(vistaArbolAVL, escenaArbolAVL, "data/avl.png");
    cargarImagenEnVista(vistaArbolB, escenaArbolB, "data/arbol_b.png");
    cargarImagenEnVista(vistaArbolBPlus, escenaArbolBPlus, "data/arbol_bplus.png");
}

// Solicita el producto a transferir y delega la programacion al simulador.
// La busqueda es O(1) promedio por tabla hash y la programacion depende del grafo y la ruta calculada.
void VentanaSucursal::onTransferirProducto() {
    if (sucursal == nullptr || simulador == nullptr) {
        QMessageBox::warning(this, "Operacion no disponible", "No hay sucursal o simulador activo.");
        std::cout << "[VentanaSucursal] Transferencia cancelada: sucursal o simulador inexistente.\n";
        return;
    }

    bool okCodigo = false;
    const QString codigo = QInputDialog::getText(
        this, "Transferir producto", "Ingrese el codigo de barras:", QLineEdit::Normal, "", &okCodigo);
    if (!okCodigo || codigo.trimmed().isEmpty()) {
        std::cout << "[VentanaSucursal] Transferencia cancelada: codigo vacio.\n";
        return;
    }

    Producto* producto = sucursal->buscarPorCodigo(codigo.trimmed().toStdString());
    if (producto == nullptr) {
        QMessageBox::warning(this, "Producto no encontrado", "El codigo no existe en esta sucursal.");
        std::cout << "[VentanaSucursal] Producto no encontrado en sucursal "
                  << sucursal->obtenerId() << ": " << codigo.trimmed().toStdString() << '\n';
        return;
    }

    std::cout << "[VentanaSucursal] Producto localizado: " << producto->codigoBarras
              << " - " << producto->nombre << '\n';

    bool okDestino = false;
    const int idDestino = QInputDialog::getInt(
        this, "Sucursal destino", "Ingrese el ID de sucursal destino:", 1, 0, 1000000, 1, &okDestino);
    if (!okDestino) {
        std::cout << "[VentanaSucursal] Transferencia cancelada: destino no confirmado.\n";
        return;
    }

    if (idDestino == sucursal->obtenerId()) {
        QMessageBox::warning(this, "Destino invalido", "La sucursal destino debe ser diferente al origen.");
        std::cout << "[VentanaSucursal] Transferencia cancelada: destino igual al origen.\n";
        return;
    }

    std::cout << "[VentanaSucursal] Programando envio de " << producto->codigoBarras
              << " desde sucursal " << sucursal->obtenerId()
              << " hacia sucursal " << idDestino << '\n';
    simulador->programarEnvio(producto, sucursal->obtenerId(), idDestino, true);
    std::cout << "[VentanaSucursal] Envio programado correctamente para "
              << producto->codigoBarras << '\n';

    QDialog dialogoEstado(this);
    dialogoEstado.setWindowTitle("Simulacion de envio");
    dialogoEstado.setModal(true);
    dialogoEstado.setFixedSize(320, 120);
    QVBoxLayout* layoutEstado = new QVBoxLayout(&dialogoEstado);
    QLabel* lblEstado = new QLabel("Enviando producto... Espere 5 segundos.", &dialogoEstado);
    lblEstado->setAlignment(Qt::AlignCenter);
    layoutEstado->addWidget(lblEstado);
    QTimer::singleShot(5000, &dialogoEstado, &QDialog::accept);
    dialogoEstado.exec();

    refrescarColas();
    cargarArboles();
    QMessageBox::information(this, "Envio programado", "La transferencia fue enviada al simulador.");
}
