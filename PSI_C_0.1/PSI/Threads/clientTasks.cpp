//
//  clientTasks.c
//  PSI
//
//  Created by Changyu Dong on 12/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>

#include "clientTasks.h"

void* Client_inputThread (void* args){
    Client_inputThreadArgs* myArgs= (Client_inputThreadArgs*)args;
    int32_t bunch=50;
    int32_t total=bunch*GBFSigmaByteLen;
    uint8_t* e= (uint8_t*)calloc(((size_t)myArgs->size)*GBFSigmaByteLen, sizeof(uint8_t));
    uint8_t* eStart=e;
    
    for(int32_t i=0;i<myArgs->size;i+=bunch){
        if(i+bunch>=myArgs->size){
            bunch=myArgs->size-i;
            total=bunch*GBFSigmaByteLen;
        }
        
        receiveBulk(myArgs->socket, total, e);
        
        for(int32_t j=0;j<bunch;j++){
            //myArgs->shared[i+j]=e+j*GBFSigmaByteLen;
            uint8_t* ba=(uint8_t*)malloc(sizeof(uint8_t)*GBFSigmaByteLen);
            memcpy(ba, e+j*GBFSigmaByteLen, GBFSigmaByteLen);
            myArgs->shared[i+j]=ba;
        }
        e=e+total;
    }
    free(eStart);
    free(myArgs);
    return NULL;
    
//    for(int i=0;i<myArgs->size;i++){
//        uint8_t* e= calloc(GBFSigmaByteLen, sizeof(uint8_t));
//        recv(myArgs->socket, e, GBFSigmaByteLen, 0);
//        myArgs->shared[i]=e;
//    }
    return NULL;
    
}

void* Client_Step2_1Task(void* args){
    Client_Step2_1TaskArgs* myArgs=(Client_Step2_1TaskArgs*)args;
    MessageDigest* H = getHashInstance(myArgs->rcvr->secLev);
    for(int j=myArgs->i;j<myArgs->rcvr->k;j+=myArgs->thrNum){
        myArgs->grs[j]= OTExtRcvr_step2_1Single(myArgs->rcvr, myArgs->pk0s[j], j, myArgs->encrypted, H);
    }
    
    MD_Destroy(H);
    free(myArgs);
    return NULL;
}

void* Client_Step2_2Task(void* args){
    Client_Step2_2TaskArgs* myArgs=(Client_Step2_2TaskArgs*)args;
    for(int j=myArgs->i;j<myArgs->rcvr->k;j+=myArgs->thrNum){
        if(myArgs->toSend==NULL)
            OTExtRcvr_step2_2Single(myArgs->rcvr, j);
        else{
            myArgs->toSend[j]=OTExtRcvr_step2_2Single(myArgs->rcvr, j);
        }
    }
    free(myArgs);
    return NULL;
}

void* Client_Step4_1Task(void* args){
    Client_Step4_1TaskArgs* myArgs=(Client_Step4_1TaskArgs*) args;
    for(int j=myArgs->i;j<myArgs->rcvr->k;j+=myArgs->thrNum){
        
        OTExtRcvr_step4_1Single(myArgs->rcvr, j);
    }
    
    free(myArgs);
    return NULL;
}

void* Client_Step4_2Task(void* args){
    Client_Step4_2TaskArgs* myArgs= (Client_Step4_2TaskArgs*)args;
    
    MessageDigest* H= getHashInstance(myArgs->rcvr->secLev);
    
    uint8_t* rowBuf= (uint8_t*)calloc(myArgs->rcvr->T->byteLenRow, sizeof(uint8_t));
    uint8_t* selection= myArgs->rcvr->str;
    uint8_t* hashBuf= (uint8_t*)calloc(H->digestLen, sizeof(uint8_t));
    
    for(int j=myArgs->i;j<myArgs->rcvr->m;j+=myArgs->thrNum){
        while(myArgs->shared[j]==NULL){
            usleep(1);
        }
        
        if(getBit(selection, j, myArgs->rcvr->leadingZeroes)){
            OTExtRcvr_step4_2SingleMT(myArgs->rcvr, myArgs->shared[j], GBFSigmaByteLen, j, H, rowBuf, hashBuf);
        }else{
            if(myArgs->partial){
                free(myArgs->shared[j]);
            }
        }
        
    }
    
    free(hashBuf);
    free(rowBuf);
    MD_Destroy(H);
    free(myArgs);
    return NULL;
}

void* Client_QueryTask(void* args){
    Client_QueryTaskArgs* myArgs=(Client_QueryTaskArgs*)args;
    
    for(int j=myArgs->i;j<myArgs->n;j+=myArgs->thrNum){
        if(GBF_query_With_Indexes(myArgs->filter, myArgs->indexes[j], myArgs->hashNum, myArgs->eHashes[j])){
            list_ins_next(myArgs->list, NULL, myArgs->set[j]);
        }
    }
    return NULL;
    
}