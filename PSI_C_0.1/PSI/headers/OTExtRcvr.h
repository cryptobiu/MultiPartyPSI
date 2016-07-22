//
//  OTExtRcvr.h
//  PSI
//
//  Created by Changyu Dong on 07/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_OTExtRcvr_h
#define PSI_OTExtRcvr_h
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#include "NPOTSndr.h"
#include "BitMatrix.h"
#include "MessageDigest.h"
#include "AESRandom.h"




typedef struct OTExtRcvr{
    // security parameter
    int32_t k;
    //how many strings to receiver -- the bit length of seleciton string
    int32_t m;
    //the selectin string
    uint8_t* str;
    int32_t strByteLen;
    int32_t leadingZeroes;
    
    BitMatrix* T;
    MessageDigest* md;
    
    //seeds to send -- by its NPOTSndr
    uint8_t*** seeds;
    //randoms seeded with the above seeds
    AESRandom*** seededRnd;
    //byte Length of the seeds -- depends on the security level (1/8)
    int32_t seedByteLen;
    
    int32_t secLev;
    
    NPOTSndr* baseOTSndr;
    
    uint8_t* row;
    uint8_t* hashTemp;
    //uint8_t* column;
    uint8_t* allZero;
    
    
    
}OTExtRcvr;
/*
 str is the selection string (can be NULL and then set before the start), m is the bitLength of the selection string, secLev is the security level (80,128,192 or 256);
 */
int OTExtRcvr_Create(OTExtRcvr** rcvr,uint8_t* str, int32_t m, int32_t secLev);

void OTExtRcvr_Destroy(OTExtRcvr* rcvr);

inline void OTExTRcvr_SetStr(OTExtRcvr* rcvr, uint8_t* str, int32_t len){
    assert(len==rcvr->strByteLen);
    rcvr->str=str;
};

/**
 * If the matrix needs to be sends in parts, use this function to reinitialise the matrix and the selection string.
 * m=strByteLen*8
 */

int OTExtRcvr_ReInitialise(OTExtRcvr* rcvr, int32_t m, uint8_t* str, int32_t strByteLen);
/*
 multithreaded
 */
int OTExtRcvr_ReInitialiseMT(OTExtRcvr* rcvr, int32_t m, uint8_t* str, int32_t strByteLen);



/*
 * Matrix encryption/decryption
 */

static void OTExtRcvr_MatrixEnc(OTExtRcvr* rcvr);

static inline void OTExtRcvr_MatrixEncSingle(OTExtRcvr* rcvr, int32_t i){
    AESRandom_NextBytes(rcvr->seededRnd[i][1], rcvr->T->data[i], rcvr->strByteLen);
    xorByteArray(rcvr->T->data[i], rcvr->str, rcvr->strByteLen);
};

void OTExtRcvr_MatrixDec(OTExtRcvr* rcvr);

//rather than xor, regenerate the matrix using the same seeds

inline void OTExtRcvr_MatrixDecSingle(OTExtRcvr* rcvr,int32_t i){
    memset(rcvr->T->data[i], 0, rcvr->strByteLen);
    AESRandom_NextBytes(rcvr->seededRnd[i][2], rcvr->T->data[i], rcvr->strByteLen);
};

/**
 * Step 0: receiver sends k group elements to the sender;
 */
inline EC_POINT** OTExtRcvr_step0(OTExtRcvr* rcvr){
    return NPOTSndr_getCs(rcvr->baseOTSndr);
};

/**
 * Step 2_1: to encrypt the seeds which to be received by the other party using received pks
 *
 * @param PK0s received pks
 * @param encrypted 2*k encrypted seeds to be sent to the other party. It is a new byte[k][2][].
 * @return k group elements g^r to be sent to the other party
 */

inline EC_POINT** OTExtRcvr_step2_1(OTExtRcvr* rcvr,EC_POINT** pk0s,uint8_t*** encrypted){
    return NPOTSndr_Step(rcvr->baseOTSndr, pk0s, encrypted);
};

inline EC_POINT* OTExtRcvr_step2_1Single(OTExtRcvr* rcvr,EC_POINT* pk0,int32_t i,uint8_t*** encrypted,MessageDigest* H){
    return NPOTSndr_StepSingle(rcvr->baseOTSndr, pk0, i, encrypted, H);
};

/**
 * Step 2_2: in addition, send a encrypted bit matrix
 * @return content of the bit matrix;
 */

uint8_t** OTExtRcvr_step2_2(OTExtRcvr* rcvr);
    
uint8_t* OTExtRcvr_step2_2Single(OTExtRcvr* rcvr,int32_t i);

/**
 * Decrypted the received byte[][], result is stored in the same array
 * @param received
 */

inline void OTExtRcvr_step4_1(OTExtRcvr* rcvr){
    OTExtRcvr_MatrixDec(rcvr);
};

inline void OTExtRcvr_step4_1Single(OTExtRcvr* rcvr,int32_t i){
    OTExtRcvr_MatrixDecSingle(rcvr, i);
};

/**
 *  Decrypted the received byte[][], result is stored in the same array
 * received: the recieved byte[][]
 * recLen: byte length of strings in received
 */
void OTExtRcvr_step4_2(OTExtRcvr* rcvr,uint8_t** received,int32_t recStrLen);
/*
 Single threaded version
 */
void OTExtRcvr_step4_2Single(OTExtRcvr* rcvr,uint8_t* received, int32_t recLen, int32_t i, MessageDigest* H);
/*
 Multi threaded version, do not use internal buffer of rcvr.
 */
void OTExtRcvr_step4_2SingleMT(OTExtRcvr* rcvr,uint8_t* received, int32_t recLen, int32_t i, MessageDigest* H, uint8_t* rowBuf, uint8_t* hashBuf);

#endif
