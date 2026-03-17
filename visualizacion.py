import json
import matplotlib.pyplot as plt
from wordcloud import WordCloud
import time
import os

def visualizar_tendencias():
    plt.ion()
    fig, eje = plt.subplots(figsize=(10, 6))
    
    print("--- Iniciando Visualizador de Tendencias ---")
    print("Esperando a que el sistema genere el archivo 'trends.json'...")

    archivo_json = "trends.json"

    while True:
        try:
            if os.path.exists(archivo_json):
                with open(archivo_json, "r", encoding='utf-8') as archivo:
                    try:
                        datos = json.load(archivo)
                    except json.JSONDecodeError:
                        time.sleep(0.5)
                        continue

                if not datos:
                    time.sleep(1)
                    continue

                frecuencias = {item['text']: item['size'] for item in datos}

                if frecuencias:
                    nube_palabras = WordCloud(
                        width=800, 
                        height=400, 
                        background_color='white',
                        colormap='viridis',
                        min_font_size=10
                    ).generate_from_frequencies(frecuencias)
                    
                    eje.clear()
                    eje.imshow(nube_palabras, interpolation='bilinear')
                    eje.axis("off")
                    eje.set_title("Tendencias en Tiempo Real (Top-K)", fontsize=20, pad=20)
                    
                    plt.draw()
                    plt.pause(0.1)
            
            time.sleep(1)

        except KeyboardInterrupt:
            print("\nVisualización detenida por el usuario.")
            break
        except Exception as e:
            print(f"Ocurrió un error inesperado: {e}")
            time.sleep(1)

if __name__ == "__main__":
    visualizar_tendencias()
