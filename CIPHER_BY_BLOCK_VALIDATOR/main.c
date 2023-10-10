
#include "main.h"

DWORD print_hex(char* buf_name, void* buf, int size) {
	if (ENABLE_PRINTS) {
		printf("First %d bytes of %s contain:\n", size, buf_name);

		//byte [size*3 + size/32 + 1] str_fmt_buf;
		char* full_str = NULL;
		char* target_str = NULL;
		//int total = 0;

		// Size of string will consist on:
		//   (size*3)			 - 3 characters for every byte (2 hex characters plus 1 space). Space changed for '\n' every 32 bytes
		//   (size/8 - size/32)	 - Every 8 bytes another space is added after the space (if it is not multiple of 32, which already has '\n' instead)
		//   (1)				 - A '\n' is added at the end
		full_str = calloc((size * 3) + (size / 8 - size / 32) + (1), sizeof(char));
		if (full_str == NULL) {
			return ERROR_NOT_ENOUGH_MEMORY;
		}
		target_str = full_str;

		for (int i = 0; i < size; i++) {
			if ((i + 1) % 32 == 0) {
				target_str += sprintf(target_str, "%02hhX\n", ((byte*)buf)[i]);
			}
			else if ((i + 1) % 8 == 0) {
				target_str += sprintf(target_str, "%02hhX  ", ((byte*)buf)[i]);
			}
			else {
				target_str += sprintf(target_str, "%02hhX ", ((byte*)buf)[i]);
			}
		}
		target_str += sprintf(target_str, "\n");
		printf(full_str);
		free(full_str);
	}
	return ERROR_SUCCESS;
}

DWORD getFileSize(uint64_t* file_size, HANDLE handle, WCHAR* file_path) {
	BOOL opened = FALSE;
	DWORD error_code = ERROR_SUCCESS;

	// Ensure handle is valid (reopen the file if necessary)
	if (!handle || handle == INVALID_HANDLE_VALUE) {
		PRINT("Invalid file handle\n");
		handle = CreateFile(file_path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (handle == INVALID_HANDLE_VALUE) {
			error_code = GetLastError();
			PRINT("\tERROR creating handle to get file size (%d)\n", error_code);
			return error_code;
		}
		opened = TRUE;
	}

	// Using GetFileSizeEx() and passing directly the file_size pointer.
	// Maybe should check file_size > 0 (although that would mean that file_size > 8 EiB = 2^63 Bytes)
	if (!GetFileSizeEx(handle, (PLARGE_INTEGER)file_size)) {
		error_code = GetLastError();
		PRINT("\tcan not get a file size error = %d\n", error_code);
		if (opened)
			CloseHandle(handle);
		return error_code;
	};
	return error_code;
}

void size_test(char* ciphered, char* deciphered) {

	HANDLE handle_ciphered;
	HANDLE handle_deciphered;
	DWORD error_code;

	size_t file_size_ciphered = 0;
	size_t file_size_deciphered = 0;

	handle_ciphered = CreateFileA(ciphered, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	handle_deciphered = CreateFileA(deciphered, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (handle_ciphered == INVALID_HANDLE_VALUE || handle_deciphered == INVALID_HANDLE_VALUE )
	{
		printf("\nInvalid handle.\n");
	}
	else {
		error_code = getFileSize(&file_size_ciphered, handle_ciphered, ciphered);
		if (error_code != ERROR_SUCCESS) {
			printf("\nError obtaining file size\n");
		}
		printf("The detected ciphered file size is %llub.\n", file_size_ciphered);

		error_code = getFileSize(&file_size_deciphered, handle_deciphered, deciphered);
		if (error_code != ERROR_SUCCESS) {
			printf("\nError obtaining file size\n");
		}
		printf("The detected deciphered file size is %llub.\n", file_size_deciphered);
	}
	if (file_size_ciphered == file_size_ciphered) {
		printf("\nTest passed. File sizes are the same.\n");
	}
	else {
		printf("\nTest failed. File sizes are different.\n");
	}
	CloseHandle(handle_ciphered);
	CloseHandle(handle_deciphered);

}

void byte_test(DWORD b_size, char* msg, byte* cip_buf, byte* dec_buf) { //***En vez de coger un char, coger 10 randoms(?)
	char caracter;
	char caracter_ciphered;
	char caracter_deciphered;

	caracter = msg[5];
	caracter_ciphered = cip_buf[5];
	caracter_deciphered = dec_buf[5];

	printf("Clear:%c\nCiphered:%c\nDeciphered:%c\n", caracter, caracter_ciphered, caracter_deciphered);
	if (caracter == caracter_deciphered) {
		printf("\nTest passed.\n");
	}
	else {
		printf("\nTest failed.\n");
	}

}


BOOL ciph_menu = FALSE;
BOOL main_menu = FALSE;

WCHAR line[SIZE_OF_BUFF] = { 0 };
WCHAR json[SIZE_OF_BUFF] = { 0 };
WCHAR dll[SIZE_OF_BUFF] = { 0 };
int result;
size_t tam;
size_t tam_test;

typedef int(__stdcall* test_func_type)();
test_func_type test_func;

typedef int(__stdcall* cipher_init_func_type)(struct Cipher*);
cipher_init_func_type cipher_init_func;
typedef int(__stdcall* cipher_func_type)(LPVOID, LPCVOID, DWORD, size_t, struct KeyData*);
cipher_func_type cipher_func;
cipher_func_type decipher_func;


//Creation of cipher struct (and pointer)
struct Cipher cipher;
//Creation of composed key
struct KeyData* composed_key;


void menuLoop() {
	//Main loop
	do {
		main_menu = FALSE;
		printf("\n\n");
		printf("  _______________________  \n");
		printf(" |                       | \n");
		printf(" |   CIPHER VALIDATOR    | \n");
		printf(" |_______________________| \n");
		printf("\n");
		//Request DLL file input
		printf("Enter DLL name (0 to exit):\n");
		fgetws(line, sizeof(line), stdin);
		if (wcscmp(line, L"0\n") == 0) {
			break;
		} else if (1 == swscanf(line, L"%ws", dll)) {
			//Check DLL file and load if possible
			HINSTANCE hLib;
			SetErrorMode(0);
			hLib = LoadLibraryW(dll);

			if (hLib != NULL) {
				printf("DLL loaded. \n");

				//Look for cipher or executeChallenge functions in the DLL
				cipher_func = (cipher_func_type)GetProcAddress(hLib, "cipher");
				//execute_func = (execute_func_type)GetProcAddress(hLib, "executeChallenge");

				//Cipher DLL
				if (cipher_func != NULL) { //There is a Cipher function...
					printf("%ws is a cipher DLL. \n", dll);
					cipher.lib_handle = hLib;
					cipher.file_name = dll;
					cipher.block_size = 8;
					cipher.id = dll;

					composed_key = malloc(1 * sizeof(struct KeyData));
					composed_key->size = 5;
					composed_key->data = malloc(composed_key->size, sizeof(byte));
					composed_key->data = (byte*)"12345";
					composed_key->expires = 0;

					//Init function call from the DLL
					cipher_init_func = (cipher_init_func_type)GetProcAddress(hLib, "init");
					if (cipher_init_func != NULL) {
						int result = cipher_init_func(&cipher);
						if (result != 0) {
							PRINT("WARNING: error \n");
						}
					} else {
						PRINT("WARNING: error accessing the address to the init() function of the cipher '%ws' (error: %d)\n", dll, GetLastError());
					}

					int choice = 0;

					//Source buffer
					char* file = "el_quijote.txt";
					FILE* f_original = NULL;
					f_original = fopen(file, "r");
					char* message;
					message = inputFile(f_original, 1);
					tam = strlen(message);
					fclose(f_original);

					//Destination buffers
					DWORD buf_size = tam;
					size_t offset = 0;

					char* file_ciphered = "el_quijote_ciphered.txt";
					FILE* f_ciphered = NULL;
					byte* ciphered_buf = malloc(buf_size * sizeof(byte));

					char* file_deciphered = "el_quijote_deciphered.txt";
					FILE* f_deciphered = NULL;
					byte* deciphered_buf = malloc(buf_size * sizeof(byte));

					//Smaller buffers for specific tests
					char* message_test = "Hola mundo estoy haciendo pruebas";
					tam_test = strlen(message_test);
					DWORD buf_size_test = tam_test;
					byte* ciphered_buf_test = malloc(buf_size_test * sizeof(byte));
					byte* deciphered_buf_test = malloc(buf_size_test * sizeof(byte));

					//CIPHERING

					//Ciphering smaller buffer (original -> cipher)
					result = cipher_func(ciphered_buf_test, message_test, buf_size_test, offset, composed_key);
					//Ciphering original buffer (original -> cipher)
					long long int begin = GetTickCount64();
					result = cipher_func(ciphered_buf, message, buf_size, offset, composed_key);
					long long int end = GetTickCount64();
					f_ciphered = fopen(file_ciphered, "wb");
					fwrite(ciphered_buf, 1, tam, f_ciphered);
					if (result != 0) {
						PRINT("WARNING: error ciphering '%ws' (error: %d)\n", dll, GetLastError()); ///*** Especificar error si es posible
					}

					//DECIPHERING
					decipher_func = (cipher_func_type)GetProcAddress(hLib, "decipher");
					if (decipher_func != NULL) {
						//Deciphering smaller buffer (original -> decipher)
						result = decipher_func(deciphered_buf_test, message_test, buf_size_test, offset, composed_key);
						//Deciphering original buffer (cipher -> decipher)
						result = decipher_func(deciphered_buf, ciphered_buf, buf_size, offset, composed_key);
						f_deciphered = fopen(file_deciphered, "wb");
						fwrite(deciphered_buf, 1, tam, f_deciphered);
						if (result != 0) {
							PRINT("WARNING: error deciphering '%ws' (error: %d)\n", dll, GetLastError());
						}
					} else {
						PRINT("WARNING: error accessing the address to the decipher() function of the cipher '%s' (error: %d)\n", dll, GetLastError());
					}

					do {
						ciph_menu = FALSE;
						printf("\n\n");
						printf("  _______________________  \n");
						printf(" |                       | \n");
						printf(" |      CIPHER MENU      | \n");
						printf(" |_______________________| \n");
						printf("\n");

						printf("Select an option:\n");
						printf("  1) Functionality test \n");
						printf("  2) Robustness test \n");
						printf("  3) Block cipher test for video \n");
						printf("  4) Block cipher test for text \n");
						printf("  0) Back to main menu \n");
						if (fgets(line, sizeof(line), stdin)) {
							if (1 == sscanf(line, "%d", &choice)) {
								switch (choice) {
									case 1:

										do {
											printf("\nSelect test to run:\n");
											printf("  1) Size \n");
											printf("  2) Byte \n");
											printf("  3) Cipher not Decipher \n");
											printf("  4) Decipher is valid \n");
											printf("  5) Run all tests \n");
											printf("  0) Back to cipher menu \n");
											if (fgets(line, sizeof(line), stdin)) {
												if (1 == sscanf(line, "%d", &choice)) {
													switch (choice) {
														case 1:
															//Checks if the size of ciphered and deciphered files are the same.
															size_test(file_ciphered, file_deciphered);
															break;
														case 2:
															//Checks if the cipher and decipher operations are executed correctly.***
															//***Mostrar 10 bytes random
															byte_test(buf_size, message, ciphered_buf, deciphered_buf);
															break;
														case 3:
															PRINT_HEX(ciphered_buf_test, tam_test)
																printf("Clear text:%s\nFrom clear to ciphered:%s\nFrom clear to deciphered:%s\n", message_test, ciphered_buf_test, deciphered_buf_test);
															break;
														case 4:
															printf("Text in clear:%s\nFrom clear to deciphered:%s\n", message_test, deciphered_buf_test);
															break;
														case 5:
															printf("Running all tests...\n");
															size_test(file_ciphered, file_deciphered);
															byte_test(buf_size, message, ciphered_buf, deciphered_buf);
															printf("Clear text:%s\nFrom clear to ciphered:%s\nFrom clear to deciphered:%s\n", message_test, ciphered_buf_test, deciphered_buf_test);
															printf("Text in clear:%s\nFrom clear to deciphered:%s\n", message_test, deciphered_buf_test);
															break;
														case 0:
															printf("Cipher menu... \n");
															ciph_menu = TRUE;
															break;
														default:
															printf("Invalid option, try again.\n");
															break;
													}
												}

											}

										} while (!ciph_menu);
										break;
									case 2:
										printf("ROBUSTNESS TEST \n");
										double elapsed = (end - begin) * 1e-3;
										printf("Time to complete cipher: %.3f seconds\n", elapsed);
										HANDLE handle_aux = CreateFileA(file_ciphered, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
										size_t file_size_cip = 0;
										getFileSize(&file_size_cip, handle_aux, file_ciphered);
										CloseHandle(handle_aux);
										printf("Bytes ciphered: %llu bytes\n", file_size_cip);
										float speed = file_size_cip / elapsed;
										printf("Ciphered at %.3f bytes/second\n", speed);
										break;
									case 3:
										printf("BLOCK CIPHER TEST VIDEO\n");
										//test_block_cipher();
										const char* directorio_trozos = "trozos";
										FILE* archivo;
										// Iterar sobre los archivos en el directorio
										for (int i = 1; i <= 4; ++i) {
											PRINT("Voy por el trozo %d\n", i);
											// Construir la ruta del archivo
											char nombre_archivo[50];  // Ajusta el tamaño según tus necesidades
											snprintf(nombre_archivo, sizeof(nombre_archivo), "%s/trozo_%d.mp4", directorio_trozos, i);

											// Abrir el archivo
											PRINT("%s\n",nombre_archivo);
											archivo = fopen(nombre_archivo, "rb");//Para video se lee en binario
											if (archivo == NULL) {
												perror("Error al abrir el archivo");
											}
											char* message;
											message = inputFile(archivo, 1);
											//tam = strlen(message); //Esto funciona solo para texto
											if (fseek(archivo, 0, SEEK_END) != 0) {
												perror("Error al buscar el final del archivo");
												// Manejar el error según sea necesario
											}

											// Obtener la posición actual, que es el tamaño del archivo
											tam = ftell(archivo);
											if (tam == -1) {
												perror("Error al obtener el tamaño del archivo");
												// Manejar el error según sea necesario
											}
											PRINT("\tEl tam del buffer sera: %d\n",tam);
											fclose(archivo);

											// Realizar operaciones en el archivo (aquí puedes agregar tu lógica)
											//cifrar y guardar cifrado
											//Destination buffers
											DWORD buf_size = tam;
											size_t offset = 0;

											char nombre_archivo_cifrado[50];
											snprintf(nombre_archivo_cifrado, sizeof(nombre_archivo_cifrado), "%s/trozo_%d_cifrado", directorio_trozos, i);
											FILE* f_ciphered = NULL;
											byte* ciphered_buf = malloc(buf_size * sizeof(byte));

											//CIPHERING
											result = cipher_func(ciphered_buf, message, buf_size, offset, composed_key);
											f_ciphered = fopen(nombre_archivo_cifrado, "wb");
											fwrite(ciphered_buf, 1, tam, f_ciphered);
											if (result != 0) {
												PRINT("WARNING: error ciphering '%ws' (error: %d)\n", dll, GetLastError()); ///*** Especificar error si es posible
											}
											fclose(f_ciphered);


											//descifrar y guardar descifrado
											char nombre_archivo_descifrado[50];
											snprintf(nombre_archivo_descifrado, sizeof(nombre_archivo_descifrado), "%s/trozo_%d_descifrado.mp4", directorio_trozos, i);
											FILE* f_deciphered = NULL;
											byte* deciphered_buf = malloc(buf_size * sizeof(byte));

											//DECIPHERING
											//Deciphering original buffer (cipher -> decipher)
											result = decipher_func(deciphered_buf, ciphered_buf, buf_size, offset, composed_key);
											f_deciphered = fopen(nombre_archivo_descifrado, "wb");
											fwrite(deciphered_buf, 1, tam, f_deciphered);
											if (result != 0) {
												PRINT("WARNING: error deciphering '%ws' (error: %d)\n", dll, GetLastError());
											}
											fclose(f_deciphered);

										}
										break;
									case 4:
										printf("BLOCK CIPHER TEST TXT\n");
										//test_block_cipher();
										const char* directorio_trozos_txt = "trozos_txt";
										FILE* archivo_txt;
										// Iterar sobre los archivos en el directorio
										for (int i = 1; i <= 10; ++i) {
											PRINT("Voy por el trozo %d\n", i);
											// Construir la ruta del archivo
											char nombre_archivo[50];  // Ajusta el tamaño según tus necesidades
											snprintf(nombre_archivo, sizeof(nombre_archivo), "%s/trozo_%d.txt", directorio_trozos_txt, i);

											// Abrir el archivo
											PRINT("%s\n", nombre_archivo);
											archivo_txt = fopen(nombre_archivo, "rb");//Para video se lee en binario
											if (archivo_txt == NULL) {
												perror("Error al abrir el archivo");
											}
											char* message;
											message = inputFile(archivo_txt, 1);
											tam = strlen(message); //Esto funciona solo para texto
											
											PRINT("\tEl tam del buffer sera: %d\n", tam);
											fclose(archivo_txt);

											// Realizar operaciones en el archivo (aquí puedes agregar tu lógica)
											//cifrar y guardar cifrado
											//Destination buffers
											DWORD buf_size = tam;
											size_t offset = 0;

											char nombre_archivo_cifrado[50];
											snprintf(nombre_archivo_cifrado, sizeof(nombre_archivo_cifrado), "%s/trozo_%d_cifrado", directorio_trozos_txt, i);
											FILE* f_ciphered = NULL;
											byte* ciphered_buf = malloc(buf_size * sizeof(byte));

											//CIPHERING
											result = cipher_func(ciphered_buf, message, buf_size, offset, composed_key);
											f_ciphered = fopen(nombre_archivo_cifrado, "wb");
											fwrite(ciphered_buf, 1, tam, f_ciphered);
											if (result != 0) {
												PRINT("WARNING: error ciphering '%ws' (error: %d)\n", dll, GetLastError()); ///*** Especificar error si es posible
											}
											fclose(f_ciphered);


											//descifrar y guardar descifrado
											char nombre_archivo_descifrado[50];
											snprintf(nombre_archivo_descifrado, sizeof(nombre_archivo_descifrado), "%s/trozo_%d_descifrado.txt", directorio_trozos_txt, i);
											FILE* f_deciphered = NULL;
											byte* deciphered_buf = malloc(buf_size * sizeof(byte));

											//DECIPHERING
											//Deciphering original buffer (cipher -> decipher)
											result = decipher_func(deciphered_buf, ciphered_buf, buf_size, offset, composed_key);
											f_deciphered = fopen(nombre_archivo_descifrado, "wb");
											fwrite(deciphered_buf, 1, tam, f_deciphered);
											if (result != 0) {
												PRINT("WARNING: error deciphering '%ws' (error: %d)\n", dll, GetLastError());
											}
											fclose(f_deciphered);

										}
										break;
									case 0:
										printf("Main menu... \n");
										main_menu = TRUE;
										break;
									default:
										printf("Invalid option, try again.\n");
										break;
								}
							}
						}

					} while (!main_menu);

				}
			} else printf("DLL not loaded.\n");

			//if (hLib != NULL) FreeLibrary(hLib);
		}
	} while (TRUE);
	exit(0);
}


void main() {

	menuLoop();
}
