
Sí, puedes adaptar el código para procesar varios archivos binarios y unirlos en un solo archivo MP4. Aquí hay un ejemplo de cómo podrías hacerlo en Python con la biblioteca MoviePy:

python
Copy code
from moviepy.editor import VideoClip, concatenate_videoclips
import numpy as np
import os
def get_video_params():
	# Ruta al archivo MP4
	ruta_video = 'coche.mp4'  # Reemplaza con la ruta de tu archivo MP4

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

	return (ancho,alto,duracion)

