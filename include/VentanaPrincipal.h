#ifndef VENTANA_PRINCIPAL_H
#define VENTANA_PRINCIPAL_H
//todas las librerias necesarias para lo grafico
#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTableWidget>
#include <QFrame>
#include <QLabel>
#include <QSpacerItem>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QDialog>
#include <QFormLayout>
#include <QDir>
#include <QTextEdit>
#include <QPixmap>
#include <QTabWidget>
#include <QFontDatabase>
#include <QWheelEvent>
#include <QSlider>
#include <QTimer>
#include <string>
#include <vector>

// Incluir estructuras de datos del backend
#include "ListaSucursales.h"
#include "Sucursal.h"
#include "CargadorCSV.h"
#include "MedidorRendimiento.h"
#include "GrafoSucursales.h"
#include "Simulador.h"
#include "VisorGrafo.h"

// VentanaPrincipal: Ventana principal de la aplicación con tema FC Barcelona en teoria jaksjas
// Hereda de QMainWindow para proporcionar menús, barras de herramientas, etc.
class VentanaPrincipal : public QMainWindow {
    Q_OBJECT

public:
    // Constructor principal de la ventana.
    explicit VentanaPrincipal(QWidget* parent = nullptr);
    ~VentanaPrincipal();

    // Crea una nueva sucursal y la agrega a la colección enlazada. Complejidad O(n).
    void agregarSucursal(int id,
                         const std::string& nombre,
                         const std::string& ubicacion,
                         int tiempoIngreso,
                         int tiempoPreparacion,
                         int intervaloDespacho);

    // Registra una sucursal dentro del grafo de red. Complejidad O(1) amortizado.
    void agregarSucursalAGrafo(int id);

    // Registra una conexión ponderada dentro del grafo de sucursales. Complejidad O(1).
    void agregarConexionAGrafo(int origen,
                               int destino,
                               int tiempo,
                               int costo,
                               bool bidireccional);

    // Selecciona la sucursal activa por id para operar desde la UI. Complejidad O(n).
    void seleccionarSucursal(int id);

    // Retorna la sucursal activa; nullptr si no hay selección. Complejidad O(1).
    Sucursal* obtenerSucursalActual() const;

private slots:
    // Slots para manejar eventos de botones
    void onCargarSucursalesCSV();
    void onCargarConexionesCSV();
    void onCargarProductosCSV();
    void onAgregarSucursal();
    void onModificarSucursal();
    void onEliminarSucursal();
    void onIrASucursal();
    void onCambioSucursalSeleccionada(int indice);
    void onInsertarProducto();
    void onEliminarProducto();
    void onBuscarProducto();
    void onGenerarReportes();
    void onBenchmarking();
    void actualizarTabla();
    void onTickSimulacion();
    
    // Nuevos slots para búsqueda avanzada (Fase 7C)
    void ejecutarBusquedaAvanzada();
    void onCambioFiltroBusqueda(int indice);

private:
    // Widgets de la interfaz
    
    // Widget central y layout principal
    QWidget* widgetCentral;
    QHBoxLayout* layoutPrincipal;

    // Panel izquierdo (Sidebar)
    QFrame* panelSidebar;
    QVBoxLayout* layoutSidebar;
    QLabel* lblTitulo;

    // Botones del sidebar
    QPushButton* btnCargarSucursalesCSV;
    QPushButton* btnCargarConexionesCSV;
    QPushButton* btnCargarProductosCSV;
    QPushButton* btnAgregarSucursal;
    QPushButton* btnModificarSucursal;
    QPushButton* btnEliminarSucursal;
    QPushButton* btnInsertar;
    QPushButton* btnEliminar;
    QPushButton* btnBuscar;
    QPushButton* btnReportes;
    QPushButton* btnBenchmarking;

    // Panel derecho (Área de contenido)
    QWidget* panelContenido;
    QVBoxLayout* layoutContenido;
    
    // Barra de búsqueda avanzada
    QWidget* barraBusqueda;
    QHBoxLayout* layoutBarraBusqueda;
    QLabel* lblBuscarPor;
    QComboBox* comboFiltroBusqueda;
    QLineEdit* inputBusqueda;
    QLineEdit* inputBusquedaFin;  // Para rango de fechas
    QPushButton* btnEjecutarBusqueda;
    QPushButton* btnMostrarTodos;

    // Barra de selección de sucursal para administración.
    QWidget* barraSucursal;
    QHBoxLayout* layoutBarraSucursal;
    QLabel* lblSucursalesRegistradas;
    QComboBox* comboSucursales;
    QPushButton* btnIrASucursal;

    // Panel de visualización del grafo.
    VisorGrafo* visorGrafo;
    
    // Tabla de productos
    QTableWidget* tablaProductos;

    // Colección enlazada de sucursales y sucursal seleccionada.
    ListaSucursales* sucursales;
    Sucursal* sucursalActual;
    GrafoSucursales* grafo;
    Simulador* simulador;
    QTimer* temporizadorSimulacion;
    
    // Cargador de CSV (propio de la ventana)
    CargadorCSV cargadorCSV;

    // Métodos de inicialización
    void configurarInterfaz();
    void configurarSidebar();
    void configurarBarraSucursal();
    void configurarPanelGrafo();
    void configurarBarraBusqueda();
    void configurarTabla();
    void aplicarEstilos();
    void conectarSenales();
    void refrescarComboSucursales();
    int obtenerIdSucursalSeleccionada() const;
    void dibujarGrafo();
    void actualizarBarraEstado();
    bool haySucursalSeleccionada() const;
    void mostrarAdvertenciaSucursalNoSeleccionada();
    void liberarSucursales();
    
    // Método auxiliar para mostrar productos en la tabla
    void mostrarProductosEnTabla(const std::vector<Producto*>& productos);
    void regenerarVisualizaciones();
};

#endif // VENTANA_PRINCIPAL_H
