#include "min-heap.h"
#include "hashMap.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int MinHeap::left(int i) const { return 2 * i + 1; }
int MinHeap::right(int i) const { return 2 * i + 2; }
int MinHeap::parent(int i) const { return (i - 1) / 2; }

void MinHeap::cambiar(int a, int b, TablaHash& tabla) {
    if (a == b) return;
    string tmpClave = arr[a];
    int tmpFrec = freqs[a];
    arr[a] = arr[b];
    freqs[a] = freqs[b];
    arr[b] = tmpClave;
    freqs[b] = tmpFrec;
    tabla.setHeapIndi(arr[a], a);
    tabla.setHeapIndi(arr[b], b);
}

void MinHeap::subir(int idx, TablaHash& tabla) {
    while (idx > 0) {
        int p = parent(idx);
        if (freqs[p] > freqs[idx]) {
            cambiar(p, idx, tabla);
            idx = p;
        } else break;
    }
}

void MinHeap::bajar(int idx, TablaHash& tabla) {
    for (;;) {
        int izq = left(idx);
        int der = right(idx);
        int menor = idx;
        if (izq < cant && freqs[izq] < freqs[menor]) menor = izq;
        if (der < cant && freqs[der] < freqs[menor]) menor = der;
        if (menor != idx) {
            cambiar(menor, idx, tabla);
            idx = menor;
        } else break;
    }
}

MinHeap::MinHeap(int k) {
    if (k <= 0) k = 1;
    cap = k;
    cant = 0;
    arr = new string[cap];
    freqs = new int[cap];
}

MinHeap::~MinHeap() {
    delete[] arr;
    delete[] freqs;
}

string MinHeap::minC() {
    if (vacia()) return string();
    return arr[0];
}

int MinHeap::minF() {
    if (vacia()) return -1;
    return freqs[0];
}

bool MinHeap::llena() const { return cant == cap; }
bool MinHeap::vacia() const { return cant == 0; }

void MinHeap::debug() {
    cout << "heap debug\n";
    cout << "elementos: " << cant << " capacidad: " << cap << "\n";
}

void MinHeap::mostrarTop() {
    if (cant == 0) {
        cout << "heap vacio\n";
        return;
    }
    int n = cant;
    int K = cap < n ? cap : n;
    string* claves = new string[n];
    int* frec = new int[n];
    for (int i = 0; i < n; ++i) {
        claves[i] = arr[i];
        frec[i] = freqs[i];
    }
    for (int i = 0; i < K; ++i) {
        int maxj = i;
        for (int j = i + 1; j < n; ++j) {
            if (frec[j] > frec[maxj]) maxj = j;
        }
        if (maxj != i) {
            string tmpClave = claves[i];
            claves[i] = claves[maxj];
            claves[maxj] = tmpClave;
            int tmpF = frec[i];
            frec[i] = frec[maxj];
            frec[maxj] = tmpF;
        }
    }
    cout << "Top-" << K << ":\n";
    for (int i = 0; i < K; ++i) {
        cout << (i + 1) << " - " << claves[i] << " : " << frec[i] << "\n";
    }
    delete[] claves;
    delete[] frec;
}

bool MinHeap::elim(const string& key, TablaHash& tabla) {
    int idx = tabla.getHeapIndi(key);
    if (idx == -1) return false;
    int ultimo = cant - 1;
    tabla.setHeapIndi(key, -1);
    if (idx == ultimo) {
        cant--;
        return true;
    }
    arr[idx] = arr[ultimo];
    freqs[idx] = freqs[ultimo];
    tabla.setHeapIndi(arr[idx], idx);
    cant--;
    if (idx > 0 && freqs[idx] < freqs[(idx - 1) / 2]) subir(idx, tabla);
    else bajar(idx, tabla);
    return true;
}

string MinHeap::act(const string& key, int freq, TablaHash& tabla) {
    string claveEliminada;
    int idx = tabla.getHeapIndi(key);
    if (idx != -1) {
        int frecAnt = freqs[idx];
        freqs[idx] = freq;
        if (freq < frecAnt) subir(idx, tabla);
        else if (freq > frecAnt) bajar(idx, tabla);
        return claveEliminada;
    }
    if (!llena()) {
        arr[cant] = key;
        freqs[cant] = freq;
        tabla.setHeapIndi(key, cant);
        cant++;
        subir(cant - 1, tabla);
        return claveEliminada;
    }
    if (freq > freqs[0]) {
        claveEliminada = arr[0];
        tabla.setHeapIndi(arr[0], -1);
        arr[0] = key;
        freqs[0] = freq;
        tabla.setHeapIndi(key, 0);
        bajar(0, tabla);
    }
    return claveEliminada;
}

void MinHeap::exportToJSON(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    file << "[\n";
    bool first = true;
    for (int i = 0; i < cant; ++i) {
        if (!first) {
            file << ",\n";
        }
        file << "    {\"text\": \"" << arr[i] << "\", \"size\": " << freqs[i] << "}";
        first = false;
    }
    file << "\n]";
    file.close();
}