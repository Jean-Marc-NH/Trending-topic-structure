#include "hashMap.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

struct NodoHash {
    string clave;
    string valor;
    int freq;
    bool eliminado;
    bool usado;
    int indi;

    NodoHash() : clave(), valor(), freq(0), eliminado(false), usado(false), indi(-1) {}
};

TablaHash::TablaHash(int tam) : tabla(new NodoHash[tam]), cap(tam), cont(0) {}

TablaHash::~TablaHash() {
    delete[] tabla;
}

int TablaHash::hashFunc(const string& clave) const {
    unsigned long hash = 5381;
    for (unsigned char c : clave) {
        hash = hash * 33 + c;
    }
    return static_cast<int>(hash % static_cast<unsigned long>(cap));
}

void TablaHash::rehash() {
    int oldCap = cap;
    cap = cap * 2 + 1;
    NodoHash* oldtab = tabla;
    tabla = new NodoHash[cap];
    cont = 0;

    for (int i = 0; i < oldCap; ++i) {
        if (oldtab[i].usado && !oldtab[i].eliminado) {
            insertar(oldtab[i].clave, oldtab[i].valor, oldtab[i].freq, oldtab[i].indi);
        }
    }
    delete[] oldtab;
}

void TablaHash::insertar(const string& clave, const string& valor, int freq, int indi) {
    if ((cont * 100) / cap > 70) {
        rehash();
    }

    int id = hashFunc(clave);
    int start = id;

    while (tabla[id].usado && !tabla[id].eliminado) {
        if (tabla[id].clave == clave) {
            tabla[id].valor = valor;
            tabla[id].freq += freq;
            return;
        }
        id = (id + 1) % cap;
        if (id == start) break;
    }

    tabla[id].clave = clave;
    tabla[id].valor = valor;
    tabla[id].freq = freq;
    tabla[id].usado = true;
    tabla[id].eliminado = false;
    tabla[id].indi = indi;
    ++cont;
}

bool TablaHash::buscar(const string& clave, string& valor) const {
    int id = hashFunc(clave);
    int start = id;

    while (tabla[id].usado) {
        if (!tabla[id].eliminado && tabla[id].clave == clave) {
            valor = tabla[id].valor;
            return true;
        }
        id = (id + 1) % cap;
        if (id == start) break;
    }
    return false;
}

bool TablaHash::buscarFreq(const string& clave, int& freq) const {
    int id = hashFunc(clave);
    int start = id;

    while (tabla[id].usado) {
        if (!tabla[id].eliminado && tabla[id].clave == clave) {
            freq = tabla[id].freq;
            return true;
        }
        id = (id + 1) % cap;
        if (id == start) break;
    }
    return false;
}

bool TablaHash::existe(const string& clave) const {
    int id = hashFunc(clave);
    int start = id;

    while (tabla[id].usado) {
        if (!tabla[id].eliminado && tabla[id].clave == clave) {
            return true;
        }
        id = (id + 1) % cap;
        if (id == start) break;
    }
    return false;
}

string& TablaHash::operator[](const string& clave) {
    if ((cont * 100) / cap > 70) {
        rehash();
    }

    int id = hashFunc(clave);
    int start = id;

    while (tabla[id].usado && !tabla[id].eliminado) {
        if (tabla[id].clave == clave) {
            return tabla[id].valor;
        }
        id = (id + 1) % cap;
        if (id == start) break;
    }

    tabla[id].clave = clave;
    tabla[id].valor.clear();
    tabla[id].freq = 1;
    tabla[id].usado = true;
    tabla[id].eliminado = false;
    tabla[id].indi = -1;
    ++cont;

    return tabla[id].valor;
}

void TablaHash::eliminar(const string& clave) {
    int id = hashFunc(clave);
    int start = id;

    while (tabla[id].usado) {
        if (!tabla[id].eliminado && tabla[id].clave == clave) {
            tabla[id].eliminado = true;
            tabla[id].indi = -1;
            --cont;
            return;
        }
        id = (id + 1) % cap;
        if (id == start) break;
    }
}

bool TablaHash::estaVacia() const {
    return cont == 0;
}

int TablaHash::obtenerTam() const {
    return cont;
}

int TablaHash::obtenerCap() const {
    return cap;
}

void TablaHash::limpiar() {
    delete[] tabla;
    cap = 101;
    tabla = new NodoHash[cap];
    cont = 0;
}

void TablaHash::guardarEnArchivo(const string& nombreArchivo) const {
    ofstream archivo(nombreArchivo);

    for (int i = 0; i < cap; ++i) {
        if (tabla[i].usado && !tabla[i].eliminado) {
            archivo << tabla[i].clave << '|' << tabla[i].valor << '|' << tabla[i].freq << '\n';
        }
    }
}

void TablaHash::cargarDesdeArchivo(const string& nombreArchivo) {
    ifstream archivo(nombreArchivo);

    string linea;
    while (getline(archivo, linea)) {
        size_t pos1 = linea.find('|');
        size_t pos2 = linea.rfind('|');
        if (pos1 != string::npos && pos2 != string::npos && pos1 != pos2) {
            string clave = linea.substr(0, pos1);
            string valor = linea.substr(pos1 + 1, pos2 - pos1 - 1);
            int freq = stoi(linea.substr(pos2 + 1));
            insertar(clave, valor, freq);
        }
    }
}

void TablaHash::mostrar() const {
    cout << "\nTbla Hash: \n";
    for (int i = 0; i < cap; ++i) {
        if (tabla[i].usado && !tabla[i].eliminado) {
            cout << '[' << i << "] " << tabla[i].clave << " -> " << tabla[i].valor
                 << " (frecu: " << tabla[i].freq << ")\n";
        }
    }
    cout << "Elem: " << cont << " cap: " << cap;
}

void TablaHash::mostrarClaves() const {
    cout << "Claves almac (" << cont << "):\n";
    int imprimidas = 0;
    for (int i = 0; i < cap; ++i) {
        if (tabla[i].usado && !tabla[i].eliminado) {
            cout << " - " << tabla[i].clave << "\n";
            ++imprimidas;
        }
    }
    if (imprimidas == 0) cout << " (ninguno ZzZ)\n";
}

int TablaHash::getHeapIndi(const string& clave) const {
    int id = hashFunc(clave);
    int start = id;
    while (tabla[id].usado) {
        if (!tabla[id].eliminado && tabla[id].clave == clave) {
            return tabla[id].indi;
        }
        id = (id + 1) % cap;
        if (id == start) break;
    }
    return -1;
}

void TablaHash::setHeapIndi(const string& clave, int indi) {
    int id = hashFunc(clave);
    int start = id;
    while (tabla[id].usado) {
        if (!tabla[id].eliminado && tabla[id].clave == clave) {
            tabla[id].indi = indi;
            return;
        }
        id = (id + 1) % cap;
        if (id == start) break;
    }
}

void TablaHash::mostrarClavesConFreq() const {
    for (int i = 0; i < cap; ++i) {
        if (tabla[i].usado && !tabla[i].eliminado) {
            cout << " - " << tabla[i].clave << " (frec max: " << tabla[i].freq << ")" << endl;
        }
    }
}