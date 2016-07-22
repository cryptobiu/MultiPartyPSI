//
//  AESRandom.c
//  PSI
//
//  Created by Changyu Dong on 01/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include "AESRandom.h"

int AESRandom_Create(AESRandom** rnd,uint32_t seedLen){
    assert (seedLen==10||seedLen==16||seedLen==24||seedLen==32);
    
    AESRandom *newRnd;
    
    if(!(newRnd=malloc(sizeof(AESRandom)))){
        return 0;
    }
    
    if(!(newRnd->ctx=malloc(sizeof(EVP_CIPHER_CTX))))
        return 0;
    
    EVP_CIPHER_CTX_init(newRnd->ctx);
    
    uint8_t key[EVP_MAX_KEY_LENGTH];
    //empty iv
    uint8_t iv[EVP_MAX_IV_LENGTH]={0};
    
    RAND_bytes(key, EVP_MAX_KEY_LENGTH);
    
    if(seedLen==10||seedLen==16){
        EVP_EncryptInit_ex(newRnd->ctx, EVP_aes_128_ctr(),0, key, iv);
        
    }else if(seedLen==24){
        EVP_EncryptInit_ex(newRnd->ctx, EVP_aes_192_ctr(),0, key, iv);
        
    }else if(seedLen==32){
        EVP_EncryptInit_ex(newRnd->ctx, EVP_aes_256_ctr(),0, key, iv);
        
    }
    
    *rnd=newRnd;
    return 1;
    
}

void AESRandom_Destroy(AESRandom* rnd){
    EVP_CIPHER_CTX_cleanup(rnd->ctx);
    free(rnd);
}

void AESRandom_NextBytes(AESRandom* rnd,uint8_t* out,size_t outl){
    int temp;
    EVP_EncryptUpdate(rnd->ctx, out, &temp, out, outl);
}

uint32_t AESRandom_NextInt(AESRandom* rnd){
    uint8_t i[4]={0};
    int len=4;
    EVP_EncryptUpdate(rnd->ctx, i,&len, i, len);
    return ((uint32_t*)i)[0];
}

int AESRandom_Create_With_Seed(AESRandom** rnd,uint8_t* seed, uint32_t seedLen){
    assert (seedLen==10||seedLen==16||seedLen==24||seedLen==32);
    
    AESRandom *newRnd;
    
    newRnd=malloc(sizeof(AESRandom));
    
    if(!(newRnd->ctx=malloc(sizeof(EVP_CIPHER_CTX))))
        return 0;
    
    EVP_CIPHER_CTX_init(newRnd->ctx);

    //empty iv
    uint8_t iv[EVP_MAX_IV_LENGTH]={0};

    if(seedLen==10){
        seedLen=16;
        //uint8_t* newSeed= calloc(seedLen, sizeof(uint8_t));
        uint8_t* newSeed=calloc(16, sizeof(uint8_t));
        memcpy(newSeed, seed, 10);
        
        
        EVP_EncryptInit_ex(newRnd->ctx, EVP_aes_128_ctr(),0, 0,0);
        EVP_CIPHER_CTX_set_key_length(newRnd->ctx, seedLen);
        EVP_EncryptInit_ex(newRnd->ctx, 0, 0, newSeed, iv);
    }
    
    else if(seedLen==16){
        
        //EVP_EncryptInit_ex(newRnd->ctx, EVP_aes_128_ctr(),0, seed, iv);
        EVP_EncryptInit_ex(newRnd->ctx, EVP_aes_128_ctr(),0, 0,0);
        EVP_CIPHER_CTX_set_key_length(newRnd->ctx, seedLen);
        EVP_EncryptInit_ex(newRnd->ctx, 0, 0, seed, iv);
        
    }else if(seedLen==24){
        //EVP_EncryptInit_ex(newRnd->ctx, EVP_aes_192_ctr(),0, seed, iv);
        EVP_EncryptInit_ex(newRnd->ctx, EVP_aes_192_ctr(),0, 0,0);
        EVP_CIPHER_CTX_set_key_length(newRnd->ctx, seedLen);
        EVP_EncryptInit_ex(newRnd->ctx, 0, 0, seed, iv);
        
    }else if(seedLen==32){
        //EVP_EncryptInit_ex(newRnd->ctx, EVP_aes_256_ctr(),0, seed, iv);
        EVP_EncryptInit_ex(newRnd->ctx, EVP_aes_256_ctr(),0, 0,0);
        EVP_CIPHER_CTX_set_key_length(newRnd->ctx, seedLen);
        EVP_EncryptInit_ex(newRnd->ctx, 0, 0, seed, iv);
        
    }

    
    *rnd=newRnd;
    return 1;
    
}

// k is the security level (80,128,192,256)
//get an AESRandom Instance seeded with a random k-bit seed
AESRandom* getRandomInstance(int32_t k){
    assert(k==80||k==128||k==192||k==256);
    
    AESRandom* rnd = NULL;
    AESRandom_Create(&rnd, k/8);
    return rnd;
}

// k is the security level (80,128,192,256)
// seed is the k-bit seed to initialise the random.
//get an AESRandom Instance seeded with a random k-bit seed
AESRandom* getSeededRandomInstance(uint8_t* seed, int32_t k){
    assert(k==80||k==128||k==192||k==256);
    
    AESRandom* rnd = NULL;
    AESRandom_Create_With_Seed(&rnd, seed, k/8);
    return rnd;
}


int AES_Setup(EVP_CIPHER_CTX** ctx, uint8_t* key, uint8_t* iv){
    EVP_CIPHER_CTX * newCtx;
    
    RAND_bytes(key, EVP_MAX_KEY_LENGTH);
    RAND_bytes(iv, EVP_MAX_IV_LENGTH);
    
    if(!(newCtx=malloc(sizeof(EVP_CIPHER_CTX))))
        return 0;
    
    EVP_CIPHER_CTX_init(newCtx);
    EVP_EncryptInit_ex(newCtx, EVP_aes_128_ctr(),0, key, iv);
    
    *ctx=newCtx;
    return 1;
}

int AES_Enc(EVP_CIPHER_CTX* ctx,uint8_t* out,int *outl,uint8_t* in,int inl){
    return EVP_EncryptUpdate(ctx, out, outl, in, inl);
    
}