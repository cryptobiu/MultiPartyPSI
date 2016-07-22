//
//  NPOTRcver.h
//  PSI
//
//  Created by Changyu Dong on 06/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_NPOTRcvr_h
#define PSI_NPOTRcvr_h
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <assert.h>
#include <stdint.h>

#include "AESRandom.h"
#include "MessageDigest.h"
#include "Util.h"

typedef struct NPOTRcvr{
    EC_GROUP* curve;
    //BN_CTX* ctx;
    BIGNUM* order;
    EC_POINT* g;
    uint8_t pointByteLen;
    
    uint8_t* str;
    int32_t strByteLen;
    int32_t strBitLen;
    int32_t leadingZeroes;
    
    //int32_t bufSize;
    
    AESRandom* rnd;
    MessageDigest* md;
    
    BIGNUM** ks;
}NPOTRcvr;

/*
 str: the selectin string;
 strLen: the byte length of str;
 LeadingZeroes: how many leadingZeroes in str
 seclev: security level, must be 80,128,192,or 256. 
 */

int NPOTRcvr_Create(NPOTRcvr** rcvr, uint8_t* str, int32_t strLen,int32_t LeadingZeroes, int32_t secLev);

void NPOTRcvr_Destroy(NPOTRcvr* rcvr);

EC_POINT** NPOTRcvr_RStep1(NPOTRcvr* rcvr, EC_POINT** Cs, int32_t CsLen);

void NPOTRcvr_RStep1Single(NPOTRcvr* rcvr, EC_POINT* C, int32_t i, EC_POINT** PK);

uint8_t** NPOTRcvr_RStep2(NPOTRcvr* rcvr, uint8_t*** received,int32_t recLen, EC_POINT** grs);

void NPOTRcvr_RStep2Single(NPOTRcvr* rcvr, uint8_t** received,int32_t recLen,EC_POINT* gr, int32_t i, MessageDigest* H, uint8_t** result);

#endif
