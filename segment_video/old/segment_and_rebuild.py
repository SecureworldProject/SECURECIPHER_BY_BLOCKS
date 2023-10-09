import os
import random
from moviepy.video.io.VideoFileClip import VideoFileClip
from moviepy.editor import VideoClip, concatenate_videoclips
import numpy as np
import sys
import pickle

def get_video_params(ruta_video):

    # Cargar el videoclip
    videoclip = VideoFileClip(ruta_video)

    # Obtener el ancho, alto y duración
    ancho = videoclip.size[0]
    alto = videoclip.size[1]
    duracion_segundos = videoclip.duration

    # Mostrar la información
    print(f'Ancho: {ancho} píxeles')
    print(f'Alto: {alto} píxeles')
    print(f'Duración: {duracion_segundos} segundos')

    # Cerrar el videoclip (liberar recursos)
    videoclip.close()
    return ancho,alto

def segmentar_video(input_video, output_folder, min_segment_duration, max_segment_duration):
    trozos_data = {}
    # Verificar si el directorio de salida existe; si no, crearlo
    if not os.path.exists(output_folder):
        os.makedirs(output_folder)

    # Cargar el video
    video_clip = VideoFileClip(input_video)

    # Obtener la duración total del video
    total_duration = video_clip.duration

    # Segmentar el video
    start_time = 0
    i = 1
    while start_time < total_duration:
        # Determinar la duración del segmento aleatoriamente (redondear a un número entero)
        segment_duration = round(random.uniform(min_segment_duration, max_segment_duration))

        # Asegurarse de que el segmento no exceda la duración total
        segment_duration = min(segment_duration, total_duration - start_time)

        # Extraer el segmento
        segment_clip = video_clip.subclip(start_time, start_time + segment_duration)

        # Guardar el segmento
        segment_filename = f"{output_folder}/trozo_{i}.mp4"
        segment_clip.write_videofile(segment_filename, codec="libx264")

        # Actualizar el tiempo de inicio para el próximo segmento
        start_time += segment_duration
        i += 1
        trozos_data[segment_filename] = segment_duration

    # Cerrar el video clip
    video_clip.close()
    with open('trozos_data.pkl', 'wb') as f:
        pickle.dump(trozos_data, f)

def leer_binario(ruta_binario, ancho, alto):
    with open(ruta_binario, 'rb') as f:
        contenido_binario = f.read()

    # Imprimir el tamaño de contenido_binario para depuración
    print("Tamaño de contenido_binario:", len(contenido_binario))

     # Imprimir la forma de los datos antes de la operación reshape
    print("Forma antes del reshape:", np.frombuffer(contenido_binario, dtype=np.uint8).shape)
    # Reshape de los datos binarios a una matriz tridimensional (alto, ancho, canales)
    video_data = np.frombuffer(contenido_binario, dtype=np.uint8).reshape((-1, alto, ancho, 3))
    return video_data

def make_frame(t, video_data_list, duracion_por_video, ancho, alto):
    print(f"Datos de entrada en make_frame:\n-t: {t}\nvideo_data_list: {video_data_list}\nduracion: {duracion_por_video}\nancho,alto: {ancho},{alto}")
    video_idx = int(t // duracion_por_video)
    frame_idx = int((t % duracion_por_video) * video_data_list[video_idx].shape[0] / duracion_por_video)
    frame = video_data_list[video_idx][frame_idx, :, :, :]
    return frame

def rebuild_video():
    # Cargar trozos_data desde el archivo trozos_data.pkl
    with open('trozos_data.pkl', 'rb') as f:
        trozos_data = pickle.load(f)
    # Configuración de los videos
    ancho, alto = get_video_params("coche.mp4")
    
    archivos_binarios = trozos_data.keys()
    archivos_descifrados = [nombre.replace(".mp4", "_desc") for nombre in archivos_binarios]
    duraciones_archivos = trozos_data.values()
    print(archivos_descifrados)
    print(duraciones_archivos)
    # Leer datos binarios de cada archivo
    video_data_list = [leer_binario(archivo, ancho, alto) for archivo in archivos_descifrados]
    # Crear videoclips individuales
    videoclips = [VideoClip(lambda t, i=i: make_frame(t, video_data_list[i], duracion_archivo, ancho, alto),
                            duration=duracion_archivo)
                  for i, duracion_archivo in enumerate(duraciones_archivos)]


    # Concatenar videoclips
    videoclip_final = concatenate_videoclips(videoclips)

    # Escribir el videoclip final en formato MP4
    videoclip_final.write_videofile('salida_final.mp4', fps=24, codec='libx264')



if __name__ == "__main__":
    opcion = input("1. Segmentar video\n2. Reconstruir video descifrado\n3. Salir\n")
    if opcion == '1':
        # Especificar el video de entrada, la carpeta de salida y los rangos de duración
        input_video = "coche.mp4"
        output_folder = "trozos"
        min_segment_duration = 2  # duración mínima en segundos
        max_segment_duration = 5  # duración máxima en segundos

        # Llamar a la función para segmentar el video
        segmentar_video(input_video, output_folder, min_segment_duration, max_segment_duration)
    elif opcion == '2':
        rebuild_video()
    elif opcion=='3':
        sys.exit()
    else:
        print("Opción no válida")

