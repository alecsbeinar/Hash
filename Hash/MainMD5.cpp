#include "MainMD5.h"
#include "md5.h"
#include <iostream>
#pragma warning(disable : 4996)
#include <stdio.h>
using namespace std;

void MD5(char** szOutput, char** pData)
{
	md5_state_t state;
	md5_byte_t digest[16];
	md5_init(&state);
	md5_append(&state, (const md5_byte_t*)(*pData), strlen(*pData));
	md5_finish(&state, digest);
	for (int i = 0; i < 16; ++i)
	{
		sprintf(*szOutput + i * 2, "%02x", digest[i]);
	}
}