import os
import random
from moviepy.video.io.VideoFileClip import VideoFileClip

def segmentar_video(input_video, output_folder, min_segment_duration, max_segment_duration):
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
        # Determinar la duración del segmento aleatoriamente
        segment_duration = random.uniform(min_segment_duration, max_segment_duration)

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

    # Cerrar el video clip
    video_clip.close()

# Especificar el video de entrada, la carpeta de salida y los rangos de duración
input_video = "coche.mp4"
output_folder = "segmentos"
min_segment_duration = 2  # duración mínima en segundos
max_segment_duration = 5  # duración máxima en segundos

# Llamar a la función para segmentar el video
segmentar_video(input_video, output_folder, min_segment_duration, max_segment_duration)
