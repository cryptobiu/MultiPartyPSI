//
//  RangeHash.h
//  PSI
//
//  Created by Changyu Dong on 24/02/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//
#include <openssl/sha.h>
#include <openssl/bn.h>
#include "byteswap.h"



#ifndef PSI_RangeHash_h
#define PSI_RangeHash_h
#define RangeHash_BuffLen 5
#define RangeHash_DataBufLen 64

typedef struct RangeHash{
    uint8_t *key;
    int32_t keyLen;
    uint32_t Buf[RangeHash_BuffLen];
    uint8_t dataBuf[RangeHash_DataBufLen];

    SHA_CTX * ctx;
    //modulus
    int32_t m;
}RangeHash;
/*
 Create a RangeHash instance. The hash function is keyed with key and output an integer in[0,m-1].
 */
int RangeHash_Create(RangeHash** hash, uint8_t *key, int32_t keyLen, int32_t m);


void RangeHash_Destroy(RangeHash *hash);

int32_t RangeHash_Digest(RangeHash *H, uint8_t *data, int32_t dataLen);

#endif
