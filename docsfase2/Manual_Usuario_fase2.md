# Manual de Usuario - Fase 2

## Sistema de Gestión de Inventario Multi-Sucursal con Red de Transporte

**Aplicación:** UIsCatalogoP2EddKike  
**Versión:** Fase 2 - Gestión de Sucursales y Traslados  
**Institución:** USAC - Proyecto de Estructuras de Datos PS2026  
**Fecha:** 2026

---

## 1. Introducción

El Sistema de Gestión de Inventario Multi-Sucursal (Fase 2) amplía las capacidades de la Fase 1 con la administración de **múltiples sucursales interconectadas** mediante una red de transporte. Esta nueva versión permite:

- **Gestión descentralizada:** Cada sucursal mantiene su propio inventario con índices independientes.
- **Red de transporte:** Sistema de rutas ponderadas (por tiempo y costo) que conecta sucursales.
- **Simulación de traslados:** Productos viajan entre sucursales siguiendo colas temporales (ingreso, preparación, salida).
- **Algoritmo Dijkstra:** Cálculo automático de rutas óptimas para transporte minimizando tiempo o costo.
- **Visualización interactiva:** Grafo de sucursales con zoom y pan, árboles de búsqueda por sucursal.
- **Aceleración configurable:** Simulación 1000x más rápida para observar traslados en tiempo real.

La interfaz mantiene la experiencia de usuario intuitiva de la Fase 1 con nuevas pestañas, tablas y controles para la red y simulación.

---

## 2. Requisitos del Sistema

### Hardware
- **Procesador:** Intel Core i5 (o equivalente) a 1.0 GHz mínimo.
- **Memoria RAM:** 
  - Mínimo: 2 GB para ≤ 10 sucursales, ≤ 5,000 productos.
  - Recomendado: 4-8 GB para ≤ 50 sucursales, ≤ 100,000 productos.
- **Almacenamiento:** 500 MB libres para archivos CSV, datos y Graphviz.
- **Pantalla:** 1920×1080 recomendado; mínimo 1366×768.

### Software
- **Sistema operativo:** Linux (Ubuntu 20.04+, Fedora 34+) o Windows 10+.
- **Compilador:** GCC 9.0+ con soporte C++17.
- **Qt:** 6.0 o superior (modelos Core y Widgets).
- **CMake:** 3.16 o superior.
- **Graphviz:** `dot` 2.40+ (para generación de imágenes de árboles).

### Instalación de Dependencias

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev libgraphviz-dev graphviz gcc g++
```

**Fedora:**
```bash
sudo dnf install gcc g++ cmake qt6-base-devel graphviz-devel graphviz
```

**macOS (Homebrew):**
```bash
brew install cmake qt@6 graphviz gcc
```

---

## 3. Instalación y Compilación

### Paso 1: Clonar el Repositorio
```bash
git clone https://github.com/kik3-h/Proyecto1_Fase2_EDD_PS2026.git
cd Proyecto1_Fase2_EDD_PS2026
```

### Paso 2: Compilar
```bash
# Opción 1: Usar Makefile (recomendado)
make

# Opción 2: CMake directo
cmake -S . -B build
cmake --build build
```

### Paso 3: Ejecutar
```bash
# Opción 1: Desde Makefile
make run

# Opción 2: Directo
./build/UICatalogoP2EddKike
```

### Estructura de Directorios Post-Compilación
```
build/
├── UICatalogoP2EddKike          # Ejecutable principal
└── [archivos objeto compilados]

data/
├── sucursales.csv               # Plantilla de sucursales
├── conexiones.csv               # Plantilla de red
├── productos.csv                # Plantilla de inventario
├── traslados.csv                # Plantilla de traslados (opcional)
├── avl.png / arbol_b.png / ...  # Imágenes de árboles (generadas)
└── grafo.png                    # Imagen del grafo de sucursales

errors.log                        # Registro de errores de carga
```

---

## 4. Interfaz Principal

### 4.1 Estructura General

La ventana principal se divide en tres zonas:

**1. Sidebar izquierdo (ancho ~200 px):**
- Secciones desplegables para carga de archivos, sucursales e inventario.
- Botones de acción con color azul FC Barcelona (#004D98).
- Scrollbar automático si es necesario.

**2. Panel central superior (pestaña "Grafo"):**
- Visualización de la red de sucursales con zoom/pan.
- Nodos circulares (sucursales) con etiquetas.
- Aristas ponderadas (tiempo/costo entre sucursales).
- Usar ratón: rueda para zoom, arrastrar para pan.

**3. Panel central inferior (pestaña "Productos"):**
- Tabla con productos de la sucursal actual.
- Columnas: Código, Nombre, Categoría, Precio, Fecha Caducidad, Estado.
- Búsqueda avanzada integrada en barra superior.

### 4.2 Sidebar - Secciones y Botones

#### Sección "Archivos"
- **Cargar Sucursales CSV:** Abre diálogo para importar sucursales_estructura.csv.
- **Cargar Conexiones CSV:** Importa red de rutas entre sucursales.
- **Cargar Productos CSV:** Carga inventario en sucursal seleccionada.
- **Cargar Traslados CSV:** Programa traslados masivos (opcional).

#### Sección "Sucursales"
- **Agregar Sucursal:** Abre formulario con campos ID, Nombre, Ubicación, Tiempos (ingreso, preparación, despacho).
- **Modificar Sucursal:** Edita parámetros de sucursal activa.
- **Eliminar Sucursal:** Remueve sucursal (con confirmación).
- **Ir a Sucursal:** Abre ventana detallada de sucursal seleccionada.

#### Sección "Inventario"
- **Insertar Producto:** Formulario para crear producto.
- **Eliminar Producto:** Búsqueda y eliminación por código.
- **Buscar Producto:** Búsqueda avanzada (ver sección 4.4).
- **Generar Reportes:** Exporta informes en Graphviz y PNG.
- **Benchmarking:** Mide rendimiento de operaciones.

### 4.3 Selector de Sucursal

**Ubicación:** Lado derecho del toolbar superior.

**Componentes:**
- Dropdown (QComboBox) listando todas las sucursales cargadas.
- Botón "Ir a Sucursal" junto al dropdown.
- Indica sucursal actualmente seleccionada.

**Uso:**
1. Selecciona sucursal del dropdown.
2. Presiona "Ir a Sucursal" para abrir ventana de sucursal.
3. Los productos mostrados en la tabla corresponden a la sucursal seleccionada.

### 4.4 Barra de Búsqueda Avanzada

**Ubicación:** Arriba de la tabla de productos.

**Componentes:**
- Selector de tipo de búsqueda: "Todas", "Por Nombre", "Por Código", "Por Categoría", "Por Rango de Fecha".
- Campo de entrada de texto/valor.
- Botón "Buscar".

**Operaciones y Complejidad:**
| Tipo de Búsqueda | Estructura | Complejidad |
|---|---|---|
| Por Código | TablaHash | O(1) promedio |
| Por Nombre | ArbolAVL | O(log n) |
| Por Categoría | ArbolBPlus | O(log n + k) |
| Por Rango de Fecha | ArbolB | O(log n + k) |

### 4.5 Tabla de Productos

**Columnas:**
| Campo | Tipo | Actualización |
|-------|------|----------------|
| Código | String | Solo lectura |
| Nombre | String | Desde búsqueda |
| Categoría | String | Desde búsqueda |
| Precio | Decimal | Solo lectura |
| Fecha Caducidad | YYYY-MM-DD | Solo lectura |
| Estado | String | En tiempo real (traslados) |

**Botones contextuales:**
- Botón derecho para menú: copiar código, ver detalles, transferir, eliminar.

### 4.6 Visualización del Grafo

**Ubicación:** Pestaña "Grafo" (panel central).

**Características:**
- **Nodos:** Círculos coloreados (azul FC Barcelona) con ID y nombre de sucursal.
- **Aristas:** Líneas con etiquetas "Xh / $Y" (tiempo en horas, costo en $).
- **Layout:** Circular; nodos distribuidos uniformemente.
- **Interacción:** 
  - Rueda del ratón: zoom in/out (1.1x a 2.0x).
  - Click+arrastrar: pan (movimiento del grafo).
  - Zoom automático a vista completa al cargar.

**Ejemplo visual en texto:**
```
        Sucursal 1 (Central)
              |
        ------+------
       /      |      \
    2h/$50  3h/$75  4h/$100
     /        |        \
   S2        S3        S4
   |         / \       |
 1h/$30  1h/$30  2h/$55 1h/$40
   |     /       \     |
   +---------S5-------+
```

---

## 5. Carga de Archivos CSV

### 5.1 Formato de sucursales.csv

**Estructura:** 7 columnas, cabecera opcional.

```
ID,Nombre,Ubicacion,TiempoIngreso,TiempoPreparacion,IntervaloDespacho
1,Central,Zona 1,5,10,3
2,Zona 2,Zona 2,6,12,4
3,Zona 3,Zona 3,7,14,5
4,Zona 4,Zona 4,5,11,3
5,Exterior,Zona Rural,8,15,6
```

**Validaciones:**
- ID debe ser único y > 0.
- Nombre y ubicación: sin comillas ni caracteres especiales.
- Tiempos en ticks; debe cumplir: Ingreso ≤ 100, Preparación ≤ 100, Despacho ≤ 20.
- Errores se registran en `errors.log`.

### 5.2 Formato de conexiones.csv

**Estructura:** 5 columnas, cabecera opcional.

```
Origen,Destino,Tiempo,Costo,Bidireccional
1,2,2,50,1
1,3,3,75,1
1,4,4,100,1
2,3,1,30,1
2,5,2,60,1
3,4,2,55,1
3,5,3,70,1
4,5,1,40,1
```

**Validaciones:**
- Origen y Destino deben ser IDs de sucursales ya cargadas.
- Tiempo y Costo > 0.
- Bidireccional: 1 (sí) o 0 (no); si es 1, se crea arista inversa automáticamente.
- Duplicados en archivo se saltan sin error.

### 5.3 Formato de productos.csv

**Estructura:** 7 columnas, cabecera opcional (detecta automáticamente).

```
Codigo,Nombre,Categoria,Precio,FechaCaducidad,Estado,ComentarioReferencia
P001,Laptop Dell,Electrónica,1200.50,2026-12-31,Disponible,Referencia-01
P002,Mouse Inalámbrico,Periféricos,25.99,2027-06-15,Disponible,Referencia-02
P003,Teclado Mecánico,Periféricos,89.99,2026-03-20,Caducado,Referencia-03
```

**Validaciones:**
- Código debe ser único dentro de la sucursal (verificado por TablaHash).
- Nombre y Categoría: sin comillas múltiples.
- Precio debe ser numérico positivo.
- Fecha en formato YYYY-MM-DD.
- Estado: "Disponible", "Tránsito", "Preparación", "Caducado".
- Errores se registran y el producto se salta.

### 5.4 Formato de traslados.csv

**Estructura:** 3 columnas, cabecera opcional.

```
Origen,Destino,ProductoID
1,2,P001
1,3,P002
2,4,P003
3,5,P001
```

**Validaciones:**
- Origen y Destino son IDs de sucursal.
- ProductoID es código de barras del producto.
- Búsqueda en sucursal origen; si no existe, se registra error y se salta.
- Cada traslado se programa en el simulador con Dijkstra.
- Duplicados en archivo se detectan y registran.

---

## 6. Ventana de Sucursal

### 6.1 Cómo Abrir la Ventana

**Opción 1:** Selector de sucursal
1. Selecciona sucursal del dropdown en toolbar superior.
2. Presiona botón "Ir a Sucursal".

**Opción 2:** Desde sidebar
1. Presiona "Ir a Sucursal" en sección "Sucursales".
2. Se abre con sucursal actualmente seleccionada.

**Ventana se abre en:** Nueva ventana modal con tres pestañas.

### 6.2 Pestaña "Colas"

Visualiza productos en las tres colas de procesamiento (FIFO).

**Componentes:**

#### Cola de Ingreso
- **Tabla:** Código, Nombre, Tiempos en Cola.
- **Descripción:** Productos recién llegados, esperando ser procesados.
- **Duración típica:** TiempoIngreso ticks.

#### Cola de Preparación
- **Tabla:** Código, Nombre, Estado de Preparación.
- **Descripción:** Productos siendo preparados para despacho.
- **Duración típica:** TiempoPreparacion ticks.

#### Cola de Salida
- **Tabla:** Código, Nombre, Destino, Tiempo de Tránsito.
- **Descripción:** Productos listos para partir hacia siguiente sucursal.
- **Duración típica:** IntervaloDespacho + DuracionViaje ticks.

**Botón "Refrescar Colas":**
- Actualiza manualmente las tablas.
- Se llama automáticamente al abrir ventana.
- Iteración sin desencolar (O(k) donde k = elementos en cola).

**Nota:** Las colas se vacían automáticamente conforme el simulador procesa eventos en tiempo acelerado (1000x).

### 6.3 Pestaña "Árboles"

Visualización interactiva de los cuatro índices de búsqueda de la sucursal.

**Componentes:**

#### Árbol AVL (Búsqueda por Nombre)
- Imagen generada con Graphviz desde data/avl.png.
- Nodos en estructura binaria con balance factor.
- Etiquetas: primeras 3 letras del nombre.

#### Árbol B (Búsqueda por Rango de Fecha Caducidad)
- Imagen data/arbol_b.png.
- Nodos multikey (típicamente 3-10 claves por nodo).
- Etiquetas: rangos de fecha (MM-DD).

#### Árbol BPlus (Búsqueda por Categoría)
- Imagen data/arbol_bplus.png.
- Nodos internos separadores, hojas con datos.
- Etiquetas: nombres de categoría.

#### Tabla Hash (Búsqueda por Código)
- Representación estadística: número de buckets, factor carga, colisiones.
- Tabla de resumen: Capacidad, Ocupación, Factor Carga %.

**Interacción con imágenes (QGraphicsView):**
- **Zoom:** Rueda del ratón (in/out).
- **Pan:** Click+arrastrar (desplazamiento).
- **Fit:** Botón "Ajustar a Ventana" para vista completa.

**Actualización:** Botón "Regenerar Árboles" recalcula todos los índices y redibuja imágenes.

### 6.4 Pestaña "Detalles"

Información general de la sucursal y opciones de transferencia de productos.

**Sección Información:**
| Campo | Valor |
|-------|-------|
| ID | Identificador único |
| Nombre | Nombre sucursal |
| Ubicación | Dirección/zona |
| Tiempo de Ingreso | X ticks |
| Tiempo de Preparación | Y ticks |
| Intervalo de Despacho | Z ticks |
| Total de Productos | N (conteo en tiempo real) |
| Tránsitos Activos | M (productos en ruta) |

**Sección Transferencia:**
- Botón "Transferir Producto a Otra Sucursal".
- Abre diálogo que solicita:
  1. Código de barras del producto.
  2. ID de sucursal destino.
- Sistema busca producto (O(1) hash), calcula ruta (O((V+E) log V) Dijkstra), inicia simulación.
- Confirmación visual: "Traslado programado de [Código] a Sucursal [ID]".

---

## 7. Transferencia de Productos Paso a Paso

### Escenario: Transferir laptop de Sucursal 1 a Sucursal 5

**Paso 1: Abrir ventana de Sucursal 1**
- En dropdown principal, selecciona "Sucursal 1 - Central".
- Presiona "Ir a Sucursal".

**Paso 2: Dirigirse a pestaña "Detalles"**
- Click en pestaña "Detalles" en ventana de sucursal.

**Paso 3: Iniciar transferencia**
- Presiona botón "Transferir Producto a Otra Sucursal".
- Se abre diálogo: "Ingrese código de barras del producto".
- Digita `P001` (código de laptop).

**Paso 4: Seleccionar destino**
- Sistema confirma: "Producto P001 (Laptop Dell) encontrado en Sucursal 1".
- Segundo diálogo: "Ingrese ID de sucursal destino".
- Digita `5` (Sucursal Exterior).

**Paso 5: Confirmación y simulación**
- Sistema calcula ruta óptima: 1 → 2 → 5 (costo total: 90).
- Mensaje: "Traslado programado: P001 desde Sucursal 1 a Sucursal 5. Ruta: 1→2→5 (costo $90, tiempo 4h)".
- Se inicia simulación en tiempo acelerado (1000x).

**Paso 6: Monitoreo del traslado**
- En consola se ven mensajes:
  ```
  [Simulador] Evento creado: P001 de 1 a 5
  [Simulador] Etapa ingreso: P001 en Sucursal 1
  [Simulador] Etapa preparación: P001 en Sucursal 1
  [Simulador] Etapa salida: P001 sale de Sucursal 1 hacia Sucursal 2
  [Simulador] Tránsito: P001 en trayecto Sucursal 1→2
  [Simulador] Etapa ingreso: P001 en Sucursal 2
  ...
  [Simulador] Traslado completado: P001 llegó a Sucursal 5 (destino final)
  ```

**Paso 7: Verificación en Sucursal 5**
- Abre ventana de Sucursal 5.
- En pestaña "Colas" se verá P001 en cola de ingreso.
- En pestaña "Productos" (tabla general) aparecerá una vez procesada (estado "Disponible").

**Tiempo real esperado:** ~100-500 ms (simulación 1000x acelerada).

---

## 8. Pruebas de Rendimiento (Benchmarking)

### 8.1 Acceso al Módulo

1. Sidebar → Inventario → "Benchmarking".
2. Se abre ventana "Medidor de Rendimiento" con inputs para número de operaciones.

### 8.2 Parámetros Configurables

**Número de inserciones:** 100-100,000 (por defecto 1,000).
**Número de búsquedas:** 100-100,000 (por defecto 1,000).
**Número de eliminaciones:** 100-100,000 (por defecto 1,000).

### 8.3 Operaciones Medidas

Por cada estructura de datos:

| Estructura | Operación | Métrica |
|-----------|-----------|---------|
| ListaEnlazada | Insertar al inicio | Tiempo total / operación |
| ListaEnlazadaOrdenada | Insertar ordenado | Tiempo total / operación |
| ArbolAVL | Insertar + buscar | Tiempo total / operación |
| ArbolB | Insertar rango | Tiempo total / operación |
| ArbolBPlus | Insertar categoría | Tiempo total / operación |
| TablaHash | Insertar por código | Tiempo total / operación |

### 8.4 Resultados Esperados

Ejemplo con 10,000 operaciones en máquina estándar:

```
=============== RESULTADOS DE BENCHMARKING ===============
Número de inserciones: 10,000

ListaEnlazada (inserción frontal):
  Tiempo total: 23 ms
  Tiempo promedio por inserción: 2.3 μs
  Throughput: 434,782 ops/s

TablaHash (inserción por código):
  Tiempo total: 45 ms
  Tiempo promedio por inserción: 4.5 μs
  Throughput: 222,222 ops/s

ArbolAVL (inserción + rebalance):
  Tiempo total: 250 ms
  Tiempo promedio por inserción: 25 μs
  Throughput: 40,000 ops/s

Búsquedas (10,000 búsquedas):
  Por código (Hash): 8 ms (0.8 μs promedio)
  Por nombre (AVL): 180 ms (18 μs promedio)
  Por rango (ArbolB): 150 ms (15 μs promedio)
```

### 8.5 Interpretación de Resultados

- **Operaciones O(1) (Hash):** ~0.5-2 μs; ideal para búsquedas frecuentes.
- **Operaciones O(log n) (AVL/B):** ~10-50 μs; eficientes para estructuras ordenadas.
- **Operaciones O(n) (ListaEnlazada):** ~1-100 μs; rápido para n pequeño, lento para n grande.

**Nota:** Tiempos varían según carga del sistema, tamaño de caché y overhead de medición.

---

## 9. Solución de Problemas Comunes

### Problema 1: "Archivo CSV no encontrado"

**Causa:** Ruta incorrecta o archivo no existe.

**Solución:**
1. Verifica que el archivo esté en directorio `data/` o ruta absoluta especificada.
2. Usa botón "Examinar" (dialog) para seleccionar archivo gráficamente.
3. Revisa `errors.log` para más detalles.

### Problema 2: "Producto duplicado" al cargar CSV

**Causa:** Código de barras ya existe en la sucursal.

**Solución:**
1. Elimina producto existente o cambia código en CSV.
2. Intenta cargar nuevamente.
3. Revisa `errors.log` para lista de duplicados rechazados.

### Problema 3: "Sucursal destino no encontrada" al transferir

**Causa:** ID de sucursal no existe en la red.

**Solución:**
1. Verifica que sucursal destino fue cargada (visible en dropdown).
2. Asegúrate de que conexiones.csv incluya ruta a esa sucursal.
3. Intenta de nuevo con ID válido.

### Problema 4: Traslado "se queda estancado"

**Causa:** Factor de aceleración es 1 (tiempo real muy lento).

**Solución:**
1. Verifica que factor aceleración es 1000 (por defecto).
2. Monitorea consola para mensajes de progreso.
3. Espera 30-60 segundos; traslad debería completarse.

### Problema 5: Tabla de "Productos" vacía aunque se cargaron

**Causa:** Productos se cargaron en sucursal diferente a la seleccionada.

**Solución:**
1. En dropdown selector, verifica sucursal activa.
2. Carga productos nuevamente en sucursal correcta.
3. Tabla se actualizará automáticamente.

### Problema 6: Imágenes de árboles no aparecen

**Causa:** Graphviz (`dot`) no instalado o imágenes no regeneradas.

**Solución:**
1. Instala Graphviz: `sudo apt install graphviz` (Linux) o `brew install graphviz` (macOS).
2. En ventana de sucursal, pestaña "Árboles", presiona "Regenerar Árboles".
3. Espera mientras las imágenes se generan (típicamente 1-5 segundos).

### Problema 7: Aplicación se congela al cargar muchos productos

**Causa:** Redimensionamiento de tablas o actualización de UI es lento.

**Solución:**
1. Carga productos en lotes (máximo 10,000 por vez).
2. Verifica RAM disponible (necesita ≥ 2 GB).
3. Cierra otras aplicaciones para liberar recursos.

### Problema 8: Dijkstra "no encuentra ruta"

**Causa:** No hay conexión entre sucursales origen y destino.

**Solución:**
1. Verifica que conexiones.csv incluya arista entre sucursales.
2. Si está desconectada la red, agrega conexiones manualmente desde UI.
3. Regenera grafo (botón "Refrescar Grafo" en pestaña principal).

### Problema 9: Error de compilación "Qt6 no encontrado"

**Causa:** Qt6 no instalado o CMake no puede localizarlo.

**Solución (Linux):**
```bash
sudo apt install qt6-base-dev
```

**Solución (macOS):**
```bash
brew install qt@6
export CMAKE_PREFIX_PATH=/usr/local/opt/qt@6:$CMAKE_PREFIX_PATH
cmake -S . -B build
```

### Problema 10: "Factor de carga elevado" en Hash

**Causa:** Muchas colisiones; tabla no se redimensionó.

**Solución:**
1. Sistema redimensiona automáticamente a 2x cuando factor > 0.75.
2. Si persiste, verifica función hash (debe distribuir uniformemente).
3. Regenera índices: elimina y recarga productos.

---

## 10. Tips y Mejores Prácticas

### Para Usuarios
1. **Carga incremental:** Carga 5,000-10,000 productos por vez para evitar lag.
2. **Backup de datos:** Exporta CSV antes de eliminaciones masivas.
3. **Monitoreo de red:** Visualiza grafo regularmente para detectar desconexiones.
4. **Aceleración:** Por defecto en 1000x; reduce a 100x si quieres ver transiciones más claras.

### Para Administradores
1. **Mantenimiento de índices:** Regenera árboles semanalmente si hay muchas inserciones/eliminaciones.
2. **Histórico de errores:** Revisa `errors.log` regularmente para diagnóstico.
3. **Benchmarking periódico:** Ejecuta benchmarks trimestrales para detectar degradación.

---

## 11. Contacto y Soporte

**Proyecto:** Estructuras de Datos - Fase 2 - USAC PS2026  
**Repositorio:** https://github.com/kik3-h/Proyecto1_Fase2_EDD_PS2026  
**Documentación técnica:** Consulta `docsfase2/diagramas_fase2.md` y `docsfase2/analisis_complejidad_fase2.md`

---

**Manual versión:** 2.0  
**Última actualización:** 2026-05-04  
**Licencia:** Proyecto académico - USAC

