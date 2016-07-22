//
//  BF.h
//  PSI
//
//  Created by Changyu Dong on 24/02/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_BF_h
#define PSI_BF_h

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "RangeHash.h"

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
(byte & 0x80 ? 1 : 0), \
(byte & 0x40 ? 1 : 0), \
(byte & 0x20 ? 1 : 0), \
(byte & 0x10 ? 1 : 0), \
(byte & 0x08 ? 1 : 0), \
(byte & 0x04 ? 1 : 0), \
(byte & 0x02 ? 1 : 0), \
(byte & 0x01 ? 1 : 0)

#define log2e 1.4428
inline void printByte(uint8_t b){
    printf(BYTETOBINARYPATTERN,BYTETOBINARY(b));
}

typedef struct BF{
    uint8_t *data;
    int32_t m;
    int32_t leadingZeroes;
    int32_t byteLen;
}BF;

//parameters of a (garbled) bloom filter. m is the size of the filter, n is the maximum number of items, k is the number of hash functions 
typedef struct BFParameters{
    int32_t m;
    int32_t n;
    int32_t k;
} BFParameters;

//generate bloom filter parameters given the max number of items and the upper bound of the false positive rate
int BF_GenerateParameters(BFParameters ** param, int32_t n, int32_t p);

void BF_Param_Destroy(BFParameters* param);

// create an m-bit empty bloom filter
int BF_Create(BF** filter, int32_t m);

//destroy the bloom filter and release the resource;
void BF_Destroy(BF* filter);

//set the bit at idx to 1
inline void BF_SetBit(BF* filter, int32_t idx){
    assert(idx<filter->m&& idx>=0);
    
    
    unsigned int intLoc = ((idx+filter->leadingZeroes)>>3);
    filter->data[intLoc] |= (1 << (7-((idx+filter->leadingZeroes) & 7)));
};

//get the bit at idx

inline int BF_GetBit(BF* filter, int32_t idx){
    assert(idx<filter->m && idx>=0);
    
    return (filter->data[(idx+filter->leadingZeroes) >> 3] & (1 << (7-((idx+filter->leadingZeroes) & 7))))==0?0:1;
};

//add an item to the bloom filter. The element is hashed by k different hash functions into different indexes in the bloomfilter; The indexes are outputed.

void BF_Add_With_Output(BF* filter, uint8_t* item, int32_t itemLen, RangeHash** hashFuncs, int32_t k, int32_t* idxOut);

void BF_Add(BF* filter, uint8_t* item, int32_t itemLen, RangeHash** hashFuncs, int32_t k);

//add an item to the bloom filter given precomputed indexes
void BF_AddMT(BF* filter, int32_t* idxes, int32_t k);

//query an element, 0 means not in, 1 means in;
int BF_Query(BF* filter,uint8_t* item, int32_t itemLen,RangeHash** hashFuncs, int32_t k);

void BF_Print(BF* filter);

#endif
