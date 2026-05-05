# Diagramas Fase 2 - Proyecto Estructuras de Datos

Documento con diagramas UML de clases, TADs y arquitectura del sistema de gestión de sucursales y traslados de productos.

---

## 1. Diagrama de Clases General

Relaciones de composición, agregación y herencia del sistema completo.

```mermaid
classDiagram
    class VentanaPrincipal {
        -listaSucursales: ListaSucursales*
        -grafo: GrafoSucursales*
        -simulador: Simulador*
        -medidor: MedidorRendimiento*
        -visorGrafo: VisorGrafo*
        -sucursalActual: Sucursal*
        -tablaProductos: QTableWidget*
        -timerSimulacion: QTimer*
        -factorAceleracion: int
        +agregarSucursal()
        +seleccionarSucursal()
        +onCargarSucursalesCSV()
        +onCargarProductosCSV()
        +onCargarTrasladosCSV()
        +onTickSimulacion()
    }
    
    class VentanaSucursal {
        -sucursal: Sucursal*
        -simulador: Simulador*
        -tablaColaIngreso: QTableWidget*
        -tablaColaPreparacion: QTableWidget*
        -tablaColaSalida: QTableWidget*
        -vistaArbolAVL: QGraphicsView*
        -vistaArbolB: QGraphicsView*
        -vistaArbolBPlus: QGraphicsView*
        +refrescarColas()
        +cargarImagenEnVista()
        +onTransferirProducto()
    }
    
    class VisorGrafo {
        -grafo: GrafoSucursales*
        -scene: QGraphicsScene*
        -escalaZoom: double
        +dibujarGrafo()
        +eventFilter()
    }
    
    class ListaSucursales {
        -cabeza: NodoLista*
        -cantidad: int
        +insertarSucursal()
        +buscarSucursalPorId()
        +iterarSucursales()
    }
    
    class Sucursal {
        -id: int
        -nombre: string
        -ubicacion: string
        -tiempoIngreso: int
        -tiempoPreparacion: int
        -intervaloDespacho: int
        -listaGeneral: ListaEnlazada*
        -listaOrdenada: ListaEnlazadaOrdenada*
        -arbolAVL: ArbolAVL*
        -arbolB: ArbolB*
        -arbolBPlus: ArbolBPlus*
        -tablaHash: TablaHash*
        -colaIngreso: Cola*
        -colaPreparacion: Cola*
        -colaSalida: Cola*
        -pilaRollback: Pila*
        +agregarProducto()
        +eliminarProducto()
        +buscarPorCodigo()
        +buscarPorNombre()
        +buscarPorCategoria()
        +buscarPorRangoFecha()
        +encolarIngreso()
        +encolarPreparacion()
        +encolarSalida()
    }
    
    class GrafoSucursales {
        -adyacencia: ListaAdyacencia**
        -maxNodos: int
        -numNodos: int
        +agregarSucursal()
        +agregarConexion()
        +rutaMasCorta()
        +obtenerAristas()
    }
    
    class Simulador {
        -grafo: GrafoSucursales*
        -sucursales: ListaSucursales*
        -eventosPendientes: Cola*
        -tickActual: long long
        -factorAceleracion: int
        +programarEnvio()
        +avanzar()
        +establecerFactorAceleracion()
        -procesarEvento()
        -procesarEtapaIngreso()
        -procesarEtapaPreparacion()
        -procesarEtapaSalidaYTransito()
    }
    
    class CargadorCSV {
        +cargarSucursales()
        +cargarConexiones()
        +cargarProductos()
    }
    
    class MedidorRendimiento {
        -tiempos: map
        +iniciarMedicion()
        +finalizarMedicion()
        +reportarResultados()
    }
    
    class Producto {
        -codigo: string
        -nombre: string
        -categoria: string
        -precio: double
        -fechaCaducidad: string
        -estado: string
    }
    
    class Cola {
        -frente: NodoCola*
        -final: NodoCola*
        +encolar()
        +desencolar()
        +verFrente()
        +obtenerFrente()
        +vacia()
    }
    
    class Pila {
        -cima: NodoPila*
        +push()
        +pop()
        +top()
        +vacia()
    }
    
    class MinHeap {
        -arreglo: NodoHeap*
        -capacidad: int
        -tamano: int
        +insertar()
        +extraerMinimo()
        +vacia()
    }
    
    class ArbolAVL {
        -raiz: NodoAVL*
        +insertar()
        +eliminar()
        +buscar()
        +generarDOT()
    }
    
    class ArbolB {
        -raiz: NodoB*
        -orden: int
        +insertar()
        +eliminar()
        +buscar()
        +generarDOT()
    }
    
    class ArbolBPlus {
        -raiz: NodoBPlus*
        -orden: int
        +insertar()
        +eliminar()
        +buscar()
        +generarDOT()
    }
    
    class TablaHash {
        -tabla: NodoHash**
        -capacidad: int
        -cantidad: int
        +insertar()
        +eliminar()
        +buscar()
    }
    
    class ListaEnlazada {
        -cabeza: NodoProducto*
        +insertar()
        +eliminar()
        +iterar()
    }
    
    class ListaEnlazadaOrdenada {
        -cabeza: NodoProducto*
        +insertar()
        +eliminar()
        +buscar()
    }
    
    %% Relaciones de composición
    VentanaPrincipal --> ListaSucursales : usa
    VentanaPrincipal --> GrafoSucursales : usa
    VentanaPrincipal --> Simulador : usa
    VentanaPrincipal --> MedidorRendimiento : usa
    VentanaPrincipal --> VisorGrafo : usa
    VentanaPrincipal --> CargadorCSV : usa
    
    VentanaSucursal --> Sucursal : muestra
    VentanaSucursal --> Simulador : usa
    
    VisorGrafo --> GrafoSucursales : dibuja
    
    ListaSucursales --> Sucursal : contiene
    
    Sucursal --> ListaEnlazada : contiene
    Sucursal --> ListaEnlazadaOrdenada : contiene
    Sucursal --> ArbolAVL : contiene
    Sucursal --> ArbolB : contiene
    Sucursal --> ArbolBPlus : contiene
    Sucursal --> TablaHash : contiene
    Sucursal --> Cola : contiene
    Sucursal --> Pila : contiene
    
    Simulador --> GrafoSucursales : usa
    Simulador --> ListaSucursales : usa
    Simulador --> Cola : contiene
    Simulador --> Producto : procesa
    
    GrafoSucursales --> MinHeap : usa
    
    Cola --> Producto : almacena
    Pila --> Producto : almacena
    
    ArbolAVL --> Producto : indexa
    ArbolB --> Producto : indexa
    ArbolBPlus --> Producto : indexa
    TablaHash --> Producto : indexa
    ListaEnlazada --> Producto : almacena
    ListaEnlazadaOrdenada --> Producto : almacena
```

---

## 2. Diagramas de TAD (Tipos Abstractos de Datos)

### 2.1 TAD Cola (FIFO)

```mermaid
graph TB
    subgraph COLATAD["TDA Cola (FIFO)"]
        direction LR
        
        subgraph def["Definición"]
            D1["Cola = conjunto de elementos Producto"]
            D2["Operación: FIFO (primero en entrar, primero en salir)"]
            D3["No se permite acceso a elementos intermedios"]
        end
        
        subgraph impl["Implementación"]
            I1["Nodos enlazados: NodoCola*"]
            I2["frente: puntero al primer nodo"]
            I3["final: puntero al último nodo"]
        end
        
        subgraph ops["Operaciones O(1)"]
            O1["encolar(p): inserta al final"]
            O2["desencolar(): elimina del frente"]
            O3["verFrente(): acceso solo lectura"]
            O4["obtenerFrente(): retorna NodoCola* para iteración"]
            O5["vacia(): consulta estado"]
        end
        
        def --> impl
        impl --> ops
    end
```

### 2.2 TAD Pila (LIFO)

```mermaid
graph TB
    subgraph PILATAD["TDA Pila (LIFO)"]
        direction LR
        
        subgraph def["Definición"]
            D1["Pila = conjunto de elementos Producto"]
            D2["Operación: LIFO (último en entrar, primero en salir)"]
            D3["Acceso solo por la cima"]
        end
        
        subgraph impl["Implementación"]
            I1["Nodos enlazados: NodoPila*"]
            I2["cima: puntero al nodo superior"]
        end
        
        subgraph ops["Operaciones O(1)"]
            O1["push(p): apila en la cima"]
            O2["pop(): desapila de la cima"]
            O3["top(): acceso solo lectura a la cima"]
            O4["vacia(): consulta estado"]
        end
        
        def --> impl
        impl --> ops
    end
```

### 2.3 TAD GrafoSucursales (Dijkstra)

```mermaid
graph TB
    subgraph GRAFOTAD["TDA GrafoSucursales"]
        direction TB
        
        subgraph def["Definición"]
            D1["Grafo = V nodos sucursales + E aristas ponderadas"]
            D2["Ponderado: tiempo y costo"]
            D3["Fuerte: ida y vuelta bidireccional"]
        end
        
        subgraph impl["Implementación"]
            I1["Lista de adyacencia: NodoArista* por sucursal"]
            I2["Arreglo dinámico redimensionable"]
            I3["Cada arista: destinoId, tiempo, costo"]
        end
        
        subgraph ops["Operaciones"]
            O1["agregarSucursal(id): O(1)"]
            O2["agregarConexion(origen,destino,tiempo,costo): O(1)"]
            O3["rutaMasCorta(origen,destino,porTiempo): O(V+E)logV Dijkstra"]
            O4["obtenerAristas(id): O(1) para iteración"]
        end
        
        def --> impl
        impl --> ops
    end
```

### 2.4 TAD MinHeap (Cola de Prioridad)

```mermaid
graph TB
    subgraph HEAPTAD["TDA MinHeap"]
        direction LR
        
        subgraph def["Definición"]
            D1["MinHeap = árbol binario completo"]
            D2["Propiedad: padre ≤ hijo (mínima raíz)"]
            D3["Uso: Dijkstra, prioritarios"]
        end
        
        subgraph impl["Implementación"]
            I1["Arreglo dinámico: NodoHeap[]"]
            I2["Capacidad: redimensionable 2x"]
            I3["Nodo: idSucursal, distancia"]
        end
        
        subgraph ops["Operaciones"]
            O1["insertar(id,distancia): O(log n) flotar"]
            O2["extraerMinimo(): O(log n) hundir"]
            O3["vacia(): O(1)"]
        end
        
        def --> impl
        impl --> ops
    end
```

### 2.5 TAD Simulador (Máquina de Estados)

```mermaid
graph TB
    subgraph SIMTAD["TDA Simulador"]
        direction TB
        
        subgraph def["Definición"]
            D1["Simulador = motor de eventos traslado temporal"]
            D2["Estados: creado (0) → ingreso (1) → preparación (2) → salida/tránsito (3)"]
            D3["Acelerable: factor 1-10000 para speedup"]
        end
        
        subgraph impl["Implementación"]
            I1["eventosPendientes: Cola*"]
            I2["EventoTraslado: producto, ruta, etapa, ticks"]
            I3["tickActual: avanza por factorAceleracion"]
        end
        
        subgraph ops["Operaciones"]
            O1["programarEnvio(p,origen,destino,porTiempo): O(1)"]
            O2["avanzar(): O(eventos) procesa etapas"]
            O3["procesarEtapa*(): O(1) transiciones de estado"]
            O4["establecerFactorAceleracion(factor): O(1)"]
        end
        
        def --> impl
        impl --> ops
    end
```

### 2.6 TAD ArbolAVL (Búsqueda Balanceada)

```mermaid
graph TB
    subgraph AVLTAD["TDA ArbolAVL"]
        direction LR
        
        subgraph def["Definición"]
            D1["ArbolAVL = árbol BST autobalanceado"]
            D2["Propiedad: |altura(izq) - altura(der)| ≤ 1"]
            D3["Búsqueda por nombre: O(log n)"]
        end
        
        subgraph impl["Implementación"]
            I1["Nodos: NodoAVL* con altura"]
            I2["Factor balance: calculado en inserción/eliminación"]
            I3["Rotaciones: simple y doble"]
        end
        
        subgraph ops["Operaciones"]
            O1["insertar(p): O(log n) con rebalance"]
            O2["eliminar(p): O(log n) con rebalance"]
            O3["buscar(nombre): O(log n) BST"]
            O4["generarDOT(): O(n) visualización Graphviz"]
        end
        
        def --> impl
        impl --> ops
    end
```

### 2.7 TAD ArbolB (Búsqueda Rango por Fecha)

```mermaid
graph TB
    subgraph BTAD["TDA ArbolB"]
        direction LR
        
        subgraph def["Definición"]
            D1["ArbolB = árbol de búsqueda multikey"]
            D2["Propiedad: 1 a orden-1 claves por nodo"]
            D3["Búsqueda por rango de fechas: O(log n + k)"]
        end
        
        subgraph impl["Implementación"]
            I1["Nodos: NodoB* con múltiples claves"]
            I2["Inserción/eliminación: split/merge"]
            I3["Mantiene balance por count mínimo"]
        end
        
        subgraph ops["Operaciones"]
            O1["insertar(p,fecha): O(log n)"]
            O2["eliminar(p,fecha): O(log n)"]
            O3["buscarRango(inicio,fin): O(log n+k)"]
            O4["generarDOT(): O(n) visualización"]
        end
        
        def --> impl
        impl --> ops
    end
```

### 2.8 TAD ArbolBPlus (Búsqueda por Categoría)

```mermaid
graph TB
    subgraph BPLUSTAD["TDA ArbolBPlus"]
        direction LR
        
        subgraph def["Definición"]
            D1["ArbolBPlus = B+ con hojas enlazadas"]
            D2["Índices en nodos internos, datos en hojas"]
            D3["Búsqueda por categoría: O(log n + k)"]
        end
        
        subgraph impl["Implementación"]
            I1["Nodos internos: NodoBPlus* sin datos"]
            I2["Hojas: contienen Producto* y siguiente"]
            I3["Recorrido lineal en hojas"]
        end
        
        subgraph ops["Operaciones"]
            O1["insertar(p,categoria): O(log n)"]
            O2["eliminar(p,categoria): O(log n)"]
            O3["buscarCategoria(nombre): O(log n+k)"]
            O4["generarDOT(): O(n) visualización"]
        end
        
        def --> impl
        impl --> ops
    end
```

### 2.9 TAD TablaHash (Búsqueda Directa por Código)

```mermaid
graph TB
    subgraph HASHTAD["TDA TablaHash"]
        direction LR
        
        subgraph def["Definición"]
            D1["TablaHash = tabla de dispersión con encadenamiento"]
            D2["Clave: código de barras (único)"]
            D3["Búsqueda por código: O(1) promedio"]
        end
        
        subgraph impl["Implementación"]
            I1["Arreglo dinámico: NodoHash**"]
            I2["Colisiones: encadenamiento frontal"]
            I3["Redimensionamiento: carga > 0.75"]
        end
        
        subgraph ops["Operaciones"]
            O1["insertar(p): O(1) amortizado"]
            O2["eliminar(codigo): O(1) promedio"]
            O3["buscar(codigo): O(1) promedio"]
        end
        
        def --> impl
        impl --> ops
    end
```

### 2.10 TAD ListaEnlazada (Inventario General)

```mermaid
graph TB
    subgraph LISTAGEN["TDA ListaEnlazada"]
        direction LR
        
        subgraph def["Definición"]
            D1["ListaEnlazada = lista simple de productos"]
            D2["Orden: según inserción"]
            D3["Acceso: iteración lineal O(n)"]
        end
        
        subgraph impl["Implementación"]
            I1["Nodos: NodoProducto* enlazados"]
            I2["cabeza: puntero al inicio"]
        end
        
        subgraph ops["Operaciones"]
            O1["insertar(p): O(1) al inicio o O(n) ordenado"]
            O2["eliminar(codigo): O(n) búsqueda"]
            O3["iterar(): O(n) toda la lista"]
        end
        
        def --> impl
        impl --> ops
    end
```

### 2.11 TAD ListaEnlazadaOrdenada (Búsqueda por Nombre)

```mermaid
graph TB
    subgraph LISTAORD["TDA ListaEnlazadaOrdenada"]
        direction LR
        
        subgraph def["Definición"]
            D1["ListaEnlazadaOrdenada = lista ordenada por nombre"]
            D2["Orden: alfabético ascendente"]
            D3["Búsqueda binaria en iteración: O(n) lineal"]
        end
        
        subgraph impl["Implementación"]
            I1["Nodos: NodoProducto* enlazados"]
            I2["cabeza: mantiene ordenamiento"]
        end
        
        subgraph ops["Operaciones"]
            O1["insertar(p): O(n) búsqueda + inserción"]
            O2["eliminar(codigo): O(n)"]
            O3["buscarPorNombre(nombre): O(n) lineal"]
        end
        
        def --> impl
        impl --> ops
    end
```

---

## 3. Red de Sucursales (Ejemplo Visual)

Grafo de 5 sucursales conectadas con pesos de tiempo y costo.

```mermaid
graph LR
    S1["🏪 Sucursal 1<br/>Central"]
    S2["🏪 Sucursal 2<br/>Zona 1"]
    S3["🏪 Sucursal 3<br/>Zona 2"]
    S4["🏪 Sucursal 4<br/>Zona 3"]
    S5["🏪 Sucursal 5<br/>Exterior"]
    
    S1 -->|2h / $50| S2
    S1 -->|3h / $75| S3
    S1 -->|4h / $100| S4
    S2 -->|1h / $30| S3
    S2 -->|2h / $60| S5
    S3 -->|2h / $55| S4
    S3 -->|3h / $70| S5
    S4 -->|1h / $40| S5
    S5 -->|2h / $60| S2
    S4 -->|3h / $80| S1
```

**Matriz de Conexiones:**
| Origen | Destino | Tiempo (h) | Costo ($) |
|--------|---------|-----------|-----------|
| 1 | 2 | 2 | 50 |
| 1 | 3 | 3 | 75 |
| 1 | 4 | 4 | 100 |
| 2 | 1 | 2 | 50 |
| 2 | 3 | 1 | 30 |
| 2 | 5 | 2 | 60 |
| 3 | 1 | 3 | 75 |
| 3 | 2 | 1 | 30 |
| 3 | 4 | 2 | 55 |
| 3 | 5 | 3 | 70 |
| 4 | 1 | 4 | 100 |
| 4 | 3 | 2 | 55 |
| 4 | 5 | 1 | 40 |
| 5 | 2 | 2 | 60 |
| 5 | 3 | 3 | 70 |
| 5 | 4 | 1 | 40 |

---

## 4. Máquina de Estados del Simulador

Transiciones de un producto en traslado entre sucursales.

```mermaid
stateDiagram-v2
    [*] --> Creado: programarEnvio()
    
    Creado --> Ingreso: Calculada ruta\ncon Dijkstra
    
    Ingreso --> Preparacion: Tiempo ingreso\ntranscurrido
    Ingreso --> Salida: Si destino=actual\n(ingreso sin preparación)
    
    Preparacion --> Salida: Tiempo preparación\ntranscurrido
    
    Salida --> Transito: Despacho iniciado
    
    Transito --> DestinoFinal: Duración viaje\ncompletada
    Transito --> Transito: Siguiente sucursal\nen ruta
    
    DestinoFinal --> Disponible: Inserción en\ninventario destino
    
    Disponible --> [*]
    
    note right of Ingreso
        O(1): Encola en colaIngreso
        Espera tiempoIngreso ticks
    end note
    
    note right of Preparacion
        O(1): Encola en colaPreparacion
        Espera tiempoPreparacion ticks
    end note
    
    note right of Salida
        O(1): Encola en colaSalida
        Espera intervaloDespacho ticks
    end note
    
    note right of Transito
        O(grado): Avanza por ruta calculada
        Duración viaje / factorAceleracion
    end note
```

---

## 5. Flujo de Datos en Traslado

Cómo fluye un producto desde programación hasta llegada.

```mermaid
graph TD
    A["programarEnvio(producto, origen, destino)"]
    B["Crear EventoTraslado"]
    C["Dijkstra: calcular ruta óptima"]
    D["Encolar en eventosPendientes"]
    E["Tick simulación: avanzar()"]
    
    E --> F["Procesar EventoTraslado"]
    F --> G{"Etapa?"}
    
    G -->|Creado| H["procesarEtapaCreacion()"]
    H --> I["Encolar en colaIngreso"]
    
    G -->|Ingreso| J["procesarEtapaIngreso()"]
    J --> K{"Tiempo ingreso<br/>transcurrido?"}
    K -->|Sí| L{"Destino<br/>final?"}
    L -->|Sí| M["Insertar en inventario"]
    M --> N["Estado: Disponible"]
    L -->|No| O["Pasar a Preparación"]
    
    G -->|Preparación| P["procesarEtapaPreparacion()"]
    P --> Q["Encolar en colaPreparacion"]
    
    G -->|Salida/Tránsito| R["procesarEtapaSalidaYTransito()"]
    R --> S["Encolar en colaSalida"]
    S --> T["Avanzar en ruta"]
    
    A --> B --> C --> D --> E
    N --> U["[FIN]"]
    
    style A fill:#e1f5e1
    style U fill:#ffe1e1
    style M fill:#e1e5ff
```

---

## 6. Resumen de Complejidades

### Operaciones por Estructura

| Estructura | Insertar | Eliminar | Buscar | Generación DOT |
|-----------|----------|----------|--------|-----------------|
| ListaEnlazada | O(1) inicio, O(n) ordenada | O(n) | O(n) | O(n) |
| ListaEnlazadaOrdenada | O(n) | O(n) | O(n) | O(n) |
| ArbolAVL | O(log n) | O(log n) | O(log n) | O(n) |
| ArbolB | O(log n) | O(log n) | O(log n) | O(n) |
| ArbolBPlus | O(log n) | O(log n) | O(log n) + O(k) | O(n) |
| TablaHash | O(1) amort. | O(1) prom. | O(1) prom. | O(n) |
| Cola | O(1) encolar | O(1) desencolar | O(1) frente | - |
| Pila | O(1) push | O(1) pop | O(1) top | - |
| MinHeap | O(log n) | O(log n) | - | - |
| GrafoSucursales | O(1) | O(1) eliminar edge | O(V+E)logV Dijkstra | O(V+E) |

### Operaciones del Simulador

| Operación | Complejidad | Descripción |
|-----------|-------------|-------------|
| programarEnvio() | O((V+E) log V) | Cálculo de ruta Dijkstra |
| procesarEtapaIngreso() | O(1) | Transición de estado |
| procesarEtapaPreparacion() | O(1) | Transición de estado |
| procesarEtapaSalidaYTransito() | O(grado) | Avance en ruta |
| avanzar() | O(eventos) | Procesa todos los eventos |
| establecerFactorAceleracion() | O(1) | Setter simple |

---

## 7. Notas Técnicas

- **Aceleración**: Factor por defecto 1000x; tickActual avanza 1000 ticks por llamada a `avanzar()`.
- **Dijkstra**: Se aplica en `procesarEtapaCreacion()` para calcular ruta óptima (tiempo o costo).
- **Redimensionamiento**: TablaHash y MinHeap se redimensionan cuando alcanzan límites de ocupación.
- **Graphviz**: Los TADs de búsqueda (AVL, B, B+) generan `.dot` que se convierten a `.png` con `dot`.
- **Sincronización**: Todas las estructuras comparten `Producto*` por referencia; operaciones de inserción mantienen consistencia transaccional.

---

**Fecha de generación**: Fase 2 - Gestión de Sucursales y Traslados de Productos
**Sistema**: Proyecto Estructuras de Datos PS2026 - USAC
