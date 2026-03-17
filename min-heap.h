#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <string>

using namespace std;

class TablaHash;

class MinHeap {
private:
    std::string* arr;
    int* freqs;
    int cap;
    int cant;

    int left(int i) const;
    int right(int i) const;
    int parent(int i) const;

    void cambiar(int a, int b, TablaHash& tabla);
    void subir(int idx, TablaHash& tabla);
    void bajar(int idx, TablaHash& tabla);

public:
    explicit MinHeap(int k);
    ~MinHeap();

    MinHeap(const MinHeap&) = delete;
    MinHeap& operator=(const MinHeap&) = delete;

    std::string minC();
    int minF();

    bool llena() const;
    bool vacia() const;

    void debug();
    void mostrarTop();
    bool elim(const std::string& key, TablaHash& tabla);
    string act(const std::string& key, int freq, TablaHash& tabla);
    void exportToJSON(const std::string& filename);
};

#endif