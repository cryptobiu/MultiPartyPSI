//
//  OTExtSndr.h
//  PSI
//
//  Created by Changyu Dong on 07/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_OTExtSndr_h
#define PSI_OTExtSndr_h
#include "NPOTRcvr.h"
#include "BitMatrix.h"

typedef struct OTExtSndr{
    int32_t k;
    int32_t m;
    
    // m strings to send, each is l-bit
    uint8_t** toSend;
    //the byte length of toSend strings
    int32_t lByteLength;
    
    
    AESRandom** seededRnd;
    //a selection string for the base receiver -- k bits
    uint8_t* s;
    int32_t sLeadingZeroes;
    int32_t sByteLen;
    
    int32_t seedByteLen;

    int32_t secLev;
    
    NPOTRcvr* baseOTRcvr;
    
    MessageDigest* H;
    
    //temp buf to be used in single threaded mode
    uint8_t* row;
    uint8_t* hashTemp;
    uint8_t* allZeroes;

}OTExtSndr;

int OTExtSndr_Create(OTExtSndr** sndr, int32_t m, int32_t secLev, uint8_t** toSend, int32_t lByteLen);

void OTExtSndr_Destroy(OTExtSndr* sndr);

void OTExtSndr_ReInitialise(OTExtSndr* sndr, uint8_t** toSend, int32_t m);

//step 1: act as the receiver of the underlying OT. Generate k pk0s
//output to be send to the other party
inline EC_POINT** OTExtSndr_step1(OTExtSndr* sndr,EC_POINT** cs,int32_t csLen){
   return NPOTRcvr_RStep1(sndr->baseOTRcvr, cs, csLen);
};

inline void OTExtSndr_step1Single(OTExtSndr* sndr,EC_POINT* c,int32_t i,EC_POINT** pks){
    NPOTRcvr_RStep1Single(sndr->baseOTRcvr, c, i, pks);
};

//step3_1: act as the receiver of the underlying OT. receives k strings
//each string is a seed
uint8_t** OTExtSndr_step3_1(OTExtSndr* sndr, uint8_t*** received, int32_t recStrLen,EC_POINT** grs);

void OTExtSndr_step3_1Single(OTExtSndr* sndr, uint8_t** received, int32_t recStrLen,EC_POINT* grs,int32_t i,MessageDigest* H, uint8_t** out);

/**
 * 	step3_2: get a matrix
 *input is m l-bit strings.
 *if the ith bit in s is 0, then use the ith seed to generate a m bit string and set it as the ith column of the matrix
 *if the ith bit in s is 1, then use the ith seed to generate a m bit string, xor the ith input string, then set the
 *result as the ith column of the matrix.
 * @param input the k bit strings sent in step 2_2
 * @param seed  the k seeds received in step 3_1
 * @return the bit matrix Q received by the sender
 */

BitMatrix* OTExtSndr_step3_2(OTExtSndr* sndr,uint8_t** input);

void OTExtSndr_step3_2Single(OTExtSndr* sndr,uint8_t* input, int32_t i, BitMatrix* out);

/**
 * Step 3_3: read each row qj of the matrix, then compute toSend[j] XOR H(j,qj XOR s)
 * output to be sent to the other party.
 */

uint8_t** OTExtSndr_step3_3(OTExtSndr* sndr,BitMatrix* Q);

//single threaded version
uint8_t* OTExtSndr_step3_3Single(OTExtSndr* sndr,BitMatrix* Q, int32_t j,MessageDigest* md);

//multi threaded version
uint8_t* OTExtSndr_step3_3SingleMT(OTExtSndr* sndr,BitMatrix* Q, int32_t j,MessageDigest* md,uint8_t* rowBuf, uint8_t* hashBuf);

#endif
