//
//  serverTasks.c
//  PSI
//
//  Created by Changyu Dong on 13/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include "serverTasks.h"

void* Server_outputThread(void* args){
    Server_outputThreadArgs* myArgs= (Server_outputThreadArgs*)args;
    
    int bunch =50;
    int total=bunch*GBFSigmaByteLen;
    uint8_t* e=(uint8_t*)calloc(total, sizeof(uint8_t));
    for(int i=0;i<myArgs->size;i+=bunch){
        if(i+bunch>=myArgs->size){
            bunch=myArgs->size-i;
            total=bunch*GBFSigmaByteLen;
        }
        for(int j=0;j<bunch;j++){
            while(myArgs->shared[i+j]==NULL){
                usleep(1);
            }
            memcpy(e+(j*GBFSigmaByteLen), myArgs->shared[i+j], GBFSigmaByteLen);
        }
        
        send(myArgs->socket, e, total, 0);
    }
    free(myArgs);
    
    
//    for(int32_t i=0;i<myArgs->size;i++){
//        while(myArgs->shared[i]==NULL){
//            usleep(1);
//        }
//        send(myArgs->socket, myArgs->shared[i], GBFSigmaByteLen, 0);
//    }
    return NULL;
}

void* Server_BuildGBF(void * args){
    Server_BuildGBFArgs* myArgs=(Server_BuildGBFArgs*)args;
    
    MTBuilder_GBF(myArgs->filter, myArgs->bfParam, myArgs->keys, myArgs->k, myArgs->keyByteLen, myArgs->set, myArgs->eByteLen, myArgs->eNum, myArgs->ehash, myArgs->rndSeedLen);
    return NULL;
    
    
}

void* Server_step1Task(void* args){
    Server_step1TaskArgs* myArgs=(Server_step1TaskArgs*)args;
    
    for(int j=myArgs->i;j<myArgs->sndr->k;j+=myArgs->trdNum){
        //myArgs->pks[j]=EC_POINT_new(myArgs->sndr->baseOTRcvr->curve);
        OTExtSndr_step1Single(myArgs->sndr, myArgs->cs[j], j, myArgs->pks);
    }
    free(myArgs);
    return NULL;
}

void* Server_step3_1Task(void* args){
    Server_step3_1TaskArgs* myArgs=(Server_step3_1TaskArgs*)args;
    MessageDigest* H = getHashInstance(myArgs->sndr->k);
    for(int j=myArgs->i;j<myArgs->sndr->k;j+=myArgs->trdNum){
        //OTExtSndr_step3_1Single(myArgs->sndr, myArgs->received[j], myArgs->sndr->k/8,myArgs->grs[j] , j, H, myArgs->result);
        
        OTExtSndr_step3_1Single(myArgs->sndr, myArgs->received[j], myArgs->sndr->seedByteLen, myArgs->grs[j], j, H, myArgs->result);
    }
    MD_Destroy(H);
    free(myArgs);
    return NULL;
}

void* Server_step3_2Task(void* args){
    Server_step3_2TaskArgs* myArgs=(Server_step3_2TaskArgs*)args;
    for(int j=myArgs->i;j<myArgs->sndr->k;j+=myArgs->trdNum){
        while(myArgs->input[j]==NULL){
            usleep(1);
        }
        
        OTExtSndr_step3_2Single(myArgs->sndr, myArgs->input[j], j, myArgs->out);
        
    }
    free(myArgs);
    return NULL;
    
}

void* Server_step3_3Task(void* args){
    Server_step3_3TaskArgs* myArgs=(Server_step3_3TaskArgs*)args;
    MessageDigest* H = getHashInstance(myArgs->sndr->k);
    uint8_t* rowBuf= (uint8_t*)calloc(myArgs->Q->byteLenRow, sizeof(uint8_t));
    uint8_t* hashBuf= (uint8_t*)calloc(H->digestLen, sizeof(uint8_t));
    
    for(int j=myArgs->i;j<myArgs->sndr->m;j+=myArgs->trdNum){
        
        myArgs->shared[j]= OTExtSndr_step3_3SingleMT(myArgs->sndr, myArgs->Q, j, H, rowBuf, hashBuf);
    }
    
    free(rowBuf);
    free(hashBuf);
    MD_Destroy(H);
    free(myArgs);
    return NULL;
    
}
