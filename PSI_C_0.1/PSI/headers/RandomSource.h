//
//  RandomSource.h
//  PSI
//
//  Created by Changyu Dong on 04/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_RandomSource_h
#define PSI_RandomSource_h
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct row{
    uint8_t* bytes;
    int32_t length;
}row;

/*
 This is a data structure to store precomputed random bytes.
 To be used in multi-threading tasks.
 */
typedef struct RandomSource{
    row ** rndBytes;
    int32_t index;
	int32_t start;
	int32_t blockSize;
    int32_t numRows;
    
}RandomSource;


int RandomSource_Create(RandomSource** rndSrc, int32_t numRows, int32_t Blocksize);

void RandomSource_Destroy(RandomSource* rnd);

/*
 Insert a row into the random source. The row contains a newly generated random bytes, the length should be a multiple of blockSize.
 */
inline void RandomSource_Insert(RandomSource* rndSrc, row* rowData, int32_t i){
    rndSrc->rndBytes[i]=rowData;
};

inline void RandomSource_Insert_Bytes(RandomSource* rndSrc, uint8_t* rowData, int32_t rowLen, int32_t i){
    rndSrc->rndBytes[i]=malloc(sizeof(row));
    rndSrc->rndBytes[i]->bytes= rowData;
    rndSrc->rndBytes[i]->length=rowLen;
};

/*
 Return a pointer pointing to a random byte string of blockSize bytes somewhere in the random source.
  Don't free the random source before finish using the randoms.
 */
inline uint8_t* RandomSource_Take(RandomSource* rndSrc){
    while(rndSrc->rndBytes[rndSrc->index]==NULL){
        //waiting
        usleep(1);
    }
    
    uint8_t* block=rndSrc->rndBytes[rndSrc->index]->bytes+rndSrc->start;
    
    rndSrc->start+=rndSrc->blockSize;
    
    if(rndSrc->start>=rndSrc->rndBytes[rndSrc->index]->length){
        rndSrc->start=0;
        rndSrc->index+=1;
    };
    return block;
};

#endif
