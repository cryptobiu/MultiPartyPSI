//
//  clientTasks.h
//  PSI
//
//  Created by Changyu Dong on 12/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_clientTasks_h
#define PSI_clientTasks_h

#include <stdint.h>
#include "RangeHash.h"
#include "Util.h"
#include "BF.h"
#include "GarbledBF.h"
#include "RandomSource.h"
#include "AESRandom.h"
#include "OTExtRcvr.h"

typedef struct Client_inputThreadArgs{
    // socket to receive data
    int32_t socket;
    //shared output array
    uint8_t** shared;
    //size of the array
    int32_t size;
}Client_inputThreadArgs;



void* Client_inputThread (void* args);

typedef struct Client_Step2_1TaskArgs{
    EC_POINT** grs;
    OTExtRcvr* rcvr;
    EC_POINT** pk0s;
    int32_t i;
    uint8_t*** encrypted;
    int32_t thrNum;
}Client_Step2_1TaskArgs;

void* Client_Step2_1Task(void* args);

typedef struct Client_Step2_2TaskArgs{
    OTExtRcvr* rcvr;
    int32_t i;
    int32_t thrNum;
    uint8_t** toSend;
}Client_Step2_2TaskArgs;

void* Client_Step2_2Task(void* args);

typedef struct Client_Step4_1TaskArgs{
    OTExtRcvr* rcvr;
    int32_t i;
    int32_t thrNum;
}Client_Step4_1TaskArgs;

void* Client_Step4_1Task(void* args);

typedef struct Client_Step4_2TaskArgs{
    OTExtRcvr* rcvr;
    uint8_t** shared;
    BF* filter;
    int32_t i;
    int32_t thrNum;
    int32_t partial;
}Client_Step4_2TaskArgs;

void* Client_Step4_2Task(void* args);

typedef struct Client_QueryTaskArgs{
    GarbledBF* filter;
    int32_t** indexes;
    int32_t hashNum;
    uint8_t** set;
    int32_t n;
    uint8_t** eHashes;
    int32_t i;
    int32_t thrNum;
    List* list;
}Client_QueryTaskArgs;

void* Client_QueryTask(void* args);
#endif
