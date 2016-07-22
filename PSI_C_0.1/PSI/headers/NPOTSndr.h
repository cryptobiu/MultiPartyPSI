//
//  NPOTSndr.h
//  PSI
//
//  Created by Changyu Dong on 06/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_NPOTSndr_h
#define PSI_NPOTSndr_h
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>


#include "MessageDigest.h"
#include "Util.h"

typedef struct NPOTSndr{
    EC_GROUP* curve;
    EC_POINT* g;
    BIGNUM* order;
    //BN_CTX* ctx;
    //int32_t bufSize;
    uint8_t pointByteLen;
    
    MessageDigest* md;

    //length k arrays
    BIGNUM** rs;
    EC_POINT** cs;
    EC_POINT** crs;
    EC_POINT** grs;
    
    //how many pairs to send
    int32_t k;
    uint8_t*** toSend;
    //the length of each string to send
    int32_t sendByteLen;
    
}NPOTSndr;
/*
 k: must be the same as the receiver's strBitLen
 sendLen: byte length of the strings to send
 toSend: k pairs of strings to send
 secLev: security level, must be 80,128,192,or 256
 */
int NPOTSndr_Create(NPOTSndr** sndr, int32_t k, int32_t sendByteLen, uint8_t*** toSend, int32_t secLev);

void NPOTSndr_Destroy(NPOTSndr* sndr);

inline EC_POINT** NPOTSndr_getCs(NPOTSndr* sndr){
    return sndr->cs;
}
/*
 encrypt toSend and also return k points (grs) to be send to the receiver.
 */
EC_POINT** NPOTSndr_Step(NPOTSndr* sndr,EC_POINT** PK0s, uint8_t*** encrypted);

EC_POINT* NPOTSndr_StepSingle(NPOTSndr* sndr,EC_POINT* PK0,int32_t i, uint8_t*** encrypted, MessageDigest* H);
#endif
