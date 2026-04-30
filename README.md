# Catálogo de Productos de Supermercado - Fase 2

Sistema distribuido de gestión de catálogo de productos para múltiples sucursales,
desarrollado en C++17 con interfaz gráfica Qt6. Implementa desde cero estructuras
de datos avanzadas (AVL, Árbol B, Árbol B+, Tabla Hash, Grafos, Colas, Pilas) y
simula transferencias de productos entre sucursales mediante grafos ponderados.

## Dependencias

Las mismas de la Fase 1: GCC ≥15.2, CMake ≥3.16, Qt6, Graphviz.

## Compilación

```bash
make
./build/uiCatalogoP2Edd
```

## Descripción

Sistema de gestión de catálogo de productos para supermercado desarrollado en C++17 con interfaz gráfica Qt6. El proyecto implementa desde cero múltiples estructuras de datos (Lista Enlazada, Lista Ordenada, Árbol AVL, Árbol B y Árbol B+) para demostrar y comparar el rendimiento de diferentes algoritmos de búsqueda e inserción.

La aplicación permite cargar productos desde archivos CSV, realizar búsquedas optimizadas por diferentes criterios (nombre, categoría, rango de fechas), visualizar gráficamente las estructuras de árbol mediante Graphviz, y ejecutar pruebas de rendimiento comparativas.

## Características Principales

- **Carga masiva** de productos desde archivos CSV (+1000 productos)
- **Búsqueda por nombre** utilizando Árbol AVL (O(log n))
- **Búsqueda por categoría** utilizando Árbol B+ con hojas enlazadas (O(log n + k))
- **Búsqueda por rango de fechas** utilizando Árbol B (O(log n))
- **Visualización gráfica** de estructuras de árbol (archivos .dot y .png)
- **Benchmarking** comparativo entre estructuras de datos
- **Interfaz gráfica** moderna con tema personalizado (Qt6 Widgets)

## Prerrequisitos

### Sistema Operativo

- Linux (probado en distribuciones basadas en Arch/Debian)

### Compilador y Herramientas de Construcción

- GCC 15.2 o superior (con soporte C++17)
- CMake 3.16 o superior
- Make

### Bibliotecas y Frameworks

- Qt 6.10.2 (módulos Core y Widgets)
- Graphviz (para generación de imágenes de árboles)

### Instalación de Dependencias (Arch Linux)

```bash
sudo pacman -S gcc cmake make qt6-base graphviz
```

### Instalación de Dependencias (Ubuntu/Debian)

```bash
sudo apt install build-essential cmake qt6-base-dev graphviz
```

## Estructura del Proyecto

```
Proyecto1_EDD_PS2026/
├── CMakeLists.txt          # Configuración de compilación
├── README.md               # Este archivo
├── include/                # Archivos de cabecera (.h)
│   ├── Producto.h
│   ├── ListaEnlazada.h
│   ├── ListaEnlazadaOrdenada.h
│   ├── ArbolAVL.h
│   ├── ArbolB.h
│   ├── ArbolBPlus.h
│   ├── CargadorCSV.h
│   ├── MedidorRendimiento.h
│   └── VentanaPrincipal.h
├── src/                    # Implementaciones (.cpp)
│   ├── principal.cpp
│   ├── Producto.cpp
│   ├── ListaEnlazada.cpp
│   ├── ListaEnlazadaOrdenada.cpp
│   ├── ArbolAVL.cpp
│   ├── ArbolB.cpp
│   ├── ArbolBPlus.cpp
│   ├── CargadorCSV.cpp
│   ├── MedidorRendimiento.cpp
│   └── VentanaPrincipal.cpp
├── data/                   # Archivos de datos y salidas
│   ├── productos.csv       # Catálogo de muestra
│   ├── avl.png            # Visualización Árbol AVL
│   ├── arbol_b.png        # Visualización Árbol B
│   └── arbol_bplus.png    # Visualización Árbol B+
├── docs/                   # Documentación
│   ├── Report.md
│   ├── Manual_Usuario.md
│   └── diagramas_mermaid.txt
└── build/                  # Directorio de compilación
```

## Instrucciones de Compilación

### Paso 1: Clonar o descargar el proyecto

```bash
git clone https://github.com/kik3-h/Proyecto1_Fase2_EDD_PS2026.git
```

### Paso 2: Crear directorio de compilación

```bash
mkdir build
cd build
```

### Paso 3: Configurar con CMake

```bash
cmake ..
```

Salida esperada:

```
-- ==============================================
-- Proyecto: UICatalogoP2EddKike v1.0.0
-- Compilador: GNU 15.2.1
-- Estándar C++: 17
-- Qt6 encontrado: 6.10.2
-- ==============================================
-- Configuring done
-- Generating done
```

### Paso 4: Compilar

```bash
make
```

O para compilación paralela (más rápida):

```bash
make -j4
```

### Paso 5: Ejecutar la aplicación

```bash
./uiCatalogoP2EddKike
```

## Uso Rápido

1. **Cargar datos**: Clic en "Cargar CSV" y seleccionar el archivo `data/productos.csv`
2. **Buscar productos**: Usar la barra de búsqueda seleccionando el tipo de filtro
3. **Insertar producto**: Clic en "Insertar Producto" y completar el formulario
4. **Visualizar árboles**: Clic en "Generar Reportes" para ver las estructuras
5. **Benchmarking**: Clic en "Pruebas Rendimiento" para comparar tiempos

## Autor

- **Nombre**: Kike Fase 2
- **Universidad**: Universidad de San Carlos de Guatemala (USAC)
- **Curso**: Estructuras de Datos - Semestre 1 2026
