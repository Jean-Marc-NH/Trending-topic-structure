#include "deque.h"
#include <iostream>

using namespace std;

Deque::Deque(int cap)
    : arr(nullptr),
      capacidad(cap > 0 ? cap : 1),
      inicio(0),
      fin(0),
      cantidad(0)
{
    arr = new string[capacidad];
}

Deque::~Deque() {
    delete[] arr;
}

bool Deque::vacia() const {
    return cantidad == 0;
}

bool Deque::llena() const {
    return cantidad == capacidad;
}

int Deque::obtenerTamano() const {
    return cantidad;
}

int Deque::obtenerCapacidad() const {
    return capacidad;
}

void Deque::push_back(const string& x) {
    arr[fin] = x;
    fin = (fin + 1) % capacidad;
    if (llena()) {
        inicio = (inicio + 1) % capacidad;
    } else {
        cantidad++;
    }
}

string Deque::pop_front() {
    if (vacia()) {
        return "";
    }
    string ret = arr[inicio];
    inicio = (inicio + 1) % capacidad;
    cantidad--;
    return ret;
}

void Deque::debug() {
    cout << "Deque xd: " << endl;
    cout << "cap=" << capacidad << endl;
    cout << "ini=" << inicio << endl;
    cout << "fin=" << fin << endl;
    cout << "cant=" << cantidad << endl;
    cout << "arreglo completo:" << endl;
    for (int i = 0; i < capacidad; ++i) {
        cout << "[" << i << "]:\"" << arr[i] << "\"" << endl;
    }
    cout << "contenido en ventana:" << endl;
    for (int i = 0, idx = inicio; i < cantidad; ++i, idx = (idx + 1) % capacidad) {
        cout << "- " << arr[idx] << endl;
    }
    cout << "____-___________" << endl;
}