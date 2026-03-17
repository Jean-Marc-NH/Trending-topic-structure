#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>

// Para lectura de tiempo y pausas
#include <chrono>
#include <thread>

#include "hashMap.h"
#include "min-heap.h"
#include "deque.h"
#include "preprocesador.h"

using namespace std;

int cargarRutas(const filesystem::path& ruta, string* rutas, int max) {
    int i = 0;
    for (const auto &entry : filesystem::directory_iterator(ruta)) {
        if (i >= max) break;
        if (!entry.is_regular_file()) continue;
        rutas[i] = entry.path().string();
        i++;
    }
    return i;
}

void procesar(Deque &dq, TablaHash &tabla, MinHeap &heap, TablaHash &hist, const string &topic, double &tHash, double &tHeap) {
    auto t1 = chrono::high_resolution_clock::now();
    auto t2 = chrono::high_resolution_clock::now();

    if (dq.llena()) {
        string old = dq.pop_front();
        int fOld = 0;
        
        t1 = chrono::high_resolution_clock::now();
        bool existe = tabla.buscarFreq(old, fOld);
        t2 = chrono::high_resolution_clock::now();
        tHash += chrono::duration_cast<chrono::duration<double, micro>>(t2 - t1).count();

        if (existe) {
            if (fOld <= 1) {
                t1 = chrono::high_resolution_clock::now();
                heap.elim(old, tabla);
                t2 = chrono::high_resolution_clock::now();
                tHeap += chrono::duration_cast<chrono::duration<double, micro>>(t2 - t1).count();

                t1 = chrono::high_resolution_clock::now();
                tabla.eliminar(old);
                if (old != "No reconocido") hist.insertar(old, "", 1);
                t2 = chrono::high_resolution_clock::now();
                tHash += chrono::duration_cast<chrono::duration<double, micro>>(t2 - t1).count();
            } else {
                t1 = chrono::high_resolution_clock::now();
                tabla.insertar(old, "", -1);
                t2 = chrono::high_resolution_clock::now();
                tHash += chrono::duration_cast<chrono::duration<double, micro>>(t2 - t1).count();

                t1 = chrono::high_resolution_clock::now();
                heap.act(old, fOld - 1, tabla);
                t2 = chrono::high_resolution_clock::now();
                tHeap += chrono::duration_cast<chrono::duration<double, micro>>(t2 - t1).count();
            }
        }
    }

    int fNew = 0;
    t1 = chrono::high_resolution_clock::now();
    bool exN = tabla.buscarFreq(topic, fNew);
    t2 = chrono::high_resolution_clock::now();
    tHash += chrono::duration_cast<chrono::duration<double, micro>>(t2 - t1).count();

    if (exN) {
        t1 = chrono::high_resolution_clock::now();
        tabla.insertar(topic, "", 1);
        t2 = chrono::high_resolution_clock::now();
        tHash += chrono::duration_cast<chrono::duration<double, micro>>(t2 - t1).count();
        fNew = fNew + 1;
    } else {
        t1 = chrono::high_resolution_clock::now();
        tabla.insertar(topic, "", 1);
        t2 = chrono::high_resolution_clock::now();
        tHash += chrono::duration_cast<chrono::duration<double, micro>>(t2 - t1).count();
        fNew = 1;
    }

    t1 = chrono::high_resolution_clock::now();
    heap.act(topic, fNew, tabla);
    t2 = chrono::high_resolution_clock::now();
    tHeap += chrono::duration_cast<chrono::duration<double, micro>>(t2 - t1).count();

    dq.push_back(topic);
}

void debugEstado(const TablaHash& tabla, const MinHeap& heap, const Deque& dq) {
    cout << "   [Test :c] Estado - Ventana: " << dq.obtenerTamano()
         << ", Tabla: " << tabla.obtenerTam()
         << ", Heap: " << (heap.vacia() ? "vacio" : "con elementos") << endl;
}

int main() {
    int cant;
    cout << "Ingrese el numero de noticias a procesar (ej: 10): ";

    cin >> cant;

    string *rutas = new string[cant];
    filesystem::path ruta = filesystem::current_path() / "Noticias";

    int leidas = cargarRutas(ruta, rutas, cant);
    if (leidas == 0) {
        cout << "No se encontraron noticias en: " << ruta.string() << endl;
        delete[] rutas;
        return 1;
    }

    int vN, kK;


    cout << "Ingrese el tamaño de la ventana (ej: 5)";
    cin >> vN;

    cout << "Ingrese el valor de K (ej: 3)";
    cin >> kK;  

    int N = vN;
    int K = kK;
    Deque dq(N);
    TablaHash tabla;
    MinHeap heap(K);
    TablaHash hist;
    PrepNoticias prep;

    cout << "Procesando " << leidas << " noticias (ventana=" << N << ", K=" << K << ")..." << endl;
    cout << "INICIANDO SIMULACION" << endl;

    for (int i = 0; i < leidas; ++i) {
        string path = rutas[i];
        ifstream archivo(path);
        string txt;
        if (archivo.is_open()) {
            string linea;
            while (getline(archivo, linea)) txt += linea + "\n";
            archivo.close();
        } else {
            cerr << "Error: No se pudo abrir " << path << endl;
            continue;
        }

        auto t1 = chrono::high_resolution_clock::now();
        string topic = prep.extTema(txt);
        auto t2 = chrono::high_resolution_clock::now();
        double tPre = chrono::duration_cast<chrono::duration<double, micro>>(t2 - t1).count();

        cout << "\nNoticia " << (i+1) << ": " << topic << endl;

        double tHash = 0.0;
        double tHeap = 0.0;
        procesar(dq, tabla, heap, hist, topic, tHash, tHeap);

        cout << "   Tiempo Preprocesado: " << tPre << " us" << endl;
        cout << "   Tiempo Hash Table: " << tHash << " us" << endl;
        cout << "   Tiempo Heap: " << tHeap << " us" << endl;

        cout << "   Top-" << K << " actual:" << endl;
        heap.mostrarTop();
        heap.exportToJSON("trends.json");

        this_thread::sleep_for(chrono::seconds(1));
    }

    cout << "\n=== RESUMEN FINAL ===" << endl;
    cout << "Top-" << K << " final (orden descendente):" << endl;
    heap.mostrarTop();

    cout << "\nTemas historicos :" << endl;
    if (hist.obtenerTam() == 0) {
        cout << "  (ninguno)" << endl;
    } else {
        hist.mostrarClavesConFreq();
    }

    cout << "\nEstadisticas finales:" << endl;
    cout << "- Noticias procesadas: " << leidas << endl;
    cout << "- Tamano de ventana (N): " << N << endl;
    cout << "- Top-K (K): " << K << endl;
    cout << "- Topics unicos en ventana actual: " << tabla.obtenerTam() << endl;
    cout << "- Topics historicos registrados: " << hist.obtenerTam() << endl;

    delete[] rutas;
    cout << "\n=== SIMULACION FINALIZADA ===" << endl;
    return 0;
}