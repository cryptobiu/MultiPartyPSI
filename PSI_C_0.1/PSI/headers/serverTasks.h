//
//  serverTasks.h
//  PSI
//
//  Created by Changyu Dong on 13/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_serverTasks_h
#define PSI_serverTasks_h

#include <stdint.h>
#include <stdlib.h>
#include "Util.h"
#include "GarbledBF.h"
#include "BF.h"
#include "OTExtSndr.h"
#include "MTBuilder.h"


typedef struct Server_outputThreadArgs{
    // socket to receive data
    int32_t socket;
    //shared output array
    uint8_t** shared;
    //size of the array
    int32_t size;
}Server_outputThreadArgs;

void* Server_outputThread(void* args);

typedef struct Server_step1TaskArgs{
    OTExtSndr* sndr;
    EC_POINT** cs;
    int32_t i;
    EC_POINT** pks;
    int32_t trdNum;
}Server_step1TaskArgs;

void* Server_step1Task(void* args);

typedef struct Server_BuildGBFArgs{
    GarbledBF* filter;
    BFParameters* bfParam;
    uint8_t** keys;
    int32_t k;
    int32_t keyByteLen;
    uint8_t** set;
    int32_t eByteLen;
    int32_t eNum;
    uint8_t** ehash;
    int32_t rndSeedLen;
}Server_BuildGBFArgs;

void* Server_BuildGBF(void * args);

typedef struct Server_step3_1TaskArgs{
    OTExtSndr* sndr;
    uint8_t*** received;
    EC_POINT** grs;
    int32_t i;
    uint8_t** result;
    int32_t trdNum;
}Server_step3_1TaskArgs;

void* Server_step3_1Task(void* args);

typedef struct Server_step3_2TaskArgs{
    OTExtSndr* sndr;
    uint8_t** input;
    int32_t i;
    int32_t trdNum;
    BitMatrix* out;
}Server_step3_2TaskArgs;

void* Server_step3_2Task(void* args);

typedef struct Server_step3_3TaskArgs{
    OTExtSndr* sndr;
    BitMatrix* Q;
    int32_t i;
    uint8_t** shared;
    int32_t trdNum;
}Server_step3_3TaskArgs;

void* Server_step3_3Task(void* args);
#endif
