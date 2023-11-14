#include <stdio.h>
#include <malloc.h>
#include <Windows.h>
#include <time.h>
#include <stdint.h>
#include "securecipher_n2.h"

//Para medir tiempos
clock_t start;
clock_t end;
float seconds;

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
    int modo_nal = 0;//No hay nal por defecto
    if (argc < 5 || argc > 7) {
        printf("Uso: %s -c(ifrar)|-d(escifrar) <nombre_archivo.ext> -k <keyfile> [-N <newkeyfile>]\n", argv[0]);
        return 1;
    }

    //Modo: Cifrado o descifrado
    char* modo = argv[1]; 

    //Fichero que se va a cifrar/descifrar
    FILE* fichero_entrada = fopen(argv[2], "rb");
    if (fichero_entrada == NULL) {
        printf("No se puede abrir el archivo: %s\n", argv[2]);
        return 1;
    }
    //Guardo el buffer para cifrar/descifrar
    int tam_buffer_entrada = get_file_size(fichero_entrada);
    printf("El fichero de entrada %s mide %d bytes\n",argv[2],tam_buffer_entrada);
    char* buffer_entrada;// = malloc(tam_buffer_entrada);
    buffer_entrada = inputFile(fichero_entrada, 1);
    //Separo nombre y extension, y guardo ambos para luego escribir los ficheros de salida
    int tam_filename = strlen(argv[2]);
    const char* punto = strrchr(argv[2], '.'); 
    char extension[255];//Aunque nunca es tan larga, este es el tamaño maximo de una extension en windows
    strcpy(extension, punto + 1);
    int tam_extension = strlen(extension);
    char filename[255];
    int final_filename = tam_filename - tam_extension -1;
    tam_filename = final_filename;
    strncpy(filename, argv[2], final_filename);
    filename[final_filename] = '\0';
    fclose(fichero_entrada);

    //Preparo la clave
    struct KeyData* key;
    FILE* fichero_clave = fopen(argv[4], "rb"); 
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
    key->next_key = NULL;
    key->keyfile = argv[4];
    fclose(fichero_clave);

    //Trato el NAL
    if ((argc == 7) && (strcmp(argv[5], "-N")==0)) {
        //Guardar la segunda clave y llamar al cifrado con el modo Nal, variable Nal = 01 por ejemplo
        modo_nal = 1;//Nal de cambio de clave
        struct KeyData* newkey;
        FILE* fichero_nueva_clave = fopen(argv[6], "rb"); //Fichero que contiene la nueva clave
        if (fichero_nueva_clave == NULL) {
            printf("No se puede abrir el archivo: %s\n", argv[6]);
            return 1;
        }
        DWORD tam_nueva_clave = get_file_size(fichero_clave);
        byte* datos_nueva_clave = malloc(tam_nueva_clave);
        datos_nueva_clave = inputFile(fichero_nueva_clave, 1);
        newkey = malloc(sizeof(struct KeyData));
        newkey->size = tam_nueva_clave;
        newkey->data = malloc(tam_nueva_clave);
        newkey->data = datos_nueva_clave;
        newkey->keyfile = argv[6];
        fclose(fichero_nueva_clave);
        key->next_key = newkey;
    }
    else if ((argc == 7)){
        printf("Modo NAL no reconocido %s\n", argv[5]);
        return 1;
    }

    //Cifrar/Descifrar
    int result;
    if (strcmp(modo, "-c") == 0) {
        //Preparar nombre y extension fichero salida
        char nombre_file_salida[255];
        snprintf(nombre_file_salida,tam_filename+tam_extension+4,"%s_c.%s",filename,extension);
        byte* buffer_salida = NULL;
        //Cifrar
        start = clock();
        result = cipher(&buffer_salida, buffer_entrada, tam_buffer_entrada, key);
        end = clock();
        seconds = (float)(end - start) / CLOCKS_PER_SEC;
        //Escribir fichero salida
        FILE* fichero_salida = fopen(nombre_file_salida, "wb");
        fwrite(buffer_salida, 1, result, fichero_salida);
        fclose(fichero_salida);
        free(buffer_salida);
        printf("- Cifrado completado, el nombre del fichero de salida es %s y ocupa %d bytes\n",nombre_file_salida, result);
        printf("- Tiempo: %.16f segundos\n",seconds);
    }
    else if (strcmp(modo, "-d") == 0) {
        //Preparar nombre y extension fichero salida
        char nombre_file_salida[255];
        snprintf(nombre_file_salida, tam_filename + tam_extension + 4, "%s_d.%s", filename, extension);
        byte* buffer_salida = NULL;
        //Descifrar
        start = clock();
        result = decipher(&buffer_salida, buffer_entrada, tam_buffer_entrada, key);
        end = clock();
        seconds = (float)(end - start) / CLOCKS_PER_SEC;
        //Escribir fichero salida
        FILE* fichero_salida = fopen(nombre_file_salida, "wb");
        fwrite(buffer_salida, 1, result, fichero_salida);
        fclose(fichero_salida);
        free(buffer_salida);
        printf("- Descifrado completado, el nombre del fichero de salida es %s y ocupa %d bytes\n", nombre_file_salida,result);
        printf("- Tiempo: %.16f segundos\n", seconds);
    }
    else {
        fprintf(stderr, "Modo no reconocido: %s\n", modo);
        fclose(fichero_entrada);
        return 1;
    }

    return 0;
}
