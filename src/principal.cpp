// principal.cpp - Punto de entrada de la aplicación Qt
// Catálogo de Productos - Proyecto EDD 2026

#include <QApplication>
#include <QFont>
#include "VentanaPrincipal.h"

int main(int argc, char* argv[]) {
    // Crear aplicación Qt
    QApplication app(argc, argv);
    
    // Configurar información de la aplicación
    QApplication::setApplicationName("Catálogo de Productos EDD");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("USAC - EDD 2026");

    // Fijar una fuente legible para toda la interfaz antes de crear la ventana principal.
    QFont fuente = QApplication::font();
    fuente.setFamily("Segoe UI");
    QApplication::setFont(fuente);

    // Crear ventana principal (gestiona sucursales e inventarios internamente).
    VentanaPrincipal ventana;
    ventana.show();

    // Ejecutar loop de eventos de Qt
    return app.exec();
}

// CÓDIGO DEL MENÚ DE CONSOLA (COMENTADO PARA REFERENCIA)
// Las estructuras de datos se instancian ahora dentro de VentanaPrincipal
//comente esto pq alfinal si me pase a qt con interfaz grafica
/*
#include <iostream>
#include <limits>
#include <string>

int mainConsola() {
    ListaEnlazada listaProductosNormal;
    ListaEnlazadaOrdenada listaProductosOrdenada;
    TablaHash tablaProductos;
    ArbolAVL arbolProductos;
    ArbolB arbolProductosFecha;
    ArbolBPlus arbolProductosCategoria;
    CargadorCSV cargadorCSV;
    Producto* productoPrueba = nullptr;
    bool continuar = true;

    while (continuar) {
        std::cout << "===== Catalogo EDP1 =====\n";
        std::cout << "1. Cargar CSV\n";
        std::cout << "2. Insertar producto de prueba\n";
        std::cout << "3. Mostrar todos los productos (Lista Normal)\n";
        std::cout << "4. Buscar producto por Nombre (Lista Ordenada)\n";
        std::cout << "5. Buscar producto por Codigo (Tabla Hash)\n";
        std::cout << "6. Listar productos alfabeticamente (AVL In-Orden)\n";
        std::cout << "7. Generar imagen del Arbol AVL (Graphviz)\n";
        std::cout << "8. Buscar productos por rango de caducidad (Arbol B)\n";
        std::cout << "9. Generar imagen del Arbol B (Graphviz)\n";
        std::cout << "10. Buscar productos por Categoria (Arbol B+)\n";
        std::cout << "11. Generar imagen del Arbol B+ (Graphviz)\n";
        std::cout << "12. Ejecutar pruebas de rendimiento (Benchmarking)\n";
        std::cout << "13. Salir\n";
        std::cout << "Seleccione una opcion: ";

        int opcion = 0;
        std::cin >> opcion;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Entrada invalida. Intente de nuevo.\n\n";
            continue;
        }

        switch (opcion) {
            case 1:
                {
                    std::string rutaArchivo;
                    std::cout << "Ingrese la ruta del archivo CSV: ";
                    std::cin >> rutaArchivo;
                    cargadorCSV.cargar(rutaArchivo,
                                       listaProductosNormal,
                                       listaProductosOrdenada,
                                       tablaProductos,
                                       arbolProductos,
                                       arbolProductosFecha,
                                       arbolProductosCategoria);
                    std::cout << '\n';
                }
                break;
            case 2:
                if (productoPrueba == nullptr) {
                    productoPrueba = new Producto("Leche Entera 1L",
                                                  "7501000010019",
                                                  "Lacteos",
                                                  "2026-12-31",
                                                  "MarcaEjemplo",
                                                  12.50,
                                                  40);

                    listaProductosNormal.insertar(productoPrueba);
                    listaProductosOrdenada.insertarOrdenado(productoPrueba);

                    if (!tablaProductos.insertar(productoPrueba)) {
                        listaProductosNormal.eliminarPorCodigoBarras(productoPrueba->codigoBarras);
                        listaProductosOrdenada.eliminarPorCodigoBarras(productoPrueba->codigoBarras);
                        delete productoPrueba;
                        productoPrueba = nullptr;
                        std::cout << "No se pudo insertar en tabla hash (codigo duplicado).\n\n";
                    } else {
                        arbolProductos.insertar(productoPrueba);
                        arbolProductosFecha.insertar(productoPrueba);
                        arbolProductosCategoria.insertar(productoPrueba);
                        std::cout << "Producto de prueba insertado correctamente en todas las estructuras.\n\n";
                    }
                } else {
                    std::cout << "El producto de prueba ya fue insertado.\n\n";
                }
                break;
            case 3:
                listaProductosNormal.mostrarTodos();
                break;
            case 4:
                {
                    std::string nombreBuscado;
                    std::cout << "Ingrese nombre exacto a buscar: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::getline(std::cin, nombreBuscado);

                    Producto* encontrado = listaProductosOrdenada.buscarPorNombre(nombreBuscado);
                    if (encontrado == nullptr) {
                        std::cout << "Producto no encontrado por nombre.\n\n";
                    } else {
                        std::cout << "Producto encontrado (Lista Ordenada):\n";
                        std::cout << *encontrado << "\n\n";
                    }
                }
                break;
            case 5:
                {
                    std::string codigoBuscado;
                    std::cout << "Ingrese codigo de barras a buscar: ";
                    std::cin >> codigoBuscado;

                    Producto* encontrado = tablaProductos.buscarPorCodigoBarras(codigoBuscado);
                    if (encontrado == nullptr) {
                        std::cout << "Producto no encontrado por codigo.\n\n";
                    } else {
                        std::cout << "Producto encontrado (Tabla Hash):\n";
                        std::cout << *encontrado << "\n\n";
                    }
                }
                break;
            case 6:
                arbolProductos.recorridoInOrden();
                break;
            case 7:
                arbolProductos.generarImagen();
                std::cout << '\n';
                break;
            case 8:
                {
                    std::string fechaInicio;
                    std::string fechaFin;
                    std::cout << "Ingrese fecha de inicio (YYYY-MM-DD): ";
                    std::cin >> fechaInicio;
                    std::cout << "Ingrese fecha de fin (YYYY-MM-DD): ";
                    std::cin >> fechaFin;
                    arbolProductosFecha.buscarPorRango(fechaInicio, fechaFin);
                    std::cout << '\n';
                }
                break;
            case 9:
                arbolProductosFecha.generarImagen();
                std::cout << '\n';
                break;
            case 10:
                {
                    std::string categoriaBuscada;
                    std::cout << "Ingrese categoria a buscar: ";
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::getline(std::cin, categoriaBuscada);
                    arbolProductosCategoria.buscarPorCategoria(categoriaBuscada);
                    std::cout << '\n';
                }
                break;
            case 11:
                arbolProductosCategoria.generarImagen();
                std::cout << '\n';
                break;
            case 12:
                {
                    MedidorRendimiento medidor;
                    medidor.ejecutarPruebasBusqueda(listaProductosNormal,
                                                    listaProductosOrdenada,
                                                    arbolProductos,
                                                    tablaProductos);
                }
                break;
            case 13:
                continuar = false;
                break;
            default:
                std::cout << "Opcion no valida.\n\n";
                break;
        }
    }

    delete productoPrueba;
    productoPrueba = nullptr;

    std::cout << "Saliendo del sistema...\n";
    return 0;
}
*/
