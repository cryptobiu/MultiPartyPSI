//
//  BuilderTasks.h
//  PSI
//
//  Created by Changyu Dong on 05/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_BuilderTasks_h
#define PSI_BuilderTasks_h
#include <stdint.h>
#include "RangeHash.h"
#include "Util.h"
#include "BF.h"
#include "RandomSource.h"
#include "AESRandom.h"



//The input argument

typedef struct BFProducerArgs{
    //bloom filter to produce
    BF* filter;
    
    //input data
    uint8_t** inputs;
    int32_t dataLen;
    
    //output indexes
    int32_t** out;
    
    // id of the thread, from 0 to threadNum-1
    int32_t i;
    //max number of tasks
    int32_t max;
    int32_t threadNum;

    //data to initialise range hash
    uint8_t** hashKeys;
    int32_t m;
    int32_t keyLen;
    int32_t hashNum;

} BFProducerArgs;


typedef struct GBFIdxProducerArgs{
    //input data
    uint8_t** inputs;
    int32_t dataLen;
    
    //output indexes
    int32_t** out;
    
    // id of the thread, from 0 to threadNum-1
    int32_t i;
    //max number of tasks
    int32_t max;
    int32_t threadNum;
    
    //data to initialise range hash
    uint8_t** hashKeys;
    int32_t m;
    int32_t keyLen;
    int32_t hashNum;
    
} GBFIdxProducerArgs;

typedef struct rndProducerArgs{
    RandomSource* rnd;
    int32_t seedLen;
    int32_t i;
    int32_t threadNum;
    int32_t rowLen;
}rndProducerArgs;

void* BFProducer(void* args);

void* GBFIndexProducer(void* args);

void* GBFRndProducer(void* args);

#endif
