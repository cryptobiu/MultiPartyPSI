//
//  OTExtRcvr.c
//  PSI
//
//  Created by Changyu Dong on 07/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include "OTExtRcvr.h"



int OTExtRcvr_Create(OTExtRcvr** rcvr,uint8_t* str, int32_t m, int32_t secLev){
    assert(secLev==80||secLev==128||secLev==192||secLev==256);
    
    OTExtRcvr* newRcvr=(OTExtRcvr*)calloc(1,sizeof(OTExtRcvr));
    
    assert(newRcvr!=NULL);
    
    newRcvr->k=secLev;
    newRcvr->secLev=secLev;
    newRcvr->seedByteLen=secLev/8;
    newRcvr->m=m;
    newRcvr->str=str;
    newRcvr->strByteLen=getByteLenByBitLen(m);
    
    MD_Create(&newRcvr->md, secLev);
    
    newRcvr->leadingZeroes=getLeadingZeroes(m);
    
    //generate random seeds
    //initialise randoms
    AESRandom* rnd= getRandomInstance(secLev);
    newRcvr->seeds=(uint8_t***)calloc(secLev, sizeof(uint8_t**));
    newRcvr->seededRnd=(AESRandom***)calloc(secLev, sizeof(AESRandom**));
    assert(newRcvr->seeds!=NULL);

    for(int i=0;i<secLev;i++){
        newRcvr->seeds[i]=(uint8_t**)calloc(2, sizeof(uint8_t*));
        newRcvr->seededRnd[i]=(AESRandom**)calloc(3, sizeof(AESRandom*));
        assert(newRcvr->seeds[i]!=NULL);
        assert(newRcvr->seededRnd[i]!=NULL);
        
        newRcvr->seeds[i][0]=(uint8_t*)calloc(newRcvr->seedByteLen, sizeof(uint8_t));
        assert(newRcvr->seeds[i][0]!=NULL);
        AESRandom_NextBytes(rnd, newRcvr->seeds[i][0], newRcvr->seedByteLen);
        
        newRcvr->seededRnd[i][0]= getSeededRandomInstance(newRcvr->seeds[i][0], secLev);
        newRcvr->seededRnd[i][2]= getSeededRandomInstance(newRcvr->seeds[i][0], secLev);
        
        newRcvr->seeds[i][1]=(uint8_t*)calloc(newRcvr->seedByteLen, sizeof(uint8_t));
        assert(newRcvr->seeds[i][1]!=NULL);
        AESRandom_NextBytes(rnd, newRcvr->seeds[i][1], newRcvr->seedByteLen);
         newRcvr->seededRnd[i][1]= getSeededRandomInstance(newRcvr->seeds[i][1], secLev);
    }
    
    //initialise bitmattrix
    BM_Create(&newRcvr->T, m, secLev);
    assert(newRcvr->T!=NULL);
    
    for(int i=0;i<secLev;i++){
        uint8_t* ti = (uint8_t*)calloc(newRcvr->strByteLen, sizeof(uint8_t));
        assert(ti!=NULL);
        AESRandom_NextBytes( newRcvr->seededRnd[i][0],ti,newRcvr->strByteLen);
        BM_SetColumn(newRcvr->T, i, ti, newRcvr->strByteLen);
    }
    
    NPOTSndr_Create(&newRcvr->baseOTSndr, secLev, newRcvr->seedByteLen, newRcvr->seeds, newRcvr->secLev);
    
    assert(newRcvr->baseOTSndr!=NULL);
    
    newRcvr->allZero=(uint8_t*)calloc(getByteLenByBitLen(secLev), sizeof(uint8_t));
    newRcvr->row=(uint8_t*)calloc(getByteLenByBitLen(secLev), sizeof(uint8_t));
    newRcvr->hashTemp=(uint8_t*)calloc(newRcvr->md->digestLen, sizeof(uint8_t));
    //newRcvr->column=calloc(getByteLenByBitLen(m), sizeof(uint8_t));

    AESRandom_Destroy(rnd);
    
    *rcvr=newRcvr;
    return 1;
}


void OTExtRcvr_Destroy(OTExtRcvr* rcvr){
    for(int i=0;i<rcvr->k;i++){
        free(rcvr->T->data[i]);
    }
    BM_Destroy(rcvr->T);
    
    MD_Destroy(rcvr->md);
    
    for(int i=0;i<rcvr->k;i++){
        free(rcvr->seeds[i][0]);
        free(rcvr->seeds[i][1]);
        free(rcvr->seeds[i]);
        AESRandom_Destroy(rcvr->seededRnd[i][0]);
        AESRandom_Destroy(rcvr->seededRnd[i][1]);
        AESRandom_Destroy(rcvr->seededRnd[i][2]);
        free(rcvr->seededRnd);
    }
    free(rcvr->seeds);
    free(rcvr->seededRnd);
    free(rcvr->row);
    free(rcvr->hashTemp);
    NPOTSndr_Destroy(rcvr->baseOTSndr);
    
}

int OTExtRcvr_ReInitialise(OTExtRcvr* rcvr, int32_t m, uint8_t* str, int32_t strByteLen){
    assert(m==8*strByteLen);
    
    rcvr->str=str;
    
    rcvr->leadingZeroes=0;
    rcvr->strByteLen=strByteLen;
    
    if(rcvr->m==m){
        //reused the byte arrays
        for(int i=0;i<rcvr->k;i++){
            memset(rcvr->T->data[i], 0, rcvr->strByteLen);
            AESRandom_NextBytes(rcvr->seededRnd[i][0], rcvr->T->data[i], rcvr->strByteLen);
        }
    }else{
        rcvr->m=m;
        for(int i=0;i<rcvr->k;i++){
            free(rcvr->T->data[i]);
        }
        BM_Destroy(rcvr->T);
        BM_Create(&rcvr->T, m, rcvr->k);
        for(int i=0;i<rcvr->k;i++){
            uint8_t* ti = (uint8_t*)calloc(strByteLen, sizeof(uint8_t));
            assert(ti!=NULL);
            AESRandom_NextBytes(rcvr->seededRnd[i][0], ti, rcvr->strByteLen);
            BM_SetColumn(rcvr->T, i, ti, strByteLen);
        }
        
        
    }

    
    return 1;
}

typedef struct rcvr_Rein_args{
    BitMatrix* bm;
    AESRandom*** seededRnd;
    int32_t byteLen;
    int32_t i;
    int32_t newMatrix;
    int32_t threadNum;
}rcvr_Rein_args;


void* rcvr_rein_task(void* args){
    rcvr_Rein_args* myArgs=(rcvr_Rein_args*)args;
    
    if(myArgs->newMatrix){
        for(int i=myArgs->i;i<myArgs->bm->k;i+=myArgs->threadNum){
            uint8_t* ti = (uint8_t*)calloc(myArgs->byteLen, sizeof(uint8_t));
            assert(ti!=NULL);
            AESRandom_NextBytes(myArgs->seededRnd[i][0], ti, myArgs->byteLen);
            BM_SetColumn(myArgs->bm, i, ti, myArgs->byteLen);
        }
    }else{
        for(int i=myArgs->i;i<myArgs->bm->k;i+=myArgs->threadNum){
            memset(myArgs->bm->data[i], 0, myArgs->byteLen);
            AESRandom_NextBytes(myArgs->seededRnd[i][0], myArgs->bm->data[i], myArgs->byteLen);
        }
    }
    free(args);
    return NULL;
}

int OTExtRcvr_ReInitialiseMT(OTExtRcvr* rcvr, int32_t m, uint8_t* str, int32_t strByteLen){
    assert(m==8*strByteLen);
    
    rcvr->str=str;
    
    rcvr->leadingZeroes=0;
    rcvr->strByteLen=strByteLen;
    
    int newMatrix=0;
    
    if(rcvr->m!=m){
        rcvr->m=m;
        newMatrix=1;
        for(int i=0;i<rcvr->k;i++){
            free(rcvr->T->data[i]);
        }
        BM_Destroy(rcvr->T);
        BM_Create(&rcvr->T, m, rcvr->k);
    }

    
    int cores= num_cores();
    pthread_t* rndThreads;
    rndThreads=(pthread_t*)malloc(sizeof(pthread_t)*cores);
    assert(rndThreads!=NULL);
    
    for(int i=0;i<cores;i++){
        rcvr_Rein_args * args = (rcvr_Rein_args*)calloc(1, sizeof(rcvr_Rein_args));
        args->bm=rcvr->T;
        args->byteLen=strByteLen;
        args->i=i;
        args->newMatrix=newMatrix;
        args->seededRnd=rcvr->seededRnd;
        args->threadNum=cores;
        
        pthread_create(&rndThreads[i], NULL, &rcvr_rein_task, args);
        
    }
    
    for(int i=0;i<cores;i++){
        pthread_join(rndThreads[i], NULL);
    }
    
    free(rndThreads);
    return 1;
}

void OTExtRcvr_MatrixEnc(OTExtRcvr* rcvr){
    for(int i=0;i<rcvr->k;i++){
        OTExtRcvr_MatrixEncSingle(rcvr,i);
    }
}

void OTExtRcvr_MatrixDec(OTExtRcvr* rcvr){
    for(int i=0;i<rcvr->k;i++){
        OTExtRcvr_MatrixDecSingle(rcvr,i);
    }
}

uint8_t** OTExtRcvr_step2_2(OTExtRcvr* rcvr){
    OTExtRcvr_MatrixEnc(rcvr);
    return rcvr->T->data;
};

uint8_t* OTExtRcvr_step2_2Single(OTExtRcvr* rcvr,int32_t i){
    OTExtRcvr_MatrixEncSingle(rcvr, i);
    return rcvr->T->data[i];
}

void OTExtRcvr_step4_2(OTExtRcvr* rcvr,uint8_t** received,int32_t recStrLen){
    for(int i=0;i<rcvr->m;i++){
        OTExtRcvr_step4_2Single(rcvr, received[i], recStrLen, i, rcvr->md);
    }
}

void OTExtRcvr_step4_2Single(OTExtRcvr* rcvr,uint8_t* received, int32_t recLen,int32_t i, MessageDigest* H){
    memset(rcvr->row, 0, rcvr->T->byteLenRow);
    //printf("byteLenRow=%d\n",rcvr->T->byteLenRow);
    BM_GetRow(rcvr->T, i, rcvr->row, rcvr->T->byteLenRow);
    //printBytes(rcvr->row, rcvr->T->byteLenRow);
    H->Init(H->ctx);
    H->update(H->ctx,rcvr->row,rcvr->T->byteLenRow);
    H->update(H->ctx,&i,4);
    H->Final(rcvr->hashTemp,H->ctx);
    
    //printf("i= %d\n",i);
    //printBytes(rcvr->hashTemp, H->digestLen);
    
    xorByteArray(received, rcvr->hashTemp, recLen);
}

void OTExtRcvr_step4_2SingleMT(OTExtRcvr* rcvr,uint8_t* received, int32_t recLen, int32_t i, MessageDigest* H, uint8_t* rowBuf, uint8_t* hashBuf){
    //memset(rowBuf, 0, rcvr->T->byteLenRow);
    memcpy(rowBuf, rcvr->allZero, rcvr->T->byteLenRow);
    BM_GetRow(rcvr->T, i, rowBuf, rcvr->T->byteLenRow);
    
    H->Init(H->ctx);
    H->update(H->ctx,rowBuf,rcvr->T->byteLenRow);
    H->update(H->ctx,&i,4);
    H->Final(hashBuf,H->ctx);
    xorByteArray(received, hashBuf, recLen);
}
