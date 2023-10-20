#include "securecipher_n2.h"

byte* get_message(byte last_byte, struct KeyData* key) {
    byte message[20] = { 0 };
    //Preparar el message de 20 bytes por cada posicion, y los elementos por separado;
    //El primer byte es el byte anterior
    message[0] = last_byte;
    byte key_array[19];
    int key_size = key->size;
    //Si la key es mucho mas larga, hago xor para reducirla a 19, si la key es mas corta la repito
    int tam_key = key->size;
    //si es menor, se hace bucle de 19, y se repite a partir de tam_key
    if (tam_key < 19) {
        for (int i = 0; i < 19; i++) {
            key_array[i] = key->data[i % tam_key];
        }
    }
    else {//Si es mayor o igual se iguala o se hace xor con el resto
        key_array[0] = key->data[0];
        key_array[1] = key->data[1];
        key_array[2] = key->data[2];
        key_array[3] = key->data[3];
        key_array[4] = key->data[4];
        key_array[5] = key->data[5];
        key_array[6] = key->data[6];
        key_array[7] = key->data[7];
        key_array[8] = key->data[8];
        key_array[9] = key->data[9];
        key_array[10] = key->data[10];
        key_array[11] = key->data[11];
        key_array[12] = key->data[12];
        key_array[13] = key->data[13];
        key_array[14] = key->data[14];
        key_array[15] = key->data[15];
        key_array[16] = key->data[16];
        key_array[17] = key->data[17];
        key_array[18] = key->data[18];
        for (int i = 19; i < tam_key; i++) {
            int j = 0;
            key_array[j] = key_array[j] ^ key->data[i];
            j = (j + 1) % 19;
        }
    }
    //key
    for (int i = 1; i < 20; i++) {
        message[i] = key_array[i - 1];
    }
    return message;
}

byte* lineal_transform(byte* message) {

    byte a[10] = { message[0], message[1], message[2], message[3], message[4] ,message[5], message[6], message[7], message[8], message[9] };
    byte b[10] = { message[10], message[11], message[12], message[13], message[14], message[15], message[16], message[17], message[18], message[19] };
    //printf("a = %x %x %x %x %x %x %x %x %x %x\n", a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]);
    //printf("b = %x %x %x %x %x %x %x %x %x %x\n", b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7], b[8], b[9]);
    //aux_a = a+b
    byte aux_a[11];
    //a = a + b
    byte overflow = 0x00;
    for (int i = 9; i > -1; i--) {
        aux_a[i + 1] = a[i] + b[i] + overflow;
        //printf("Resultado parcial: %x + %x (+%x)= %x\n", a[i], b[i], overflow, aux_a[i + 1]);
        if (aux_a[i + 1] > 0xF) {
            overflow = 0x01;
        }
        else {
            overflow = 0x00;
        }
    }
    if (overflow == 0x01) { aux_a[0] = 0x01; }
    else {
        aux_a[0] = aux_a[1];
        aux_a[1] = aux_a[2];
        aux_a[2] = aux_a[3];
        aux_a[3] = aux_a[4];
        aux_a[4] = aux_a[5];
        aux_a[5] = aux_a[6];
        aux_a[6] = aux_a[7];
        aux_a[7] = aux_a[8];
        aux_a[8] = aux_a[9];
        aux_a[9] = aux_a[10];
        aux_a[10] = aux_a[0];
    }
    //printf("resul aux_a = %x %x %x %x %x %x %x %x %x %x %x\n", aux_a[0], aux_a[1], aux_a[2], aux_a[3], aux_a[4], aux_a[5], aux_a[6], aux_a[7], aux_a[8], aux_a[9], aux_a[10]);

    //b = aux_a + 2b
    //2b
    byte aux_b[11];
    overflow = 0x00;
    for (int i = 9; i > -1; i--) {
        aux_b[i + 1] = b[i] + b[i] + overflow;
        //printf("Resultado parcial: %x + %x (+%x)= %x\n", b[i], b[i], overflow, aux_b[i + 1]);
        if (aux_b[i + 1] > 0xF) {
            overflow = 0x01;
        }
        else {
            overflow = 0x00;
        }
    }
    if (overflow == 0x01) { aux_b[0] = 0x01; }
    else {
        aux_b[0] = aux_b[1];
        aux_b[1] = aux_b[2];
        aux_b[2] = aux_b[3];
        aux_b[3] = aux_b[4];
        aux_b[4] = aux_b[5];
        aux_b[5] = aux_b[6];
        aux_b[6] = aux_b[7];
        aux_b[7] = aux_b[8];
        aux_b[8] = aux_b[9];
        aux_b[9] = aux_b[10];
        aux_b[10] = aux_b[0];
    }
    //PRINTf("resul aux_b = %x %x %x %x %x %x %x %x %x %x %x\n", aux_b[0], aux_b[1], aux_b[2], aux_b[3], aux_b[4], aux_b[5], aux_b[6], aux_b[7], aux_b[8], aux_b[9], aux_b[10]);

    //aux_bb = aux_a+aux_b
    byte aux_bb[12];
    overflow = 0x00;
    for (int i = 10; i > -1; i--) { //sumo dos arrays de 11
        aux_bb[i + 1] = aux_a[i] + aux_b[i] + overflow;
        //PRINTf("Resultado parcial: %x + %x (+%x)= %x\n", aux_a[i], aux_b[i], overflow, aux_bb[i + 1]);
        if (aux_bb[i + 1] > 0xF) {
            overflow = 0x01;
        }
        else {
            overflow = 0x00;
        }
    }
    if (overflow == 0x01) { aux_bb[0] = 0x01; }
    else {
        aux_bb[0] = aux_bb[1];
        aux_bb[1] = aux_bb[2];
        aux_bb[2] = aux_bb[3];
        aux_bb[3] = aux_bb[4];
        aux_bb[4] = aux_bb[5];
        aux_bb[5] = aux_bb[6];
        aux_bb[6] = aux_bb[7];
        aux_bb[7] = aux_bb[8];
        aux_bb[8] = aux_bb[9];
        aux_bb[9] = aux_bb[10];
        aux_bb[10] = aux_bb[11];
        aux_bb[11] = aux_bb[0];
    }
    //PRINTf("resul aux_bb= %x %x %x %x %x %x %x %x %x %x %x %x\n", aux_bb[0], aux_bb[1], aux_bb[2], aux_bb[3], aux_bb[4], aux_bb[5], aux_bb[6], aux_bb[7], aux_bb[8], aux_bb[9], aux_bb[10], aux_bb[11]);

    //actualizo el message
    //byte message[20];
    message[0] = aux_a[0];
    message[1] = aux_a[1];
    message[2] = aux_a[2];
    message[3] = aux_a[3];
    message[4] = aux_a[4];
    message[5] = aux_a[5];
    message[6] = aux_a[6];
    message[7] = aux_a[7];
    message[8] = aux_a[8];
    message[9] = aux_a[9];
    message[10] = aux_bb[0];
    message[11] = aux_bb[1];
    message[12] = aux_bb[2];
    message[13] = aux_bb[3];
    message[14] = aux_bb[4];
    message[15] = aux_bb[5];
    message[16] = aux_bb[6];
    message[17] = aux_bb[7];
    message[18] = aux_bb[8];
    message[19] = aux_bb[9];
    //PRINT("Message: %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx %hhx\n", message[0], message[1], message[2], message[3], message[4], message[5], message[6], message[7], message[8], message[9], message[10], message[11], message[12], message[13], message[14], message[15], message[16], message[17], message[18], message[19]);

    return message;
}

byte confusion(byte* message) {
    //confusion: message -> byte ciphered
    //convierto el mensaje en 4 numeros de 5 bytes
    byte variable_0[5] = { message[0], message[1], message[2], message[3], message[4] };
    byte variable_1[5] = { message[5], message[6], message[7], message[8], message[9] };
    byte variable_2[5] = { message[10], message[11], message[12], message[13], message[14] };
    byte variable_3[5] = { message[15], message[16], message[17], message[18], message[19] };
    //Ahora puedo sacar el valor entero, o puedo operar sobre cada byte

    uint64_t value_0 = ((uint64_t)variable_0[0] << 32) + ((uint64_t)variable_0[1] << 24) +
        ((uint64_t)variable_0[2] << 16) + ((uint64_t)variable_0[3] << 8) + (uint64_t)variable_0[4];
    //PRINT("\tValue_0 como int: %"PRIu64" y %"PRIx64"\n", value_0, value_0);
    uint64_t value_1 = ((uint64_t)variable_1[0] << 32) + ((uint64_t)variable_1[1] << 24) +
        ((uint64_t)variable_1[2] << 16) + ((uint64_t)variable_1[3] << 8) + (uint64_t)variable_1[4];
    //PRINT("\tValue_1 como int: %"PRIu64" y %"PRIx64"\n", value_0, value_1);
    uint64_t value_2 = ((uint64_t)variable_2[0] << 32) + ((uint64_t)variable_2[1] << 24) +
        ((uint64_t)variable_2[2] << 16) + ((uint64_t)variable_2[3] << 8) + (uint64_t)variable_2[4];
    //PRINT("\tValue_0 como int: %"PRIu64" y %"PRIx64"\n", value_2, value_2);
    uint64_t value_3 = ((uint64_t)variable_3[0] << 32) + ((uint64_t)variable_3[1] << 24) +
        ((uint64_t)variable_3[2] << 16) + ((uint64_t)variable_3[3] << 8) + (uint64_t)variable_3[4];
    //PRINT("\tValue_0 como int: %"PRIu64" y %"PRIx64"\n", value_3, value_3);


    //uint64_t resultado_xor = value_0 ^ value_1;
    uint64_t resultado = (0xFF) ^ (value_3) ^ (value_2) ^ (value_3 & value_1) ^ (value_2 & value_1) ^ (value_3 & value_2 & value_0) ^ (value_3 & value_1 & value_0) ^ (value_2 & value_1 & value_0) ^ (value_3 & value_2 & value_1 & value_0);
    //PRINT("\tResultado: %"PRIu64" y %"PRIx64" y entero %d\n", resultado, resultado, resultado % 256);
    resultado = resultado % 256;
    //PRINT("\tResultado final: %d\n", resultado);
    return resultado;
}

//int cipher(LPVOID out_buf, LPCVOID in_buf, DWORD size, size_t offset, struct KeyData* key) { //offset es la posicion en el fichero, en esta version no es necesario
int cipher(LPVOID out_buf, LPCVOID in_buf, DWORD size, struct KeyData* key, int nal_mode) { //offset es la posicion en el fichero, en esta version no es necesario
    
    byte* message = (byte*)malloc(20 * sizeof(byte));
    size_t buf_pos = 0; //posicion en el bufer de cifrado
    byte last_byte = 0xFF;
    //byte pattern[5] = { 0x01, 0x45, 0xFC, 0xC7, 0xA2 }; //solo si uso memcmp
    int pos_nal;
    if (nal_mode == 1) { pos_nal = 45; }
    for (buf_pos; buf_pos < size; buf_pos++) {
        if (nal_mode == 1) {
            /*if (buf_pos >= 4) {  // Se puede cambiar para mirar cada n bytes
                if (((byte*)in_buf)[buf_pos - 4] == 0x01 &&
                    ((byte*)in_buf)[buf_pos - 3] == 0x45 &&
                    ((byte*)in_buf)[buf_pos - 2] == 0xFC &&
                    ((byte*)in_buf)[buf_pos - 1] == 0xC7 &&
                    ((byte*)in_buf)[buf_pos] == 0xA2) {
                    printf("Encontrada la cadena de bytes en la posición %zu\n", buf_pos - 4);
                }
            }*/
            if (buf_pos == pos_nal) {
                //out_buf = (byte*)realloc(out_buf, (size + 5) * sizeof(byte));
                ((byte*)in_buf)[buf_pos] = 0x01;
                ((byte*)in_buf)[buf_pos + 1] = 0x45;
                ((byte*)in_buf)[buf_pos + 2] = 0xFC;
                ((byte*)in_buf)[buf_pos + 3] = 0xC7;
                ((byte*)in_buf)[buf_pos + 4] = 0xA2;
                //buf_pos = buf_pos + 4;
                printf("Secuencia introducida\n");
            }
        }
        //message = get_message(last_byte, key);
        memcpy(message, get_message(last_byte, key), 20);
        //Hago la transformacion lineal y actualizo el message
        message = lineal_transform(message);
        //Confusion
        byte resultado = confusion(message);
        ((byte*)out_buf)[buf_pos] = (((byte*)in_buf)[buf_pos] ^ resultado) % 256;
        last_byte = ((byte*)in_buf)[buf_pos];
    }
    //printf("Buffer ciphered\n");
    free(message);
    
    return 0;
}

//int decipher(LPVOID out_buf, LPCVOID in_buf, DWORD size, size_t offset, struct KeyData* key) {
int decipher(LPVOID out_buf, LPCVOID in_buf, DWORD size, struct KeyData* key, int nal_mode) {

    byte* message = (byte*)malloc(20 * sizeof(byte));
    size_t buf_pos = 0; //posicion en el bufer, solo valida para escribir en el bufer, para cifrar se usa la posicion real en el fichero
    byte last_byte = 0xFF;
    byte pattern[5] = { 0x01, 0x45, 0xFC, 0xC7, 0xA2 };
    for (buf_pos; buf_pos < size; buf_pos++) {
        //message = get_message(last_byte, key);
        memcpy(message, get_message(last_byte, key), 20);
        //Hago la transformacion lineal y actualizo el message
        message = lineal_transform(message);
        //Confusion
        byte resultado = confusion(message);
        ((byte*)out_buf)[buf_pos] = (((byte*)in_buf)[buf_pos] ^ resultado) % 256;
        last_byte = ((byte*)out_buf)[buf_pos];
        //Añado el calculo del nal
        if (nal_mode == 1) {
            if (buf_pos >= 4) {  // Asegúrate de que haya al menos 5 bytes en el buffer
                if (((byte*)in_buf)[buf_pos - 4] == 0x01 &&
                    ((byte*)in_buf)[buf_pos - 3] == 0x45 &&
                    ((byte*)in_buf)[buf_pos - 2] == 0xFC &&
                    ((byte*)in_buf)[buf_pos - 1] == 0xC7 &&
                    ((byte*)in_buf)[buf_pos] == 0xA2) {
                    printf("A.Encontrada la cadena de bytes en la posición %zu\n", buf_pos - 4);
                }
            }
            if (buf_pos >= 4) {  // Asegúrate de que haya al menos 5 bytes en el buffer
                if (((byte*)out_buf)[buf_pos - 4] == 0x01 &&
                    ((byte*)out_buf)[buf_pos - 3] == 0x45 &&
                    ((byte*)out_buf)[buf_pos - 2] == 0xFC &&
                    ((byte*)out_buf)[buf_pos - 1] == 0xC7 &&
                    ((byte*)out_buf)[buf_pos] == 0xA2) {
                    printf("B.Encontrada la cadena de bytes en la posición %zu\n", buf_pos - 4);
                }
            }
        }
    }

    //printf("Buffer deciphered\n");
    free(message);
    return 0;
}

