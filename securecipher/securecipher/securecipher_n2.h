#include <stdio.h>
#include <malloc.h>
#include <Windows.h>
#include <time.h>
#include <stdint.h>

typedef unsigned char byte;

struct KeyData {
	byte* data;
	int size;
	struct KeyData* next_key;
};

byte* get_message(byte last_byte, struct KeyData* key);
byte* lineal_transform(byte* message);
byte confusion(byte* message);
int cipher(LPVOID out_buf, LPCVOID in_buf, DWORD size, struct KeyData* key, int nal_mode);
int decipher(LPVOID out_buf, LPCVOID in_buf, DWORD size, struct KeyData* key, int nal_mode);