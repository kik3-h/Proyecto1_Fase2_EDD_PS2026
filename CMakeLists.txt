cmake_minimum_required(VERSION 3.16)
project(UICatalogoP2EddKike VERSION 2.0.0 LANGUAGES CXX)

# =============================================================================
# Configuración de C++17
# =============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =============================================================================
# Configuración de Qt6
# Habilitar procesamiento automático de MOC, UIC y RCC
# =============================================================================
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# =============================================================================
# Encontrar paquetes de Qt6
# =============================================================================
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# =============================================================================
# Archivos fuente y cabeceras
# =============================================================================
set(FUENTES
    src/principal.cpp
    src/Producto.cpp
    src/ListaEnlazada.cpp
    src/ListaEnlazadaOrdenada.cpp
    src/TablaHash.cpp
    src/ArbolAVL.cpp
    src/ArbolB.cpp
    src/ArbolBPlus.cpp
    src/Pila.cpp
    src/Cola.cpp
    src/Sucursal.cpp
    src/ListaSucursales.cpp
    src/MinHeap.cpp
    src/GrafoSucursales.cpp
    src/CargadorCSV.cpp
    src/MedidorRendimiento.cpp
    src/VentanaPrincipal.cpp
)

set(CABECERAS
    include/Producto.h
    include/ListaEnlazada.h
    include/ListaEnlazadaOrdenada.h
    include/TablaHash.h
    include/ArbolAVL.h
    include/ArbolB.h
    include/ArbolBPlus.h
    include/Pila.h
    include/Cola.h
    include/Sucursal.h
    include/ListaSucursales.h
    include/MinHeap.h
    include/GrafoSucursales.h
    include/CargadorCSV.h
    include/MedidorRendimiento.h
    include/VentanaPrincipal.h
)

# =============================================================================
# Crear ejecutable
# =============================================================================
add_executable(UICatalogoP2EddKike 
    ${FUENTES}
    ${CABECERAS}
)

# =============================================================================
# Incluir directorio de cabeceras
# =============================================================================
target_include_directories(UICatalogoP2EddKike PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# =============================================================================
# Enlazar con bibliotecas de Qt6
# =============================================================================
target_link_libraries(UICatalogoP2EddKike PRIVATE
    Qt6::Core
    Qt6::Widgets
)

# =============================================================================
# Opciones de compilación (warnings)
# =============================================================================
target_compile_options(UICatalogoP2EddKike PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# =============================================================================
# Configuración para Windows (opcional)
# =============================================================================
if(WIN32)
    set_target_properties(UICatalogoP2EddKike PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# =============================================================================
# Mensaje de información
# =============================================================================
message(STATUS "==============================================")
message(STATUS "Proyecto: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Compilador: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Estándar C++: ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt6 encontrado: ${Qt6_VERSION}")
message(STATUS "==============================================")
