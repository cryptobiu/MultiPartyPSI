//
//  BF.c
//  PSI
//
//  Created by Changyu Dong on 24/02/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//



#include "BF.h"

extern void printByte(uint8_t b);

int BF_GenerateParameters(BFParameters** param, int32_t n, int32_t p){
    BFParameters* newParam;
    if(!(newParam=malloc(sizeof(BFParameters)))){
        return 0;
    };
    
    newParam->n=n;
    newParam->k=p;
    newParam->m=ceil(n*p*log2e);
    if (newParam->m%8!=0)
        newParam->m=ceil(newParam->m/8.0)*8;

    *param=newParam;
    return 1;
}

void BF_Param_Destroy(BFParameters* param){
    free(param);
}

int BF_Create(BF** filter, int32_t m){
    //calculate the byte length of the bloom filter
    int r= m%8;
    int leadingZeroes;
    int byteLen;
    if(r==0){
        byteLen = m/8;
        leadingZeroes=0;
    }else{
        byteLen=m/8+1;
        leadingZeroes=8-r;
    }
    
    BF* bf;
    
    bf=malloc(sizeof(BF));
    
    if(bf==NULL){
        return 0;
    }
    
    bf->byteLen=byteLen;
    bf->leadingZeroes=leadingZeroes;
    bf->m=m;
    
    bf->data=(uint8_t*)calloc(byteLen, sizeof(uint8_t));
    *filter=bf;
    if(bf->data==NULL){
        return 0;
    }
    else{
        return 1;
    }
}

void BF_Destroy(BF* filter){
    free(filter->data);
    free(filter);
}




void BF_Add_With_Output(BF* filter, uint8_t* item, int32_t itemLen, RangeHash** hashFuncs, int32_t k, int32_t* idxOut){
    int32_t idx;
    for(int i=0;i<k;i++){
        idx=RangeHash_Digest(hashFuncs[i], item, itemLen);
        BF_SetBit(filter, idx);
        idxOut[i]=idx;
    }
}

void BF_Add(BF* filter, uint8_t* item, int32_t itemLen, RangeHash** hashFuncs, int32_t k){
    int32_t idx;
    for(int i=0;i<k;i++){
        idx=RangeHash_Digest(hashFuncs[i], item, itemLen);
        BF_SetBit(filter, idx);
    }
}

void BF_AddMT(BF* filter, int32_t* idxes, int32_t k){
    for(int32_t i=0;i<k;i++){
        //printf("%d\n",idxes[i]);
        if(idxes[i]!=-1)
            BF_SetBit(filter,idxes[i]);
    }
}

int BF_Query(BF* filter,uint8_t* item, int32_t itemLen,RangeHash** hashFuncs, int32_t k){
    int32_t idx;
    for(int32_t i=0;i<k;i++){
        idx=RangeHash_Digest(hashFuncs[i], item, itemLen);
        if(BF_GetBit(filter, idx)==0)
            return 0;
    }
    return 1;
}

void BF_Print(BF* filter){
    for(int i=0;i<filter->byteLen;i++){
        printByte(filter->data[i]);
        printf(" ");
    }
    printf("\n");
}

