#ifndef DEQUE_H
#define DEQUE_H

#include <string>

class Deque {
private:
    std::string* arr;
    int capacidad;
    int inicio;
    int fin;
    int cantidad;

public:
    explicit Deque(int cap);
    ~Deque();

    void push_back(const std::string& x);
    std::string pop_front();

    bool vacia() const;
    bool llena() const;

    int obtenerTamano() const;
    int obtenerCapacidad() const;

    void debug();
};

#endif