//
//  BuilderTasks.c
//  PSI
//
//  Created by Changyu Dong on 05/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include "BuilderTasks.h"

void* BFProducer(void* args){
    BFProducerArgs* myArgs= (BFProducerArgs*)args;
    RangeHash** hashes;
    hashes=malloc(sizeof(RangeHash*)*myArgs->hashNum);
    
    //initialise rangeHashes
    for(uint32_t i=0;i<myArgs->hashNum;i++){
        RangeHash_Create(&(hashes[i]), myArgs->hashKeys[i], myArgs->keyLen, myArgs->m);
    }
    
    //start from i up to the end
    
    for(uint32_t i=myArgs->i;i<myArgs->max;i+=myArgs->threadNum){
        
        int32_t* result;
        result=malloc(sizeof(int32_t)*myArgs->hashNum);
        assert(result!=NULL);
        //produce indexes
        for(uint32_t j=0;j<myArgs->hashNum;j++){
            result[j]=RangeHash_Digest(hashes[j], myArgs->inputs[i], myArgs->dataLen);
            if(exists(result[j], result, j)){
                result[j]=-1;
            }
        }
        myArgs->out[i]=result;
        
        BF_AddMT(myArgs->filter, result, myArgs->hashNum);
    }
    
    for(uint32_t i=0;i<myArgs->hashNum;i++){
        RangeHash_Destroy(hashes[i]);
    }
    free(hashes);
    free(myArgs);
    return NULL;
}

void* GBFIndexProducer(void* args){
    GBFIdxProducerArgs* myArgs= (GBFIdxProducerArgs*)args;
    RangeHash** hashes;
    hashes=malloc(sizeof(RangeHash*)*myArgs->hashNum);
    
    //initialise rangeHashes
    for(uint32_t i=0;i<myArgs->hashNum;i++){
        RangeHash_Create(&(hashes[i]), myArgs->hashKeys[i], myArgs->keyLen, myArgs->m);
    }
    
    //start from i up to the end
    
    for(uint32_t i=myArgs->i;i<myArgs->max;i+=myArgs->threadNum){
        
        int32_t* result;
        result=malloc(sizeof(int32_t)*myArgs->hashNum);
        assert(result!=NULL);
        //produce indexes
        for(uint32_t j=0;j<myArgs->hashNum;j++){
            result[j]=RangeHash_Digest(hashes[j], myArgs->inputs[i], myArgs->dataLen);
            if(exists(result[j], result, j)){
                result[j]=-1;
            }
        }
        myArgs->out[i]=result;
    }
    
    for(uint32_t i=0;i<myArgs->hashNum;i++){
        RangeHash_Destroy(hashes[i]);
    }
    free(hashes);
    free(myArgs);
    return NULL;
}

void* GBFRndProducer(void* args){
    rndProducerArgs* myArgs= (rndProducerArgs*) args;
    //initialise an AES random
    AESRandom * rnd;
    AESRandom_Create(&rnd, myArgs->seedLen);
    
    assert(rnd!=NULL);
    
    for(int i=myArgs->i;i<myArgs->rnd->numRows;i+=myArgs->threadNum){
        uint8_t* rowBytes;
        rowBytes = calloc(myArgs->rowLen, sizeof(uint8_t));
        AESRandom_NextBytes(rnd, rowBytes, myArgs->rowLen);
        RandomSource_Insert_Bytes(myArgs->rnd, rowBytes, myArgs->rowLen, i);
    }
    
    AESRandom_Destroy(rnd);
    free(myArgs);
    return NULL;
}