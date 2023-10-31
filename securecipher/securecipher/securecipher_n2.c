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

int cipher(byte** out_buf, byte* in_buf, DWORD size, struct KeyData* key) {    

    //Variables de cifrado
    byte* message = (byte*)malloc(20 * sizeof(byte));
    size_t buf_pos = 0; //posicion en el bufer de cifrado
    size_t out_pos = 0;
    size_t in_pos = 0;
    byte last_byte = 0xFF;
    byte nal_buffer[5] = { 0x01, 0x45, 0xFC, 0xC7, 0xA2 }; //solo si uso memcmp o memcpy
    int pos_nal = size/2;
    int tam_nal = 0;
    byte* nal = NULL;
    int nal_mode = 0;
    if (key->next_key != NULL) { nal_mode = 1; }

    //Preparo el buffer de salida con hueco para el nal si es necesario
    if (nal_mode == 1) {
        tam_nal = key->next_key->size + 7; // 5 bytes del nal, 1 del modo del nal, 1 para decir el tam, mas la nueva clave
        //printf("El tam del nal es: %d\n", tam_nal);
        *out_buf = (byte*)malloc(size + tam_nal);
        //Creo y relleno el NAL
        nal = malloc(tam_nal);
        for (int i = 0; i < 5; i++) {
            nal[i] = nal_buffer[i];
        }
        nal[5] = 0x01;
        nal[6] = (byte)key->next_key->size;
        for (int i = 0; i < key->next_key->size; i++) {
            nal[i + 7] = key->next_key->data[i];
        }

    }
    else { *out_buf = (byte*)malloc(size); }
    
    for (buf_pos; buf_pos < size; buf_pos++) {
        //printf("Empezando vuelta bucle %d, %d, %d\n", out_pos, in_pos, buf_pos);
        if (buf_pos == pos_nal) {
            if (nal_mode == 1) {
                //introduzco la cadena del nal y la cifro
                for (int i = 0; i < tam_nal; i++) {
                    //printf("out_pos= %d\n", out_pos);
                    memcpy(message, get_message(last_byte, key), 20);
                    //Hago la transformacion lineal y actualizo el message
                    message = lineal_transform(message);
                    //Confusion
                    byte resultado = confusion(message);
                    (*out_buf)[out_pos] = ((nal)[i] ^ resultado) % 256;
                    //(*out_buf)[out_pos] = (nal_buffer)[i];
                    last_byte = (nal)[i];
                    out_pos++;
                    printf("(%02x) %c  ", (*out_buf)[out_pos-1], nal[i]);
                }

                //printf("Secuencia introducida\n");
                //in_pos++;
                //printf("\nAsi quedan las posiciones, in: %d, out:%d, buf:%d\n",in_pos,out_pos,buf_pos);
                //Cambiar la clave
                //printf("Clave previa al NAL %.*s\n", key->size, key->data);
                key->size = key->next_key->size;
                key->data = key->next_key->data;
                //printf("Clave despues del NAL %.*s\n", key->size, key->data);
                printf("\n");
                continue;
            }
        }
        //message = get_message(last_byte, key);
        memcpy(message, get_message(last_byte, key), 20);
        //Hago la transformacion lineal y actualizo el message
        message = lineal_transform(message);
        //Confusion
        byte resultado = confusion(message);
        //((byte*)out_buf)[out_pos] = (((byte*)in_buf)[in_pos] ^ resultado) % 256;
        //(*out_buf)[out_pos] = (((byte*)in_buf)[in_pos] ^ resultado) % 256;
        (*out_buf)[out_pos] = (((byte*)in_buf)[in_pos] ^ resultado) % 256;
        last_byte = ((byte*)in_buf)[in_pos];
        out_pos++;
        in_pos++;
    }
    //printf("Buffer ciphered\n");
    free(message);
    
    return out_pos;
}

int decipher(byte** out_buf, byte* in_buf, DWORD size, struct KeyData* key) {

    *out_buf = (byte*)malloc(size);
    byte* message = (byte*)malloc(20 * sizeof(byte));
    size_t buf_pos = 0; //posicion en el bufer, solo valida para escribir en el bufer, para cifrar se usa la posicion real en el fichero
    byte last_byte = 0xFF;
    byte pattern[5] = { 0x01, 0x45, 0xFC, 0xC7, 0xA2 };
    size_t in_pos=0;
    size_t out_pos=0;
    int nal_mode = 0;
    int tam_nal = 0;
    byte* nal = NULL;

    for (buf_pos; buf_pos < size; buf_pos++) {
        memcpy(message, get_message(last_byte, key), 20);
        //Hago la transformacion lineal y actualizo el message
        message = lineal_transform(message);
        //Confusion
        byte resultado = confusion(message);
        //((byte*)out_buf)[buf_pos] = (((byte*)in_buf)[buf_pos] ^ resultado) % 256;
        (*out_buf)[out_pos] = ((in_buf)[in_pos] ^ resultado) % 256;
        last_byte = (*out_buf)[out_pos];
        //Añado el calculo del nal en modo cambio de clave
        if (out_pos >= 5) {  // Asegúrate de que haya al menos 5 bytes en el buffer
            if ((*out_buf)[out_pos - 5] == 0x01 &&
                (*out_buf)[out_pos - 4] == 0x45 &&
                (*out_buf)[out_pos - 3] == 0xFC &&
                (*out_buf)[out_pos - 2] == 0xC7 &&
                (*out_buf)[out_pos - 1] == 0xA2 &&
                (*out_buf)[out_pos] == 0x01) {
                printf("Outbuffer.Encontrada la cadena de bytes en la posición %zu\n", out_pos - 5);
                out_pos = out_pos - 5; //TODO descomentar esto despues, es MUY importante
                in_pos++;
                //Descifrar el tam y la clave
                int tam_new_key = 0;
                memcpy(message, get_message(last_byte, key), 20);
                message = lineal_transform(message);
                resultado = confusion(message);
                tam_new_key = (int)((in_buf)[in_pos] ^ resultado) % 256;
                last_byte = tam_new_key;
                in_pos++;
                printf("El size de la nueva clave es %d\n", tam_new_key);
                byte* new_key = malloc(tam_new_key);
                for (int i = 0; i < tam_new_key; i++) {
                    memcpy(message, get_message(last_byte, key), 20);
                    message = lineal_transform(message);
                    resultado = confusion(message);
                    printf("(%02x) ", (in_buf)[in_pos]);
                    new_key[i] = ((in_buf)[in_pos] ^ resultado) % 256;
                    last_byte = new_key[i];
                    printf("%c ", last_byte);
                    in_pos++;
                }
                //Reemplazar clave y machacar el fichero de key
                key->size = tam_new_key;
                key->data = new_key;
                FILE* key_file = fopen(key->keyfile, "w");
                fwrite(new_key,1,tam_new_key,key_file);
                fclose(key_file);
                printf("\n");
                continue;
            }
        }
        out_pos++;
        in_pos++;
    }

    //printf("Buffer deciphered\n");
    free(message);
    return out_pos;
}

