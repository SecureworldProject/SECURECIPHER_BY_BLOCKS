#include <stdio.h>
#include <malloc.h>
#include <Windows.h>
#include <time.h>
#include <stdint.h>
#include "securecipher_n2.h"

//Funciones auxiliares
DWORD get_file_size(FILE* file){
    DWORD pos_actual = ftell(file);
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Error al desplazarse al final del archivo");
        return -1; // Retorna -1 para indicar un error
    }
    DWORD tam = ftell(file);
    if (fseek(file, pos_actual, SEEK_SET) != 0) {
        perror("Error al restaurar la posición original del puntero de archivo");
        return -1; // Retorna -1 para indicar un error
    }
    return tam;
}

char* inputFile(FILE* fp, size_t size) {
    //The size is extended by the input with the value of the provisional
    char* str;
    int ch;
    size_t len = 0;
    str = realloc(NULL, sizeof(*str) * size);//size is start size
    if (!str)return str;
    while (EOF != (ch = fgetc(fp))) {//&& ch != '\n') {
        str[len++] = ch;
        if (len == size) {
            str = realloc(str, sizeof(*str) * (size += 16));
            if (!str)return str;
        }
    }
    str[len++] = '\0';

    return realloc(str, sizeof(*str) * len);
}

//Main
int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Uso: %s -c(ifrar)|-d(escifrar) <nombre_archivo.ext> -k <keyfile>\n", argv[0]);
        return 1;
    }

    char* modo = argv[1]; //Modo: Cifrado o descifrado

    FILE* fichero_entrada = fopen(argv[2], "rb"); //Fichero para cifrar
    if (fichero_entrada == NULL) {
        printf("No se puede abrir el archivo: %s\n", argv[2]);
        return 1;
    }
    //Guardo el buffer para cifrar/descifrar
    int tam_buffer_entrada = get_file_size(fichero_entrada);
    char* buffer_entrada;// = malloc(tam_buffer_entrada);
    buffer_entrada = inputFile(fichero_entrada, 1);
    //Separo nombre y extension, y guardo ambos para luego escribir los ficheros de salida
    //TODO: Comprobar que hay punto y nombre, comprobar el nombre, etc...
    int tam_filename = strlen(argv[2]);
    const char* punto = strrchr(argv[2], '.'); 
    char* extension[255];//Aunque nunca es tan larga, este es el tamaño maximo de una extension en windows
    strcpy(extension, punto + 1);
    int tam_extension = strlen(extension);
    char* filename[255];
    int final_filename = tam_filename - tam_extension - 1;
    tam_filename = final_filename;
    strncpy(filename, argv[2], final_filename);
    filename[final_filename] = '\0';
    /*printf("La extension %s tiene un tamaño de %d\n",extension,strlen(extension));
    printf("El nombre %s tiene un tamaño de %d\n",filename,strlen(filename));
    printf("Final filename es %d\n", final_filename);*/
    fclose(fichero_entrada);

    struct KeyData* key;
    FILE* fichero_clave = fopen(argv[4], "rb"); //Fichero que contiene la clave
    if (fichero_clave == NULL) {
        printf("No se puede abrir el archivo: %s\n", argv[4]);
        return 1;
    }
    DWORD tam_clave = get_file_size(fichero_clave);
    byte* datos_clave = malloc(tam_clave);
    datos_clave = inputFile(fichero_clave,1);
    key = malloc(sizeof(struct KeyData));
    key->size = tam_clave;
    key->data = malloc(tam_clave);
    key->data = datos_clave;
    fclose(fichero_clave);

    //Cifrar/Descifrar
    int result;
    if (strcmp(modo, "-c") == 0) {
        //Preparar nombre y extension fichero salida
        char nombre_file_salida[255];
        snprintf(nombre_file_salida,tam_filename+tam_extension+4,"%s_c.%s",filename,extension);
        byte* buffer_salida = malloc(tam_buffer_entrada);
        //Cifrar
        result = cipher(buffer_salida, buffer_entrada, tam_buffer_entrada, key);
        if (result != 0) {
            printf("Error de cifrado (error: %d)\n",GetLastError());
        }
        //Escribir fichero salida
        FILE* fichero_salida = fopen(nombre_file_salida, "wb");
        fwrite(buffer_salida, 1, tam_buffer_entrada, fichero_salida);
        fclose(fichero_salida);
        printf("Cifrado completado, el nombre del fichero de salida es %s\n",nombre_file_salida);
    }
    else if (strcmp(modo, "-d") == 0) {
        //Preparar nombre y extension fichero salida
        char nombre_file_salida[255];
        snprintf(nombre_file_salida, tam_filename + tam_extension + 4, "%s_d.%s", filename, extension);
        byte* buffer_salida = malloc(tam_buffer_entrada);
        //Cifrar
        result = decipher(buffer_salida, buffer_entrada, tam_buffer_entrada, key);
        if (result != 0) {
            printf("Error de descifrado (error: %d)\n", GetLastError());
        }
        //Escribir fichero salida
        FILE* fichero_salida = fopen(nombre_file_salida, "wb");
        fwrite(buffer_salida, 1, tam_buffer_entrada, fichero_salida);
        fclose(fichero_salida);
        printf("Descifrado completado, el nombre del fichero de salida es %s\n", nombre_file_salida);
    }
    else {
        fprintf(stderr, "Modo no reconocido: %s\n", modo);
        fclose(fichero_entrada);
        return 1;
    }
    //frees

    return 0;
}
