#ifndef VENTANA_SUCURSAL_H
#define VENTANA_SUCURSAL_H

#include <QDialog>
#include <QEvent>
#include <QString>

class QLabel;
class QTabWidget;
class QTableWidget;
class QGraphicsView;
class QGraphicsScene;
class QPushButton;
class Cola;
class Simulador;
class Sucursal;

// VentanaSucursal: Dialogo modal para visualizar el estado operativo de una sucursal.
// Presenta colas de despacho, arboles de indices y acciones de transferencia.
class VentanaSucursal : public QDialog {
    Q_OBJECT

public:
    // Construye el dialogo de sucursal con referencias al backend. Complejidad O(1).
    explicit VentanaSucursal(Sucursal* sucursal, Simulador* simulador, QWidget* parent = nullptr);

    // Destructor por defecto; Qt libera widgets hijos automaticamente. Complejidad O(1).
    ~VentanaSucursal() override = default;

private slots:
    // Programa traslado de producto usando el simulador. Complejidad O((V+E) log V).
    void onTransferirProducto();

protected:
    // Captura la rueda del raton para aplicar zoom sobre las vistas de arboles. Complejidad O(1).
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    Sucursal* sucursal;
    Simulador* simulador;
    QTabWidget* pestanas;
    QLabel* lblInfoSucursal;
    QTableWidget* tablaColaIngreso;
    QTableWidget* tablaColaPreparacion;
    QTableWidget* tablaColaSalida;
    QGraphicsView* vistaArbolAVL;
    QGraphicsView* vistaArbolB;
    QGraphicsView* vistaArbolBPlus;
    QGraphicsScene* escenaArbolAVL;
    QGraphicsScene* escenaArbolB;
    QGraphicsScene* escenaArbolBPlus;
    QPushButton* btnTransferir;
    QPushButton* btnRefrescarColas;

    // Configura la estructura visual del dialogo. Complejidad O(1).
    void configurarInterfaz();

    // Aplica tema visual blaugrana a controles del dialogo. Complejidad O(1).
    void aplicarEstilos();

    // Actualiza etiqueta superior con metadatos de sucursal. Complejidad O(1).
    void cargarDatosSucursal();

    // Recorre nodos de colas y llena tablas sin mutar la estructura. Complejidad O(n).
    void refrescarColas();

    // Genera imagenes de arboles y las renderiza en vistas graficas. Complejidad O(1).
    void cargarArboles();

    // Llena una tabla con los elementos de una cola enlazada. Complejidad O(n).
    void llenarTablaDesdeCola(QTableWidget* tabla, Cola* cola);

    // Carga una imagen PNG en una vista grafica y ajusta su escala inicial. Complejidad O(1).
    void cargarImagenEnVista(QGraphicsView* vista, QGraphicsScene* escena, const QString& rutaImagen);
};

#endif  // VENTANA_SUCURSAL_H
