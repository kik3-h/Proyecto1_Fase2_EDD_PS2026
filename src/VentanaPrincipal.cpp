#include "VentanaPrincipal.h"

#include <vector>
#include <QTableWidgetItem>
#include <QDoubleValidator>
#include <QIntValidator>
#include <QStatusBar>
#include <QDialogButtonBox>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <iostream>


// Constructor: Inicializa la ventana principal para modo multi-sucursal.
VentanaPrincipal::VentanaPrincipal(QWidget* parent)
    : QMainWindow(parent),
      widgetCentral(nullptr),
      layoutPrincipal(nullptr),
      panelSidebar(nullptr),
      layoutSidebar(nullptr),
      lblTitulo(nullptr),
      btnCargarSucursalesCSV(nullptr),
      btnCargarConexionesCSV(nullptr),
      btnCargarProductosCSV(nullptr),
      btnAgregarSucursal(nullptr),
      btnModificarSucursal(nullptr),
      btnEliminarSucursal(nullptr),
      btnInsertar(nullptr),
      btnEliminar(nullptr),
      btnBuscar(nullptr),
      btnReportes(nullptr),
      btnBenchmarking(nullptr),
      barraSucursal(nullptr),
      layoutBarraSucursal(nullptr),
      lblSucursalesRegistradas(nullptr),
      comboSucursales(nullptr),
      btnIrASucursal(nullptr),
      visorGrafo(nullptr),
      tablaProductos(nullptr),
      sucursales(new ListaSucursales()),
      sucursalActual(nullptr),
      grafo(new GrafoSucursales()),
      simulador(new Simulador(grafo, sucursales)),
      temporizadorSimulacion(new QTimer(this)) {
    
    // Configurar ventana principal
    setWindowTitle("Catalogo P1 EDD KIKE");
    setMinimumSize(1200, 700);
    resize(1400, 800);

    // Inicializar componentes
    configurarInterfaz();
    aplicarEstilos();
    conectarSenales();
    temporizadorSimulacion->start(1000);

    // Sucursal por defecto para iniciar operaciones de inventario.
    agregarSucursal(1, "Central", "Ciudad de Guatemala", 10, 15, 20);
    seleccionarSucursal(1);
    refrescarComboSucursales();
    dibujarGrafo();
    actualizarTabla();
}

// Destructor
VentanaPrincipal::~VentanaPrincipal() {
    if (temporizadorSimulacion != nullptr) {
        temporizadorSimulacion->stop();
    }
    delete simulador;
    simulador = nullptr;
    liberarSucursales();
    delete grafo;
    grafo = nullptr;
    delete sucursales;
    sucursales = nullptr;
}

// agregarSucursal: Inserta una sucursal nueva en la lista enlazada de sucursales.
// Complejidad: O(n), por validación de id existente.
void VentanaPrincipal::agregarSucursal(int id,
                                       const std::string& nombre,
                                       const std::string& ubicacion,
                                       int tiempoIngreso,
                                       int tiempoPreparacion,
                                       int intervaloDespacho) {
    if (sucursales == nullptr) {
        return;
    }

    if (sucursales->buscar(id) != nullptr) {
        return;
    }

    Sucursal* nuevaSucursal = new Sucursal(id,
                                           nombre,
                                           ubicacion,
                                           tiempoIngreso,
                                           tiempoPreparacion,
                                           intervaloDespacho);
    sucursales->agregar(nuevaSucursal);
    agregarSucursalAGrafo(id);
    refrescarComboSucursales();
    dibujarGrafo();
}

// agregarSucursalAGrafo: Registra un nodo de sucursal en el grafo de red.
// Complejidad: O(1) amortizado.
void VentanaPrincipal::agregarSucursalAGrafo(int id) {
    if (grafo == nullptr) {
        return;
    }

    grafo->agregarSucursal(id);
}

// agregarConexionAGrafo: Registra una arista ponderada entre sucursales.
// Complejidad: O(1).
void VentanaPrincipal::agregarConexionAGrafo(int origen,
                                             int destino,
                                             int tiempo,
                                             int costo,
                                             bool bidireccional) {
    if (grafo == nullptr) {
        return;
    }

    grafo->agregarConexion(origen, destino, tiempo, costo, bidireccional);
}

// seleccionarSucursal: Selecciona una sucursal activa por id.
// Complejidad: O(n), por búsqueda lineal en la lista.
void VentanaPrincipal::seleccionarSucursal(int id) {
    if (sucursales == nullptr) {
        sucursalActual = nullptr;
        return;
    }

    sucursalActual = sucursales->buscar(id);
    actualizarBarraEstado();
}

// obtenerSucursalActual: Retorna la sucursal activa actual.
// Complejidad: O(1).
Sucursal* VentanaPrincipal::obtenerSucursalActual() const {
    return sucursalActual;
}

// haySucursalSeleccionada: Verifica si hay sucursal seleccionada.
// Complejidad: O(1).
bool VentanaPrincipal::haySucursalSeleccionada() const {
    return sucursalActual != nullptr;
}

// mostrarAdvertenciaSucursalNoSeleccionada: Muestra alerta de selección faltante.
// Complejidad: O(1).
void VentanaPrincipal::mostrarAdvertenciaSucursalNoSeleccionada() {
    QMessageBox::warning(this, "Sucursal no seleccionada", "Seleccione una sucursal primero");
}

// liberarSucursales: Libera memoria de las sucursales administradas.
// Complejidad: O(n), donde n es el número de sucursales.
void VentanaPrincipal::liberarSucursales() {
    if (sucursales == nullptr) {
        sucursalActual = nullptr;
        return;
    }

    ListaSucursales::NodoSucursal* actual = sucursales->obtenerPrimero();
    while (actual != nullptr) {
        delete actual->dato;
        actual->dato = nullptr;
        actual = actual->siguiente;
    }
    sucursalActual = nullptr;
}

// Configurar la interfaz principal
void VentanaPrincipal::configurarInterfaz() {
    // Crear widget central
    widgetCentral = new QWidget(this);
    setCentralWidget(widgetCentral);

    // Layout horizontal principal (sidebar | contenido)
    layoutPrincipal = new QHBoxLayout(widgetCentral);
    layoutPrincipal->setContentsMargins(0, 0, 0, 0);
    layoutPrincipal->setSpacing(0);

    // Configurar sidebar
    configurarSidebar();
    
    // Crear panel de contenido (derecha)
    panelContenido = new QWidget(widgetCentral);
    layoutContenido = new QVBoxLayout(panelContenido);
    layoutContenido->setContentsMargins(10, 10, 10, 10);
    layoutContenido->setSpacing(10);
    
    // Configurar barra de administración, grafo, búsqueda y tabla
    configurarBarraSucursal();
    configurarPanelGrafo();
    configurarBarraBusqueda();
    configurarTabla();
    
    // Agregar componentes al panel de contenido
    layoutContenido->addWidget(barraSucursal);
    layoutContenido->addWidget(visorGrafo);
    layoutContenido->addWidget(barraBusqueda);
    layoutContenido->addWidget(tablaProductos, 1);  // stretch=1 para expandir

    // Agregar widgets al layout principal
    layoutPrincipal->addWidget(panelSidebar);
    layoutPrincipal->addWidget(panelContenido, 1);
}

// Configurar el panel lateral (Sidebar)
void VentanaPrincipal::configurarSidebar() {
    // Frame contenedor del sidebar
    panelSidebar = new QFrame(widgetCentral);
    panelSidebar->setObjectName("panelSidebar");
    panelSidebar->setFixedWidth(250);

    // Layout vertical para el sidebar
    layoutSidebar = new QVBoxLayout(panelSidebar);
    layoutSidebar->setContentsMargins(15, 20, 15, 20);
    layoutSidebar->setSpacing(12);

    // Título del sidebar
    lblTitulo = new QLabel("Catalogo EDD", panelSidebar);
    lblTitulo->setObjectName("lblTitulo");
    lblTitulo->setAlignment(Qt::AlignCenter);

    // Crear botones del menú - sección Archivos
    QLabel* lblSeccionArchivos = new QLabel("Archivos", panelSidebar);
    lblSeccionArchivos->setObjectName("lblSeccionSidebar");
    btnCargarSucursalesCSV = new QPushButton(panelSidebar);
    btnCargarSucursalesCSV->setObjectName("btnSidebar");
    btnCargarSucursalesCSV->setText("Cargar Sucursales CSV");
    btnCargarSucursalesCSV->setCursor(Qt::PointingHandCursor);

    btnCargarConexionesCSV = new QPushButton(panelSidebar);
    btnCargarConexionesCSV->setObjectName("btnSidebar");
    btnCargarConexionesCSV->setText("Cargar Conexiones CSV");
    btnCargarConexionesCSV->setCursor(Qt::PointingHandCursor);

    btnCargarProductosCSV = new QPushButton(panelSidebar);
    btnCargarProductosCSV->setObjectName("btnSidebar");
    btnCargarProductosCSV->setText("Cargar Productos CSV");
    btnCargarProductosCSV->setCursor(Qt::PointingHandCursor);

    // Sección Sucursales
    QLabel* lblSeccionSucursales = new QLabel("Sucursales", panelSidebar);
    lblSeccionSucursales->setObjectName("lblSeccionSidebar");
    btnAgregarSucursal = new QPushButton(panelSidebar);
    btnAgregarSucursal->setObjectName("btnSidebar");
    btnAgregarSucursal->setText("Agregar Sucursal");
    btnAgregarSucursal->setCursor(Qt::PointingHandCursor);

    btnModificarSucursal = new QPushButton(panelSidebar);
    btnModificarSucursal->setObjectName("btnSidebar");
    btnModificarSucursal->setText("Modificar Sucursal");
    btnModificarSucursal->setCursor(Qt::PointingHandCursor);

    btnEliminarSucursal = new QPushButton(panelSidebar);
    btnEliminarSucursal->setObjectName("btnSidebar");
    btnEliminarSucursal->setText("Eliminar Sucursal");
    btnEliminarSucursal->setCursor(Qt::PointingHandCursor);

    // Sección Inventario
    QLabel* lblSeccionInventario = new QLabel("Inventario", panelSidebar);
    lblSeccionInventario->setObjectName("lblSeccionSidebar");
    btnInsertar = new QPushButton("Insertar Producto", panelSidebar);
    btnInsertar->setObjectName("btnSidebar");
    btnInsertar->setText("Insertar Producto");
    btnInsertar->setCursor(Qt::PointingHandCursor);

    btnEliminar = new QPushButton("Eliminar Producto", panelSidebar);
    btnEliminar->setObjectName("btnSidebar");
    btnEliminar->setText("Eliminar Producto");
    btnEliminar->setCursor(Qt::PointingHandCursor);

    btnBuscar = new QPushButton("Buscar Producto", panelSidebar);
    btnBuscar->setObjectName("btnSidebar");
    btnBuscar->setText("Buscar Producto");
    btnBuscar->setCursor(Qt::PointingHandCursor);

    btnReportes = new QPushButton("Generar Reportes", panelSidebar);
    btnReportes->setObjectName("btnSidebar");
    btnReportes->setText("Generar Reportes");
    btnReportes->setCursor(Qt::PointingHandCursor);

    btnBenchmarking = new QPushButton("Pruebas Rendimiento", panelSidebar);
    btnBenchmarking->setObjectName("btnSidebar");
    btnBenchmarking->setText("Pruebas Rendimiento");
    btnBenchmarking->setCursor(Qt::PointingHandCursor);

    // Agregar widgets al layout del sidebar
    layoutSidebar->addWidget(lblTitulo);
    layoutSidebar->addSpacing(12);
    layoutSidebar->addWidget(lblSeccionArchivos);
    layoutSidebar->addWidget(btnCargarSucursalesCSV);
    layoutSidebar->addWidget(btnCargarConexionesCSV);
    layoutSidebar->addWidget(btnCargarProductosCSV);
    layoutSidebar->addSpacing(8);
    layoutSidebar->addWidget(lblSeccionSucursales);
    layoutSidebar->addWidget(btnAgregarSucursal);
    layoutSidebar->addWidget(btnModificarSucursal);
    layoutSidebar->addWidget(btnEliminarSucursal);
    layoutSidebar->addSpacing(8);
    layoutSidebar->addWidget(lblSeccionInventario);
    layoutSidebar->addWidget(btnInsertar);
    layoutSidebar->addWidget(btnEliminar);
    layoutSidebar->addWidget(btnBuscar);
    layoutSidebar->addWidget(btnReportes);
    layoutSidebar->addWidget(btnBenchmarking);

    // Espaciador para empujar botones hacia arriba
    layoutSidebar->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, 
                                                  QSizePolicy::Expanding));

    // Etiqueta de versión al fondo
    QLabel* lblVersion = new QLabel("v1.0.0 - USAC 2026", panelSidebar);
    lblVersion->setObjectName("lblVersion");
    lblVersion->setAlignment(Qt::AlignCenter);
    layoutSidebar->addWidget(lblVersion);
}

// configurarBarraSucursal: Crea la barra de selección de sucursal para administración.
// Complejidad: O(1).
void VentanaPrincipal::configurarBarraSucursal() {
    barraSucursal = new QWidget(panelContenido);
    barraSucursal->setObjectName("barraSucursal");
    barraSucursal->setFixedHeight(52);

    layoutBarraSucursal = new QHBoxLayout(barraSucursal);
    layoutBarraSucursal->setContentsMargins(8, 6, 8, 6);
    layoutBarraSucursal->setSpacing(10);

    lblSucursalesRegistradas = new QLabel("Sucursales registradas:", barraSucursal);
    lblSucursalesRegistradas->setObjectName("lblSucursalesRegistradas");

    comboSucursales = new QComboBox(barraSucursal);
    comboSucursales->setObjectName("comboSucursales");
    comboSucursales->setMinimumWidth(280);

    btnIrASucursal = new QPushButton("Ir a Sucursal", barraSucursal);
    btnIrASucursal->setObjectName("btnIrASucursal");
    btnIrASucursal->setCursor(Qt::PointingHandCursor);
    btnIrASucursal->setFixedWidth(140);

    layoutBarraSucursal->addWidget(lblSucursalesRegistradas);
    layoutBarraSucursal->addWidget(comboSucursales, 1);
    layoutBarraSucursal->addWidget(btnIrASucursal);
}

// configurarPanelGrafo: Prepara el contenedor de visualización del grafo de sucursales.
// Complejidad: O(1).
void VentanaPrincipal::configurarPanelGrafo() {
    visorGrafo = new VisorGrafo(panelContenido);
}

// Configurar la barra de búsqueda avanzada
void VentanaPrincipal::configurarBarraBusqueda() {
    // Contenedor de la barra de búsqueda
    barraBusqueda = new QWidget(panelContenido);
    barraBusqueda->setObjectName("barraBusqueda");
    barraBusqueda->setFixedHeight(50);
    
    layoutBarraBusqueda = new QHBoxLayout(barraBusqueda);
    layoutBarraBusqueda->setContentsMargins(5, 5, 5, 5);
    layoutBarraBusqueda->setSpacing(10);
    
    // Label "Buscar por:"
    lblBuscarPor = new QLabel("Buscar por:", barraBusqueda);
    lblBuscarPor->setObjectName("lblBuscarPor");
    
    // ComboBox con tipos de búsqueda
    comboFiltroBusqueda = new QComboBox(barraBusqueda);
    comboFiltroBusqueda->setObjectName("comboFiltroBusqueda");
    comboFiltroBusqueda->addItem("Codigo de barras (Hash)");
    comboFiltroBusqueda->addItem("Nombre (AVL)");
    comboFiltroBusqueda->addItem("Categoria (B+)");
    comboFiltroBusqueda->addItem("Rango de Fecha (B)");
    comboFiltroBusqueda->setFixedWidth(180);
    
    // Input de búsqueda principal
    inputBusqueda = new QLineEdit(barraBusqueda);
    inputBusqueda->setObjectName("inputBusqueda");
    inputBusqueda->setPlaceholderText("Codigo de barras...");
    inputBusqueda->setMinimumWidth(200);
    
    // Input secundario para rango de fechas (oculto por defecto)
    inputBusquedaFin = new QLineEdit(barraBusqueda);
    inputBusquedaFin->setObjectName("inputBusquedaFin");
    inputBusquedaFin->setPlaceholderText("Fecha fin (YYYY-MM-DD)");
    inputBusquedaFin->setFixedWidth(160);
    inputBusquedaFin->setVisible(false);
    
    // Botón buscar
    btnEjecutarBusqueda = new QPushButton("Buscar", barraBusqueda);
    btnEjecutarBusqueda->setObjectName("btnBusqueda");
    btnEjecutarBusqueda->setCursor(Qt::PointingHandCursor);
    btnEjecutarBusqueda->setFixedWidth(100);
    
    // Botón mostrar todos
    btnMostrarTodos = new QPushButton("Mostrar Todos", barraBusqueda);
    btnMostrarTodos->setObjectName("btnMostrarTodos");
    btnMostrarTodos->setCursor(Qt::PointingHandCursor);
    btnMostrarTodos->setFixedWidth(130);
    
    // Agregar widgets al layout
    layoutBarraBusqueda->addWidget(lblBuscarPor);
    layoutBarraBusqueda->addWidget(comboFiltroBusqueda);
    layoutBarraBusqueda->addWidget(inputBusqueda);
    layoutBarraBusqueda->addWidget(inputBusquedaFin);
    layoutBarraBusqueda->addWidget(btnEjecutarBusqueda);
    layoutBarraBusqueda->addWidget(btnMostrarTodos);
    layoutBarraBusqueda->addStretch();
}

// Configurar la tabla de productos
void VentanaPrincipal::configurarTabla() {
    tablaProductos = new QTableWidget(widgetCentral);
    tablaProductos->setObjectName("tablaProductos");

    // Configurar columnas
    tablaProductos->setColumnCount(6);
    QStringList encabezados;
    encabezados << "Código" << "Nombre" << "Categoría" << "Precio" << "Stock" << "Caducidad";
    tablaProductos->setHorizontalHeaderLabels(encabezados);

    // Configurar comportamiento de la tabla
    tablaProductos->setEditTriggers(QAbstractItemView::NoEditTriggers);  // Solo lectura
    tablaProductos->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaProductos->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaProductos->setAlternatingRowColors(true);
    tablaProductos->setSortingEnabled(true);

    // Configurar headers
    tablaProductos->horizontalHeader()->setStretchLastSection(true);
    tablaProductos->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tablaProductos->verticalHeader()->setVisible(false);

    // Establecer anchos iniciales de columnas
    tablaProductos->setColumnWidth(0, 120);  // Código
    tablaProductos->setColumnWidth(1, 250);  // Nombre
    tablaProductos->setColumnWidth(2, 120);  // Categoría
    tablaProductos->setColumnWidth(3, 100);  // Precio
    tablaProductos->setColumnWidth(4, 80);   // Stock
    tablaProductos->setColumnWidth(5, 120);  // Caducidad
}

// Aplicar estilos QSS (Tema FC Barcelona: Blaugrana kjaksajs)

void VentanaPrincipal::aplicarEstilos() {
    QString estilos = R"(
        /* ===== VENTANA PRINCIPAL ===== */
        QMainWindow {
            background-color: #f5f5f5;
        }

        /* ===== SIDEBAR (Panel Izquierdo) ===== */
        #panelSidebar {
            background-color: #004D98;
            border: none;
        }

        /* ===== TÍTULO DEL SIDEBAR ===== */
        #lblTitulo {
            color: #EDBB00;
            font-size: 22px;
            font-weight: bold;
            padding: 10px;
            border-bottom: 2px solid #EDBB00;
            margin-bottom: 10px;
        }

        #lblSeccionSidebar {
            color: #EDBB00;
            font-size: 13px;
            font-weight: bold;
            margin-top: 6px;
        }

        /* ===== BOTONES DEL SIDEBAR ===== */
        #btnSidebar {
            background-color: #A50044;
            color: white;
            font-size: 14px;
            font-weight: bold;
            padding: 12px 15px;
            border: none;
            border-radius: 6px;
            text-align: left;
        }

        #btnSidebar:hover {
            background-color: #EDBB00;
            color: #004D98;
        }

        #btnSidebar:pressed {
            background-color: #d4a800;
            color: #003366;
        }

        /* ===== ETIQUETA DE VERSIÓN ===== */
        #lblVersion {
            color: rgba(255, 255, 255, 0.6);
            font-size: 11px;
            padding: 5px;
        }

        /* ===== BARRA SUPERIOR DE SUCURSALES ===== */
        #barraSucursal {
            background-color: #f8f9fa;
            border: 1px solid #dee2e6;
            border-radius: 6px;
        }

        #lblSucursalesRegistradas {
            color: #004D98;
            font-weight: bold;
            font-size: 13px;
        }

        #comboSucursales {
            padding: 6px 10px;
            border: 2px solid #004D98;
            border-radius: 4px;
            background-color: white;
            font-size: 13px;
        }

        #comboSucursales:hover {
            border-color: #A50044;
        }

        #btnIrASucursal {
            background-color: #004D98;
            color: white;
            font-weight: bold;
            border: none;
            border-radius: 4px;
            padding: 8px 14px;
        }

        #btnIrASucursal:hover {
            background-color: #A50044;
        }

        /* ===== PANEL DEL GRAFO ===== */
        #frameGrafo {
            background-color: white;
            border: 1px solid #d8dde3;
            border-radius: 6px;
        }

        #lblTituloGrafo {
            color: #004D98;
            font-size: 14px;
            font-weight: bold;
        }

        #vistaGrafo {
            border: 1px solid #e7e9ec;
            background-color: #ffffff;
        }

        /* ===== TABLA DE PRODUCTOS ===== */
        #tablaProductos {
            background-color: white;
            alternate-background-color: #f0f4f8;
            gridline-color: #ddd;
            border: none;
            font-size: 13px;
        }

        #tablaProductos::item {
            padding: 8px;
            border-bottom: 1px solid #eee;
        }

        #tablaProductos::item:selected {
            background-color: #004D98;
            color: white;
        }

        #tablaProductos::item:hover {
            background-color: #e8f0fe;
        }

        /* ===== ENCABEZADOS DE LA TABLA ===== */
        QHeaderView::section {
            background-color: #004D98;
            color: white;
            font-weight: bold;
            font-size: 13px;
            padding: 10px 8px;
            border: none;
            border-right: 1px solid #003366;
        }

        QHeaderView::section:hover {
            background-color: #0066cc;
        }

        /* ===== SCROLLBARS ===== */
        QScrollBar:vertical {
            background-color: #f0f0f0;
            width: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:vertical {
            background-color: #004D98;
            border-radius: 6px;
            min-height: 30px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #A50044;
        }

        QScrollBar:horizontal {
            background-color: #f0f0f0;
            height: 12px;
            border-radius: 6px;
        }

        QScrollBar::handle:horizontal {
            background-color: #004D98;
            border-radius: 6px;
            min-width: 30px;
        }

        QScrollBar::handle:horizontal:hover {
            background-color: #A50044;
        }

        QScrollBar::add-line, QScrollBar::sub-line {
            border: none;
            background: none;
        }

        /* ===== TOOLTIPS ===== */
        QToolTip {
            background-color: #004D98;
            color: white;
            border: 1px solid #EDBB00;
            padding: 5px;
            border-radius: 4px;
            font-size: 12px;
        }

        /* ===== DIÁLOGOS DE ENTRADA ===== */
        QInputDialog, QMessageBox {
            background-color: white;
        }

        QInputDialog QLineEdit {
            padding: 8px;
            border: 2px solid #004D98;
            border-radius: 4px;
            font-size: 14px;
        }

        QInputDialog QPushButton, QMessageBox QPushButton {
            background-color: #004D98;
            color: white;
            padding: 8px 20px;
            border: none;
            border-radius: 4px;
            font-weight: bold;
        }

        QInputDialog QPushButton:hover, QMessageBox QPushButton:hover {
            background-color: #A50044;
        }

        /* ===== BARRA DE BÚSQUEDA AVANZADA ===== */
        #barraBusqueda {
            background-color: #f8f9fa;
            border: 1px solid #dee2e6;
            border-radius: 6px;
        }

        #lblBuscarPor {
            color: #004D98;
            font-weight: bold;
            font-size: 13px;
        }

        #comboFiltroBusqueda {
            padding: 6px 10px;
            border: 2px solid #004D98;
            border-radius: 4px;
            background-color: white;
            font-size: 13px;
        }

        #comboFiltroBusqueda:hover {
            border-color: #A50044;
        }

        #comboFiltroBusqueda::drop-down {
            border: none;
            width: 25px;
        }

        #inputBusqueda, #inputBusquedaFin {
            padding: 8px 12px;
            border: 2px solid #004D98;
            border-radius: 4px;
            font-size: 13px;
            background-color: white;
        }

        #inputBusqueda:focus, #inputBusquedaFin:focus {
            border-color: #EDBB00;
        }

        #btnBusqueda {
            background-color: #004D98;
            color: white;
            font-weight: bold;
            padding: 8px 15px;
            border: none;
            border-radius: 4px;
        }

        #btnBusqueda:hover {
            background-color: #A50044;
        }

        #btnMostrarTodos {
            background-color: #6c757d;
            color: white;
            font-weight: bold;
            padding: 8px 15px;
            border: none;
            border-radius: 4px;
        }

        #btnMostrarTodos:hover {
            background-color: #5a6268;
        }

        /* ===== FORMULARIO INSERCIÓN (QDialog) ===== */
        QDialog {
            background-color: #f5f5f5;
        }

        QDialog QLabel {
            font-size: 13px;
            color: #333;
            font-weight: bold;
        }

        QDialog QLineEdit {
            padding: 8px;
            border: 2px solid #004D98;
            border-radius: 4px;
            font-size: 13px;
            background-color: white;
        }

        QDialog QLineEdit:focus {
            border-color: #EDBB00;
        }

        QDialog QPushButton {
            background-color: #004D98;
            color: white;
            padding: 10px 25px;
            border: none;
            border-radius: 4px;
            font-weight: bold;
            font-size: 13px;
        }

        QDialog QPushButton:hover {
            background-color: #A50044;
        }
    )";

    setStyleSheet(estilos);
}

// Conectar señales y slots
void VentanaPrincipal::conectarSenales() {
    // Botones del sidebar
    connect(btnCargarSucursalesCSV, &QPushButton::clicked, this, &VentanaPrincipal::onCargarSucursalesCSV);
    connect(btnCargarConexionesCSV, &QPushButton::clicked, this, &VentanaPrincipal::onCargarConexionesCSV);
    connect(btnCargarProductosCSV, &QPushButton::clicked, this, &VentanaPrincipal::onCargarProductosCSV);
    connect(btnAgregarSucursal, &QPushButton::clicked, this, &VentanaPrincipal::onAgregarSucursal);
    connect(btnModificarSucursal, &QPushButton::clicked, this, &VentanaPrincipal::onModificarSucursal);
    connect(btnEliminarSucursal, &QPushButton::clicked, this, &VentanaPrincipal::onEliminarSucursal);
    connect(btnInsertar, &QPushButton::clicked, this, &VentanaPrincipal::onInsertarProducto);
    connect(btnEliminar, &QPushButton::clicked, this, &VentanaPrincipal::onEliminarProducto);
    connect(btnBuscar, &QPushButton::clicked, this, &VentanaPrincipal::onBuscarProducto);
    connect(btnReportes, &QPushButton::clicked, this, &VentanaPrincipal::onGenerarReportes);
    connect(btnBenchmarking, &QPushButton::clicked, this, &VentanaPrincipal::onBenchmarking);

    connect(btnIrASucursal, &QPushButton::clicked, this, &VentanaPrincipal::onIrASucursal);
    connect(comboSucursales, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &VentanaPrincipal::onCambioSucursalSeleccionada);
    
    // Barra de búsqueda avanzada
    connect(btnEjecutarBusqueda, &QPushButton::clicked, this, &VentanaPrincipal::ejecutarBusquedaAvanzada);
    connect(btnMostrarTodos, &QPushButton::clicked, this, &VentanaPrincipal::actualizarTabla);
    connect(comboFiltroBusqueda, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            this, &VentanaPrincipal::onCambioFiltroBusqueda);
    
    // Enter en input ejecuta búsqueda
    connect(inputBusqueda, &QLineEdit::returnPressed, this, &VentanaPrincipal::ejecutarBusquedaAvanzada);
    connect(inputBusquedaFin, &QLineEdit::returnPressed, this, &VentanaPrincipal::ejecutarBusquedaAvanzada);

    // Tick de simulación en modo paso temporal de 1 segundo.
    connect(temporizadorSimulacion, &QTimer::timeout, this, &VentanaPrincipal::onTickSimulacion);
}

// onTickSimulacion: Avanza un tick de la simulación y registra trazas en consola.
// Complejidad: O(e), donde e es la cantidad de eventos activos.
void VentanaPrincipal::onTickSimulacion() {
    if (simulador == nullptr) {
        return;
    }

    simulador->avanzar();
    std::cout << "[VentanaPrincipal] Tick de simulacion procesado.\n";
}

// refrescarComboSucursales: Reconstruye el combo con ID y nombre de sucursales.
// Complejidad: O(s), donde s es la cantidad de sucursales.
void VentanaPrincipal::refrescarComboSucursales() {
    if (comboSucursales == nullptr || sucursales == nullptr) {
        return;
    }

    const bool habiaSenales = comboSucursales->blockSignals(true);
    comboSucursales->clear();

    int indiceSeleccionar = -1;
    int indiceActual = 0;
    ListaSucursales::NodoSucursal* nodo = sucursales->obtenerPrimero();
    while (nodo != nullptr) {
        Sucursal* sucursal = nodo->dato;
        if (sucursal != nullptr) {
            const QString etiqueta = QString("%1 - %2")
                                         .arg(sucursal->obtenerId())
                                         .arg(QString::fromStdString(sucursal->obtenerNombre()));
            comboSucursales->addItem(etiqueta, sucursal->obtenerId());
            if (sucursalActual != nullptr && sucursal->obtenerId() == sucursalActual->obtenerId()) {
                indiceSeleccionar = indiceActual;
            }
            ++indiceActual;
        }
        nodo = nodo->siguiente;
    }

    if (comboSucursales->count() > 0) {
        if (indiceSeleccionar >= 0) {
            comboSucursales->setCurrentIndex(indiceSeleccionar);
        } else {
            comboSucursales->setCurrentIndex(0);
            seleccionarSucursal(obtenerIdSucursalSeleccionada());
        }
    } else {
        sucursalActual = nullptr;
    }

    comboSucursales->blockSignals(habiaSenales);
    actualizarBarraEstado();
}

// obtenerIdSucursalSeleccionada: Obtiene el id asociado al item seleccionado en combo.
// Complejidad: O(1).
int VentanaPrincipal::obtenerIdSucursalSeleccionada() const {
    if (comboSucursales == nullptr || comboSucursales->currentIndex() < 0) {
        return -1;
    }
    return comboSucursales->currentData().toInt();
}

// dibujarGrafo: Delega la visualización del grafo al componente especializado.
// Complejidad: O(V + E), donde V son sucursales y E conexiones.
void VentanaPrincipal::dibujarGrafo() {
    if (visorGrafo == nullptr || grafo == nullptr) {
        return;
    }
    visorGrafo->dibujarGrafo(grafo, obtenerIdSucursalSeleccionada());
}

// actualizarBarraEstado: Muestra sucursal actual y cantidad de productos en la barra de estado.
// Complejidad: O(p), donde p es la cantidad de productos visibles de la sucursal.
void VentanaPrincipal::actualizarBarraEstado() {
    if (statusBar() == nullptr) {
        return;
    }

    if (!haySucursalSeleccionada()) {
        statusBar()->showMessage("Sucursal actual: [ninguna] | Productos: 0");
        return;
    }

    const int cantidadProductos =
        static_cast<int>(sucursalActual->obtenerListaGeneral()->obtenerTodos().size());
    const QString mensaje = QString("Sucursal actual: %1 | Productos: %2")
                                .arg(QString::fromStdString(sucursalActual->obtenerNombre()))
                                .arg(cantidadProductos);
    statusBar()->showMessage(mensaje);
}

// SLOT: Carga archivo CSV de sucursales.
// Complejidad: O(n) por cantidad de líneas procesadas.
void VentanaPrincipal::onCargarSucursalesCSV() {
    QString archivo = QFileDialog::getOpenFileName(
        this,
        "Seleccionar archivo CSV de sucursales",
        "data/",
        "Archivos CSV (*.csv);;Todos los archivos (*.*)"
    );

    if (archivo.isEmpty()) {
        return;
    }

    if (!cargadorCSV.cargarSucursales(archivo.toStdString(), *sucursales, *grafo)) {
        QMessageBox::critical(this, "Error", "No se pudo cargar el archivo de sucursales.");
        return;
    }

    refrescarComboSucursales();
    dibujarGrafo();
    actualizarTabla();
    QMessageBox::information(this, "Carga completada", "Sucursales cargadas correctamente.");
}

// SLOT: Carga archivo CSV de conexiones.
// Complejidad: O(m) por cantidad de conexiones.
void VentanaPrincipal::onCargarConexionesCSV() {
    QString archivo = QFileDialog::getOpenFileName(
        this,
        "Seleccionar archivo CSV de conexiones",
        "data/",
        "Archivos CSV (*.csv);;Todos los archivos (*.*)"
    );

    if (archivo.isEmpty()) {
        return;
    }

    if (!cargadorCSV.cargarConexiones(archivo.toStdString(), *grafo)) {
        QMessageBox::critical(this, "Error", "No se pudo cargar el archivo de conexiones.");
        return;
    }

    dibujarGrafo();
    QMessageBox::information(this, "Carga completada", "Conexiones cargadas correctamente.");
}

// SLOT: Carga productos CSV para la sucursal seleccionada.
// Complejidad: O(n), por líneas de productos procesadas.
void VentanaPrincipal::onCargarProductosCSV() {
    if (!haySucursalSeleccionada()) {
        mostrarAdvertenciaSucursalNoSeleccionada();
        return;
    }

    QString archivo = QFileDialog::getOpenFileName(
        this,
        "Seleccionar archivo CSV de productos",
        "data/",
        "Archivos CSV (*.csv);;Todos los archivos (*.*)"
    );

    if (archivo.isEmpty()) {
        return;
    }

    const int idSucursal = sucursalActual->obtenerId();
    const bool cargado = cargadorCSV.cargar(
        archivo.toStdString(),
        *(sucursalActual->obtenerListaGeneral()),
        *(sucursalActual->obtenerListaOrdenada()),
        *(sucursalActual->obtenerTablaHash()),
        *(sucursalActual->obtenerArbolAVL()),
        *(sucursalActual->obtenerArbolB()),
        *(sucursalActual->obtenerArbolBPlus()),
        idSucursal
    );

    if (!cargado) {
        QMessageBox::critical(this, "Error", "No se pudo cargar el archivo de productos.");
        return;
    }

    actualizarTabla();
    QMessageBox::information(this, "Carga completada", "Productos cargados correctamente.");
}

// SLOT: Agrega sucursal desde formulario.
// Complejidad: O(s) por validación de id duplicado.
void VentanaPrincipal::onAgregarSucursal() {
    QDialog dialogo(this);
    dialogo.setWindowTitle("Agregar Sucursal");
    dialogo.setMinimumWidth(420);

    QVBoxLayout* layout = new QVBoxLayout(&dialogo);
    QFormLayout* form = new QFormLayout();

    QLineEdit* inputId = new QLineEdit(&dialogo);
    inputId->setValidator(new QIntValidator(0, 999999, &dialogo));
    QLineEdit* inputNombre = new QLineEdit(&dialogo);
    QLineEdit* inputUbicacion = new QLineEdit(&dialogo);
    QLineEdit* inputIngreso = new QLineEdit(&dialogo);
    inputIngreso->setValidator(new QIntValidator(0, 100000, &dialogo));
    QLineEdit* inputPreparacion = new QLineEdit(&dialogo);
    inputPreparacion->setValidator(new QIntValidator(0, 100000, &dialogo));
    QLineEdit* inputDespacho = new QLineEdit(&dialogo);
    inputDespacho->setValidator(new QIntValidator(0, 100000, &dialogo));

    form->addRow("ID:", inputId);
    form->addRow("Nombre:", inputNombre);
    form->addRow("Ubicación:", inputUbicacion);
    form->addRow("Tiempo ingreso:", inputIngreso);
    form->addRow("Tiempo traspaso:", inputPreparacion);
    form->addRow("Intervalo despacho:", inputDespacho);
    layout->addLayout(form);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                      Qt::Horizontal,
                                                      &dialogo);
    layout->addWidget(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() != QDialog::Accepted) {
        return;
    }

    if (inputId->text().trimmed().isEmpty() ||
        inputNombre->text().trimmed().isEmpty() ||
        inputUbicacion->text().trimmed().isEmpty() ||
        inputIngreso->text().trimmed().isEmpty() ||
        inputPreparacion->text().trimmed().isEmpty() ||
        inputDespacho->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Datos incompletos", "Complete todos los campos.");
        return;
    }

    const int id = inputId->text().toInt();
    if (sucursales->buscar(id) != nullptr) {
        QMessageBox::warning(this, "ID duplicado", "Ya existe una sucursal con ese ID.");
        return;
    }

    agregarSucursal(id,
                    inputNombre->text().toStdString(),
                    inputUbicacion->text().toStdString(),
                    inputIngreso->text().toInt(),
                    inputPreparacion->text().toInt(),
                    inputDespacho->text().toInt());
    seleccionarSucursal(id);
    refrescarComboSucursales();
    dibujarGrafo();
    actualizarTabla();
}

// SLOT: Modifica metadatos de la sucursal seleccionada.
// Complejidad: O(1).
void VentanaPrincipal::onModificarSucursal() {
    if (!haySucursalSeleccionada()) {
        mostrarAdvertenciaSucursalNoSeleccionada();
        return;
    }

    QDialog dialogo(this);
    dialogo.setWindowTitle("Modificar Sucursal");
    dialogo.setMinimumWidth(420);

    QVBoxLayout* layout = new QVBoxLayout(&dialogo);
    QFormLayout* form = new QFormLayout();

    QLineEdit* inputNombre = new QLineEdit(QString::fromStdString(sucursalActual->obtenerNombre()), &dialogo);
    QLineEdit* inputUbicacion = new QLineEdit(QString::fromStdString(sucursalActual->obtenerUbicacion()), &dialogo);
    QLineEdit* inputIngreso = new QLineEdit(QString::number(sucursalActual->obtenerTiempoIngreso()), &dialogo);
    inputIngreso->setValidator(new QIntValidator(0, 100000, &dialogo));
    QLineEdit* inputPreparacion = new QLineEdit(QString::number(sucursalActual->obtenerTiempoPreparacion()), &dialogo);
    inputPreparacion->setValidator(new QIntValidator(0, 100000, &dialogo));
    QLineEdit* inputDespacho = new QLineEdit(QString::number(sucursalActual->obtenerIntervaloDespacho()), &dialogo);
    inputDespacho->setValidator(new QIntValidator(0, 100000, &dialogo));

    form->addRow("Nombre:", inputNombre);
    form->addRow("Ubicación:", inputUbicacion);
    form->addRow("Tiempo ingreso:", inputIngreso);
    form->addRow("Tiempo traspaso:", inputPreparacion);
    form->addRow("Intervalo despacho:", inputDespacho);
    layout->addLayout(form);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                      Qt::Horizontal,
                                                      &dialogo);
    layout->addWidget(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() != QDialog::Accepted) {
        return;
    }

    sucursalActual->establecerNombre(inputNombre->text().trimmed().toStdString());
    sucursalActual->establecerUbicacion(inputUbicacion->text().trimmed().toStdString());
    sucursalActual->establecerTiempoIngreso(inputIngreso->text().toInt());
    sucursalActual->establecerTiempoPreparacion(inputPreparacion->text().toInt());
    sucursalActual->establecerIntervaloDespacho(inputDespacho->text().toInt());

    refrescarComboSucursales();
    dibujarGrafo();
    actualizarBarraEstado();
}

// SLOT: Elimina la sucursal seleccionada de la lista enlazada.
// Complejidad: O(s).
void VentanaPrincipal::onEliminarSucursal() {
    const int idSeleccionado = obtenerIdSucursalSeleccionada();
    if (idSeleccionado < 0) {
        QMessageBox::warning(this, "Sin selección", "No hay sucursal seleccionada.");
        return;
    }

    Sucursal* sucursalEliminar = sucursales->buscar(idSeleccionado);
    if (sucursalEliminar == nullptr) {
        QMessageBox::warning(this, "No encontrada", "La sucursal seleccionada no existe.");
        return;
    }

    const QMessageBox::StandardButton respuesta = QMessageBox::question(
        this,
        "Eliminar sucursal",
        QString("¿Desea eliminar la sucursal %1?").arg(idSeleccionado),
        QMessageBox::Yes | QMessageBox::No
    );
    if (respuesta != QMessageBox::Yes) {
        return;
    }

    const bool eliminada = sucursales->eliminar(idSeleccionado);
    if (!eliminada) {
        QMessageBox::critical(this, "Error", "No se pudo eliminar la sucursal.");
        return;
    }

    if (sucursalActual == sucursalEliminar) {
        sucursalActual = nullptr;
    }
    delete sucursalEliminar;

    refrescarComboSucursales();
    dibujarGrafo();
    actualizarTabla();
}

// SLOT: Cambia la sucursal activa según selección del combo.
// Complejidad: O(s).
void VentanaPrincipal::onCambioSucursalSeleccionada(int indice) {
    Q_UNUSED(indice)
    const int id = obtenerIdSucursalSeleccionada();
    if (id < 0) {
        return;
    }

    seleccionarSucursal(id);
    actualizarTabla();
    dibujarGrafo();
}

// SLOT: Acción de navegación hacia sucursal seleccionada.
// Complejidad: O(1).
void VentanaPrincipal::onIrASucursal() {
    const int id = obtenerIdSucursalSeleccionada();
    if (id < 0) {
        QMessageBox::warning(this, "Sin selección", "Seleccione una sucursal.");
        return;
    }

    seleccionarSucursal(id);
    std::cout << "Viajando a sucursal [" << id << "]\n";
    actualizarTabla();
    dibujarGrafo();
}

// SLOT: Actualizar tabla de productos
// Obtiene todos los productos de la lista y los muestra en el QTableWidget
void VentanaPrincipal::actualizarTabla() {
    const int idCombo = obtenerIdSucursalSeleccionada();
    if (idCombo >= 0) {
        seleccionarSucursal(idCombo);
    }

    // Deshabilitar ordenamiento temporalmente para evitar problemas
    tablaProductos->setSortingEnabled(false);
    
    // Limpiar tabla
    tablaProductos->setRowCount(0);

    if (!haySucursalSeleccionada()) {
        tablaProductos->setSortingEnabled(true);
        actualizarBarraEstado();
        return;
    }

    // Obtener todos los productos como vector (buffer temporal)
    std::vector<Producto*> productos = sucursalActual->obtenerListaGeneral()->obtenerTodos();

    // Llenar la tabla
    for (size_t i = 0; i < productos.size(); ++i) {
        Producto* prod = productos[i];
        if (prod == nullptr) continue;

        int fila = tablaProductos->rowCount();
        tablaProductos->insertRow(fila);

        // Columna 0: Código de barras
        tablaProductos->setItem(fila, 0, 
            new QTableWidgetItem(QString::fromStdString(prod->codigoBarras)));

        // Columna 1: Nombre
        tablaProductos->setItem(fila, 1, 
            new QTableWidgetItem(QString::fromStdString(prod->nombre)));

        // Columna 2: Categoría
        tablaProductos->setItem(fila, 2, 
            new QTableWidgetItem(QString::fromStdString(prod->categoria)));

        // Columna 3: Precio (formateado con 2 decimales)
        QTableWidgetItem* itemPrecio = new QTableWidgetItem(
            QString("Q %1").arg(prod->precio, 0, 'f', 2));
        itemPrecio->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tablaProductos->setItem(fila, 3, itemPrecio);

        // Columna 4: Stock
        QTableWidgetItem* itemStock = new QTableWidgetItem(
            QString::number(prod->stock));
        itemStock->setTextAlignment(Qt::AlignCenter);
        tablaProductos->setItem(fila, 4, itemStock);

        // Columna 5: Fecha de caducidad
        tablaProductos->setItem(fila, 5, 
            new QTableWidgetItem(QString::fromStdString(prod->fechaCaducidad)));
    }

    // Reactivar ordenamiento
    tablaProductos->setSortingEnabled(true);
    actualizarBarraEstado();
}

// SLOT: Insertar nuevo producto
// Usa un QDialog con QFormLayout para mejor experiencia de usuario
void VentanaPrincipal::onInsertarProducto() {
    if (!haySucursalSeleccionada()) {
        mostrarAdvertenciaSucursalNoSeleccionada();
        return;
    }

    // Crear diálogo de inserción
    QDialog dialogo(this);
    dialogo.setWindowTitle("Insertar Nuevo Producto");
    dialogo.setMinimumWidth(400);
    
    // Layout principal del diálogo
    QVBoxLayout* layoutDialogo = new QVBoxLayout(&dialogo);
    layoutDialogo->setSpacing(15);
    layoutDialogo->setContentsMargins(20, 20, 20, 20);
    
    // Título del formulario
    QLabel* lblTituloForm = new QLabel("Ingrese los datos del producto:");
    lblTituloForm->setStyleSheet("font-size: 15px; font-weight: bold; color: #004D98; margin-bottom: 10px;");
    layoutDialogo->addWidget(lblTituloForm);
    
    // Formulario con campos
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(12);
    formLayout->setLabelAlignment(Qt::AlignRight);
    
    QLineEdit* inputNombre = new QLineEdit();
    inputNombre->setPlaceholderText("Ej: Leche Entera 1L");
    
    QLineEdit* inputCodigo = new QLineEdit();
    inputCodigo->setPlaceholderText("Ej: 1001");
    
    QLineEdit* inputCategoria = new QLineEdit();
    inputCategoria->setPlaceholderText("Ej: Lacteos");
    
    QLineEdit* inputFecha = new QLineEdit();
    inputFecha->setPlaceholderText("YYYY-MM-DD");
    inputFecha->setText("2026-12-31");
    
    QLineEdit* inputMarca = new QLineEdit();
    inputMarca->setPlaceholderText("Ej: Lala");
    
    QLineEdit* inputPrecio = new QLineEdit();
    inputPrecio->setPlaceholderText("0.00");
    inputPrecio->setValidator(new QDoubleValidator(0, 100000, 2, &dialogo));
    
    QLineEdit* inputStock = new QLineEdit();
    inputStock->setPlaceholderText("0");
    inputStock->setValidator(new QIntValidator(0, 1000000, &dialogo));
    
    formLayout->addRow("Nombre:", inputNombre);
    formLayout->addRow("Codigo de Barras:", inputCodigo);
    formLayout->addRow("Categoria:", inputCategoria);
    formLayout->addRow("Fecha Caducidad:", inputFecha);
    formLayout->addRow("Marca:", inputMarca);
    formLayout->addRow("Precio (Q):", inputPrecio);
    formLayout->addRow("Stock:", inputStock);
    
    layoutDialogo->addLayout(formLayout);
    
    // Separador
    QFrame* linea = new QFrame();
    linea->setFrameShape(QFrame::HLine);
    linea->setStyleSheet("background-color: #ddd;");
    layoutDialogo->addWidget(linea);
    
    // Botones de acción
    QHBoxLayout* layoutBotones = new QHBoxLayout();
    layoutBotones->setSpacing(10);
    
    QPushButton* btnCancelar = new QPushButton("Cancelar");
    btnCancelar->setStyleSheet("background-color: #6c757d;");
    btnCancelar->setFixedWidth(100);
    
    QPushButton* btnAceptar = new QPushButton("Insertar");
    btnAceptar->setDefault(true);
    btnAceptar->setFixedWidth(100);
    
    layoutBotones->addStretch();
    layoutBotones->addWidget(btnCancelar);
    layoutBotones->addWidget(btnAceptar);
    
    layoutDialogo->addLayout(layoutBotones);
    
    // Conexiones
    connect(btnCancelar, &QPushButton::clicked, &dialogo, &QDialog::reject);
    connect(btnAceptar, &QPushButton::clicked, &dialogo, &QDialog::accept);
    
    // Ejecutar diálogo
    if (dialogo.exec() != QDialog::Accepted) {
        return;
    }
    
    // Validar campos
    QString nombre = inputNombre->text().trimmed();
    QString codigo = inputCodigo->text().trimmed();
    QString categoria = inputCategoria->text().trimmed();
    QString fecha = inputFecha->text().trimmed();
    QString marca = inputMarca->text().trimmed();
    QString precioStr = inputPrecio->text().trimmed();
    QString stockStr = inputStock->text().trimmed();
    
    if (nombre.isEmpty() || codigo.isEmpty() || categoria.isEmpty() || 
        fecha.isEmpty() || marca.isEmpty() || precioStr.isEmpty() || stockStr.isEmpty()) {
        QMessageBox::warning(this, "Campos Incompletos",
            "Por favor, complete todos los campos del formulario.");
        return;
    }
    
    double precio = precioStr.toDouble();
    int stock = stockStr.toInt();
    
    // Crear nuevo producto
    Producto* nuevoProducto = new Producto(
        nombre.toStdString(),
        codigo.toStdString(),
        categoria.toStdString(),
        fecha.toStdString(),
        marca.toStdString(),
        precio,
        stock
    );

    // Insertar transaccionalmente en la sucursal seleccionada.
    if (!sucursalActual->agregarProducto(nuevoProducto)) {
        delete nuevoProducto;
        QMessageBox::warning(this, "Error",
            "No se pudo insertar el producto en la sucursal seleccionada.");
        return;
    }

    // Se programa traslado inicial en la misma sucursal para activar el flujo
    // de estados y trazas de simulación desde consola.
    if (simulador != nullptr && sucursalActual != nullptr) {
        const int idSucursal = sucursalActual->obtenerId();
        simulador->programarEnvio(nuevoProducto, idSucursal, idSucursal, true);
    }

    // Actualizar tabla
    actualizarTabla();

    QMessageBox::information(this, "Éxito",
        QString("Producto '%1' insertado correctamente.").arg(nombre));
}

// regenerarVisualizaciones: Regenera DOT y PNG de los árboles con el estado
// actual en memoria para evitar reportes obsoletos tras operaciones CRUD.
// Complejidad: O(n) por árbol al recorrer todos sus nodos.
void VentanaPrincipal::regenerarVisualizaciones() {
    if (!haySucursalSeleccionada()) {
        return;
    }

    QDir dir;
    if (!dir.exists("data")) {
        dir.mkpath("data");
    }

    sucursalActual->obtenerArbolAVL()->generarImagen();
    sucursalActual->obtenerArbolB()->generarImagen();
    sucursalActual->obtenerArbolBPlus()->generarImagen();
}

// SLOT: Eliminar producto
// Solicita código de barras y elimina el producto de todas las estructuras
void VentanaPrincipal::onEliminarProducto() {
    if (!haySucursalSeleccionada()) {
        mostrarAdvertenciaSucursalNoSeleccionada();
        return;
    }

    bool ok;
    QString codigo = QInputDialog::getText(this, "Eliminar Producto",
        "Ingrese el código de barras del producto a eliminar:",
        QLineEdit::Normal, "", &ok);

    if (!ok || codigo.isEmpty()) {
        return;
    }

    // Buscar el producto primero
    Producto* producto = sucursalActual->buscarPorCodigo(codigo.toStdString());
    
    if (producto == nullptr) {
        QMessageBox::warning(this, "No encontrado",
            QString("No se encontró ningún producto con el código '%1'.").arg(codigo));
        return;
    }

    // Confirmar eliminación
    QMessageBox::StandardButton respuesta = QMessageBox::question(
        this, "Confirmar Eliminación",
        QString("¿Está seguro de eliminar el producto?\n\n"
                "Nombre: %1\n"
                "Código: %2\n"
                "Categoría: %3")
            .arg(QString::fromStdString(producto->nombre))
            .arg(QString::fromStdString(producto->codigoBarras))
            .arg(QString::fromStdString(producto->categoria)),
        QMessageBox::Yes | QMessageBox::No
    );

    if (respuesta != QMessageBox::Yes) {
        return;
    }

    // Eliminar de forma transaccional en todas las estructuras.
    std::string codigoStr = codigo.toStdString();
    const bool eliminacionCorrecta = sucursalActual->eliminarProducto(codigoStr);

    if (!eliminacionCorrecta) {
        QMessageBox::critical(
            this,
            "Error de Eliminacion",
            "No se pudo completar la eliminación en todas las estructuras.\n"
            "Se aplicó rollback para preservar consistencia."
        );
        return;
    }

    // Actualizar tabla
    actualizarTabla();
    regenerarVisualizaciones();

    QMessageBox::information(this, "Éxito",
        "Producto eliminado correctamente.");
}

// SLOT: Buscar producto por código de barras
// Usa QInputDialog para pedir el código y busca en la TablaHash
void VentanaPrincipal::onBuscarProducto() {
    if (!haySucursalSeleccionada()) {
        mostrarAdvertenciaSucursalNoSeleccionada();
        return;
    }

    bool ok;
    QString codigo = QInputDialog::getText(
        this,
        "Buscar Producto",
        "Ingrese el código de barras:",
        QLineEdit::Normal,
        "",
        &ok
    );

    if (!ok || codigo.isEmpty()) {
        return;  // Usuario canceló
    }

    // Buscar en la tabla hash
    std::string codigoStr = codigo.toStdString();
    Producto* producto = sucursalActual->buscarPorCodigo(codigoStr);

    if (producto == nullptr) {
        QMessageBox::warning(
            this,
            "No Encontrado",
            QString("No se encontró ningún producto con el código:\n%1").arg(codigo)
        );
        return;
    }

    // Mostrar información del producto
    QString info = QString(
        "===================================\n"
        "      PRODUCTO ENCONTRADO\n"
        "===================================\n\n"
        "Nombre: %1\n"
        "Codigo: %2\n"
        "Categoria: %3\n"
        "Marca: %4\n"
        "Precio: Q %5\n"
        "Stock: %6 unidades\n"
        "Caducidad: %7\n"
    )
    .arg(QString::fromStdString(producto->nombre))
    .arg(QString::fromStdString(producto->codigoBarras))
    .arg(QString::fromStdString(producto->categoria))
    .arg(QString::fromStdString(producto->marca))
    .arg(producto->precio, 0, 'f', 2)
    .arg(producto->stock)
    .arg(QString::fromStdString(producto->fechaCaducidad));

    QMessageBox::information(this, "Producto Encontrado", info);
}

// SLOT: Generar reportes Graphviz
// Llama a generarImagen() de AVL, B y B+
void VentanaPrincipal::onGenerarReportes() {
    if (!haySucursalSeleccionada()) {
        mostrarAdvertenciaSucursalNoSeleccionada();
        return;
    }

    if (sucursalActual->obtenerListaGeneral()->estaVacia()) {
        QMessageBox::warning(this, "Sin Datos",
            "No hay productos cargados para generar reportes.\n"
            "Por favor, cargue un archivo CSV primero.");
        return;
    }

    // Asegurar que el directorio 'data' existe
    QDir dir;
    if (!dir.exists("data")) {
        dir.mkpath("data");
    }

    // Regenerar visualizaciones desde el estado vigente en memoria.
    regenerarVisualizaciones();

    // Crear diálogo visor con pestañas
    QDialog* visor = new QDialog(this);
    visor->setWindowTitle("Visor de Arboles - Reportes Graphviz");
    visor->resize(1000, 700);
    visor->setWindowFlags(visor->windowFlags() | Qt::WindowMaximizeButtonHint);
    
    QVBoxLayout* layoutVisor = new QVBoxLayout(visor);
    
    // TabWidget para las 3 pestañas
    QTabWidget* tabs = new QTabWidget(visor);
    
    // Función auxiliar para crear pestaña con imagen
    auto crearPestana = [&](const QString& titulo, const QString& rutaImagen) -> QWidget* {
        QWidget* pestana = new QWidget();
        pestana->setObjectName(titulo);
        QVBoxLayout* layoutPestana = new QVBoxLayout(pestana);
        layoutPestana->setContentsMargins(5, 5, 5, 5);
        
        // Controles de zoom
        QHBoxLayout* layoutControles = new QHBoxLayout();
        QPushButton* btnZoomIn = new QPushButton("+");
        btnZoomIn->setFixedSize(40, 30);
        QPushButton* btnZoomOut = new QPushButton("-");
        btnZoomOut->setFixedSize(40, 30);
        QPushButton* btnResetZoom = new QPushButton("Reset");
        btnResetZoom->setFixedWidth(60);
        QLabel* lblZoom = new QLabel("Zoom:");
        
        layoutControles->addWidget(lblZoom);
        layoutControles->addWidget(btnZoomOut);
        layoutControles->addWidget(btnZoomIn);
        layoutControles->addWidget(btnResetZoom);
        layoutControles->addStretch();
        
        // GraphicsView y Scene
        QGraphicsScene* scene = new QGraphicsScene(pestana);
        QGraphicsView* view = new QGraphicsView(scene, pestana);
        view->setDragMode(QGraphicsView::ScrollHandDrag);
        view->setRenderHint(QPainter::Antialiasing);
        view->setRenderHint(QPainter::SmoothPixmapTransform);
        view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        view->setBackgroundBrush(QBrush(QColor(240, 240, 240)));
        
        // Cargar imagen
        QPixmap pixmap(rutaImagen);
        if (!pixmap.isNull()) {
            scene->addPixmap(pixmap);
            scene->setSceneRect(pixmap.rect());
        } else {
            scene->addText("No se pudo cargar: " + rutaImagen);
        }
        
        // Conectar botones de zoom
        connect(btnZoomIn, &QPushButton::clicked, [view]() {
            view->scale(1.25, 1.25);
        });
        connect(btnZoomOut, &QPushButton::clicked, [view]() {
            view->scale(0.8, 0.8);
        });
        connect(btnResetZoom, &QPushButton::clicked, [view]() {
            view->resetTransform();
        });
        
        layoutPestana->addLayout(layoutControles);
        layoutPestana->addWidget(view);
        
        return pestana;
    };
    
    // Crear las 3 pestañas
    tabs->addTab(crearPestana("Arbol AVL", "data/avl.png"), "Arbol AVL");
    tabs->addTab(crearPestana("Arbol B", "data/arbol_b.png"), "Arbol B");
    tabs->addTab(crearPestana("Arbol B+", "data/arbol_bplus.png"), "Arbol B+");
    
    layoutVisor->addWidget(tabs);
    
    // Botón cerrar
    QPushButton* btnCerrar = new QPushButton("Cerrar");
    btnCerrar->setFixedWidth(100);
    connect(btnCerrar, &QPushButton::clicked, visor, &QDialog::accept);
    
    QHBoxLayout* layoutBoton = new QHBoxLayout();
    layoutBoton->addStretch();
    layoutBoton->addWidget(btnCerrar);
    layoutVisor->addLayout(layoutBoton);
    
    visor->exec();
    delete visor;
}

// SLOT: Ejecutar pruebas de rendimiento (Benchmarking)
// Muestra resultados en QDialog con QTextEdit monoespaciado
void VentanaPrincipal::onBenchmarking() {
    if (!haySucursalSeleccionada()) {
        mostrarAdvertenciaSucursalNoSeleccionada();
        return;
    }

    if (sucursalActual->obtenerListaGeneral()->estaVacia()) {
        QMessageBox::warning(this, "Sin Datos",
            "No hay productos cargados para ejecutar benchmarking.\n"
            "Por favor, cargue un archivo CSV primero.");
        return;
    }

    // Ejecutar benchmarking y obtener resultado como string
    MedidorRendimiento medidor;
    std::string resultado = medidor.ejecutarPruebasBusqueda(
        *(sucursalActual->obtenerListaGeneral()),
        *(sucursalActual->obtenerListaOrdenada()),
        *(sucursalActual->obtenerArbolAVL()),
        *(sucursalActual->obtenerTablaHash())
    );

    // Crear diálogo para mostrar resultados
    QDialog* dialogo = new QDialog(this);
    dialogo->setWindowTitle("Resultados de Benchmarking");
    dialogo->resize(700, 550);
    dialogo->setWindowFlags(dialogo->windowFlags() | Qt::WindowMaximizeButtonHint);
    
    QVBoxLayout* layout = new QVBoxLayout(dialogo);
    layout->setContentsMargins(15, 15, 15, 15);
    
    // Título
    QLabel* lblTitulo = new QLabel("Pruebas de Rendimiento - Estructuras de Datos");
    lblTitulo->setStyleSheet("font-size: 16px; font-weight: bold; color: #004D98; margin-bottom: 10px;");
    lblTitulo->setAlignment(Qt::AlignCenter);
    layout->addWidget(lblTitulo);
    
    // QTextEdit con fuente monoespaciada
    QTextEdit* textEdit = new QTextEdit(dialogo);
    textEdit->setReadOnly(true);
    textEdit->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    textEdit->setText(QString::fromStdString(resultado));
    textEdit->setStyleSheet(
        "QTextEdit {"
        "  background-color: #1e1e1e;"
        "  color: #d4d4d4;"
        "  border: 1px solid #333;"
        "  border-radius: 4px;"
        "  padding: 10px;"
        "  font-size: 12px;"
        "}"
    );
    layout->addWidget(textEdit);
    
    // Botón cerrar
    QHBoxLayout* layoutBoton = new QHBoxLayout();
    QPushButton* btnCerrar = new QPushButton("Cerrar");
    btnCerrar->setFixedWidth(100);
    connect(btnCerrar, &QPushButton::clicked, dialogo, &QDialog::accept);
    layoutBoton->addStretch();
    layoutBoton->addWidget(btnCerrar);
    layout->addLayout(layoutBoton);
    
    dialogo->exec();
    delete dialogo;
}

// SLOT: Cambio de filtro de búsqueda
// Muestra/oculta el input de fecha fin según selección

void VentanaPrincipal::onCambioFiltroBusqueda(int indice) {
    // Índice 3 = "Rango de Fecha (B)".
    bool esRango = (indice == 3);
    inputBusquedaFin->setVisible(esRango);
    
    // Actualizar placeholder según tipo de búsqueda
    switch (indice) {
        case 0:  // Codigo de barras (Hash)
            inputBusqueda->setPlaceholderText("Codigo de barras...");
            break;
        case 1:  // Nombre (AVL)
            inputBusqueda->setPlaceholderText("Nombre del producto...");
            break;
        case 2:  // Categoria (B+)
            inputBusqueda->setPlaceholderText("Categoria (ej: Lacteos)...");
            break;
        case 3:  // Rango de Fecha (B)
            inputBusqueda->setPlaceholderText("Fecha inicio (YYYY-MM-DD)");
            break;
    }
    inputBusqueda->clear();
    inputBusquedaFin->clear();
}

// SLOT: Ejecutar búsqueda avanzada según filtro seleccionado

void VentanaPrincipal::ejecutarBusquedaAvanzada() {
    if (!haySucursalSeleccionada()) {
        mostrarAdvertenciaSucursalNoSeleccionada();
        return;
    }

    QString termino = inputBusqueda->text().trimmed();
    
    if (termino.isEmpty()) {
        QMessageBox::warning(this, "Campo Vacio", 
            "Por favor, ingrese un termino de busqueda.");
        return;
    }
    
    int filtro = comboFiltroBusqueda->currentIndex();
    std::vector<Producto*> resultados;
    
    switch (filtro) {
        case 0: {  // Codigo de barras (Hash)
            Producto* p = sucursalActual->buscarPorCodigo(termino.toStdString());
            if (p != nullptr) {
                resultados.push_back(p);
            }
            break;
        }
        case 1: {  // Nombre (AVL)
            Producto* p = sucursalActual->buscarPorNombre(termino.toStdString());
            if (p != nullptr) {
                resultados.push_back(p);
            }
            break;
        }
        case 2: {  // Categoria (B+)
            resultados = sucursalActual->buscarPorCategoria(termino.toStdString());
            break;
        }
        case 3: {  // Rango de Fecha (B)
            QString fechaFin = inputBusquedaFin->text().trimmed();
            if (fechaFin.isEmpty()) {
                QMessageBox::warning(this, "Fecha Fin Requerida",
                    "Para busqueda por rango, ingrese tambien la fecha de fin.");
                return;
            }
            resultados = sucursalActual->buscarPorRangoFecha(termino.toStdString(),
                                                            fechaFin.toStdString());
            break;
        }
    }
    
    // Mostrar resultados en la tabla
    mostrarProductosEnTabla(resultados);
    
    // Mensaje informativo
    if (resultados.empty()) {
        QMessageBox::information(this, "Sin Resultados",
            "No se encontraron productos con el criterio especificado.");
    } else {
        QString msg = QString("Se encontraron %1 producto(s).").arg(resultados.size());
        statusBar()->showMessage(msg, 5000);
    }
}

// Mostrar un conjunto de productos en la tabla

void VentanaPrincipal::mostrarProductosEnTabla(const std::vector<Producto*>& productos) {
    tablaProductos->setSortingEnabled(false);
    tablaProductos->setRowCount(0);
    
    for (size_t i = 0; i < productos.size(); ++i) {
        Producto* prod = productos[i];
        if (prod == nullptr) continue;
        
        int fila = tablaProductos->rowCount();
        tablaProductos->insertRow(fila);
        
        tablaProductos->setItem(fila, 0, 
            new QTableWidgetItem(QString::fromStdString(prod->codigoBarras)));
        tablaProductos->setItem(fila, 1, 
            new QTableWidgetItem(QString::fromStdString(prod->nombre)));
        tablaProductos->setItem(fila, 2, 
            new QTableWidgetItem(QString::fromStdString(prod->categoria)));
        
        QTableWidgetItem* itemPrecio = new QTableWidgetItem(
            QString("Q %1").arg(prod->precio, 0, 'f', 2));
        itemPrecio->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tablaProductos->setItem(fila, 3, itemPrecio);
        
        QTableWidgetItem* itemStock = new QTableWidgetItem(QString::number(prod->stock));
        itemStock->setTextAlignment(Qt::AlignCenter);
        tablaProductos->setItem(fila, 4, itemStock);
        
        tablaProductos->setItem(fila, 5, 
            new QTableWidgetItem(QString::fromStdString(prod->fechaCaducidad)));
    }
    
    tablaProductos->setSortingEnabled(true);
}
