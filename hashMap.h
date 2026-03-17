#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>

struct NodoHash; 

class TablaHash {
private:
    NodoHash* tabla;
    int cap;
    int cont;

    int hashFunc(const std::string& clave) const;
    void rehash();

public:
    explicit TablaHash(int tam = 101);
    ~TablaHash();

    void insertar(const std::string& clave, const std::string& valor, int freq = 1, int indi = -1);
    bool buscar(const std::string& clave, std::string& valor) const;
    bool buscarFreq(const std::string& clave, int& freq) const;
    bool existe(const std::string& clave) const;
    std::string& operator[](const std::string& clave);
    void eliminar(const std::string& clave);
    bool estaVacia() const;
    int obtenerTam() const;
    int obtenerCap() const;
    void limpiar();
    void guardarEnArchivo(const std::string& nombreArchivo) const;
    void cargarDesdeArchivo(const std::string& nombreArchivo);
    void mostrar() const;
    int getHeapIndi(const std::string& clave) const;
    void setHeapIndi(const std::string& clave, int idx);
    void mostrarClaves() const;
    void mostrarClavesConFreq() const;
};

#endif
