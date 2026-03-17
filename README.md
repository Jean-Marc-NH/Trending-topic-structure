# Analizador de Tópicos en Noticias (Streaming)

Este proyecto implementa un sistema en C++ para analizar flujos de noticias en tiempo real y detectar los "Top-K" tópicos más frecuentes dentro de una ventana deslizante.

## Descripción

El programa procesa archivos de texto que contienen noticias, extrae el tema principal (topic) de cada una y mantiene un ranking actualizado de los temas más mencionados. Utiliza estructuras de datos propias optimizadas para el rendimiento.

## Características Principales

- **Ventana Deslizante (Sliding Window):** Mantiene solo las noticias más recientes (tamaño N) para el análisis.
- **Top-K Tópicos:** Muestra los K temas más frecuentes en la ventana actual.
- **Estructuras de Datos Propias:**
  - `TablaHash`: Para el conteo de frecuencias de los tópicos.
  - `MinHeap`: Para mantener eficientemente los K tópicos más frecuentes.
  - `Deque`: Para gestionar la ventana deslizante de noticias.
- **Preprocesamiento de Texto:** Limpieza, tokenización y eliminación de stopwords para extraer temas relevantes.
- **Métricas de Rendimiento:** Mide y muestra el tiempo de ejecución de las etapas de preprocesamiento, hash table y heap.

## Requisitos

- Compilador de C++ compatible con C++17 (ej. GCC, Clang, MSVC).

## Compilación

Para compilar el proyecto, ejecute el siguiente comando en la terminal:

```bash
g++ -std=c++17 -o m main.cpp preprocesador.cpp hashMap.cpp deque.cpp min-heap.cpp
```

Esto generará un ejecutable llamado `m` (o `m.exe` en Windows).

## Uso

1. Asegúrese de tener una carpeta llamada `Noticias` en el mismo directorio que el ejecutable, conteniendo los archivos de texto a procesar.
2. Ejecute el programa:

```bash
./m
```

3. Siga las instrucciones en pantalla para ingresar:
   - Número de noticias a procesar.
   - Tamaño de la ventana deslizante (N).
   - Cantidad de tópicos a mostrar en el ranking (K).

## Ejemplo de Ejecución

```text
Ingrese el numero de noticias a procesar (ej: 10): 5
Ingrese el tamaño de la ventana (ej: 5): 3
Ingrese el valor de K (ej: 3): 2

Procesando 5 noticias (ventana=3, K=2)...
INICIANDO SIMULACION

Noticia 1: greg maddux
   Tiempo Preprocesado: 343.3 us
   Tiempo Hash Table: 1.3 us
   Tiempo Heap: 0.7 us
   Top-2 actual:
1 - greg maddux : 1
...
```

## Estructura del Proyecto

- `main.cpp`: Punto de entrada y lógica principal de la simulación.
- `preprocesador.h/cpp`: Lógica para limpieza de texto y extracción de tópicos (`PrepNoticias`).
- `hashMap.h/cpp`: Implementación de la Tabla Hash (`TablaHash`).
- `min-heap.h/cpp`: Implementación del Min-Heap (`MinHeap`).
- `deque.h/cpp`: Implementación de la Cola de Doble Terminación (`Deque`).
- `visualizacion.py`: Script de Python para visualizar los tópicos en tiempo real.

## Ejecución con Visualización

Para ejecutar el sistema con la visualización de tópicos en tiempo real:

1.  **Compilar el código C++:**
    ```bash
    g++ main.cpp min-heap.cpp deque.cpp hashMap.cpp preprocesador.cpp -o main.exe
    ```

2.  **Instalar dependencias de Python:**
    ```bash
    pip install matplotlib wordcloud
    ```

3.  **Ejecutar el script de visualización:**
    Abra una terminal y ejecute:
    ```bash
    python visualizacion.py
    ```
    *Mantenga esta ventana abierta.*

4.  **Ejecutar el programa principal:**
    En otra terminal, ejecute:
    ```bash
    ./main.exe
    ```
