/*
* SecureWorld file context.h
contiene la estructura en C en la que se carga el contexto escrito en el fichero config.json, y que contiene la configuración de todo el sistema.
Los distintos proyectos (challenges, mirror, app descifradora etc) deben incluir este fichero para compilar pues sus DLL
se invocarán con un parámetro de tipo contexto que se definirá en context.h

Nokia Febrero 2021
*/

#ifndef context_h
#define context_h

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "windows.h"
#include "json.h"
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif	//__cplusplus


	/////  DEFINITIONS  /////

#ifndef NULL
#define NULL ((void*)0)
#endif //NULL

#ifndef MAX_PATH
#define MAX_PATH 260
#endif //MAX_PATH

#ifndef SUBKEY_SIZE
#define SUBKEY_SIZE 64
#endif //SUBKEY_SIZE


#define NOOP ((void)0)
#define ENABLE_PRINTS 1					// Affects the PRINT() functions. If 0 does not print anything. If 1 traces are printed.
#define PRINT(...) do { if (ENABLE_PRINTS) printf(__VA_ARGS__); else NOOP;} while (0)
#define PRINT1(...) PRINT("    "); PRINT(__VA_ARGS__)
#define PRINT2(...) PRINT("        "); PRINT(__VA_ARGS__)
#define PRINT3(...) PRINT("            "); PRINT(__VA_ARGS__)
#define PRINT4(...) PRINT("                "); PRINT(__VA_ARGS__)
#define PRINT5(...) PRINT("                    "); PRINT(__VA_ARGS__)
#define PRINTX(DEPTH, ...) do { if (ENABLE_PRINTS) { for (int x=0; x<DEPTH; x++){ printf("    ");} printf(__VA_ARGS__); } else NOOP;} while (0)

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

struct KeyData {
	byte* data;
	int size;
	time_t expires;		// In the case of the full key we will take the first expire date
	CRITICAL_SECTION critical_section;
};


struct Cipher {
	char* id;
	WCHAR* file_name;
	HINSTANCE lib_handle;
	int block_size;
	char* custom;
};

#ifdef __cplusplus
}
#endif	//__cplusplus

#endif	//context_h