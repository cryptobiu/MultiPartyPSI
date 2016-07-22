//
//  AESRandom.h
//  PSI
//
//  Created by Changyu Dong on 01/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_AESRandom_h
#define PSI_AESRandom_h
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdint.h>
#include <string.h>

typedef struct AESRandom{
    uint32_t seedLen;
    EVP_CIPHER_CTX* ctx;
}AESRandom;
//create an AES random instance given the length of the seed. Will seed the random with a random seed. The length of the seed is should be 10, 16, 24,32 bytes, corresponding to 80,128,192,256 bit security.
int AESRandom_Create(AESRandom** rnd,uint32_t seedLen);

int AESRandom_Create_With_Seed(AESRandom** rnd,uint8_t* seed, uint32_t seedLen);

void AESRandom_Destroy(AESRandom* rnd);
// generates outl random bytes and stored in the buffer out. If out contains data, it will be xored with newly generated random bytes;

void AESRandom_NextBytes(AESRandom* rnd,uint8_t* out, size_t outl);

uint32_t AESRandom_NextInt(AESRandom* rnd);

AESRandom* getRandomInstance(int32_t k);

AESRandom* getSeededRandomInstance(uint8_t* seed, int32_t k);

int AES_Setup(EVP_CIPHER_CTX** ctx, uint8_t* key, uint8_t* iv);

int AES_Enc(EVP_CIPHER_CTX* ctx,uint8_t* out,int *outl,uint8_t* in,int inl);

#endif
