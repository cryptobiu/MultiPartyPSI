//
//  OTExtSndr.c
//  PSI
//
//  Created by Changyu Dong on 07/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>

#include "OTExtSndr.h"

int OTExtSndr_Create(OTExtSndr** sndr, int32_t m, int32_t secLev, uint8_t** toSend, int32_t lByteLen){
    OTExtSndr* newSndr=(OTExtSndr*)calloc(1,sizeof(OTExtSndr));
    assert(newSndr!=NULL);
    
    newSndr->m=m;
    newSndr->k=secLev;
    
    newSndr->secLev=secLev;
    newSndr->seedByteLen=newSndr->secLev/8;
    newSndr->lByteLength=lByteLen;
    newSndr->toSend=toSend;
    newSndr->seededRnd=(AESRandom**)calloc(secLev, sizeof(AESRandom*));
    
    newSndr->H=getHashInstance(secLev);
    
    //initialise s
    newSndr->sByteLen=getByteLenByBitLen(secLev);
    newSndr->sLeadingZeroes=getLeadingZeroes(secLev);
    newSndr->s=(uint8_t*)calloc(newSndr->sByteLen, sizeof(uint8_t));
    assert(newSndr->s!=NULL);
    AESRandom* rnd= getRandomInstance(secLev);
    assert(rnd!=NULL);
    AESRandom_NextBytes(rnd, newSndr->s,newSndr->sByteLen);
    NPOTRcvr_Create(&newSndr->baseOTRcvr, newSndr->s, newSndr->sByteLen, newSndr->sLeadingZeroes, secLev);
    
    assert(newSndr->baseOTRcvr!=NULL);
    
    
    newSndr->row=(uint8_t*)calloc(newSndr->sByteLen, sizeof(uint8_t));
    assert(newSndr->row!=NULL);
    
    newSndr->allZeroes=(uint8_t*)calloc(newSndr->sByteLen, sizeof(uint8_t));
    assert(newSndr->allZeroes!=NULL);
    
    newSndr->hashTemp=(uint8_t*)calloc(newSndr->H->digestLen, sizeof(uint8_t));
    assert(newSndr->hashTemp!=NULL);
    
    AESRandom_Destroy(rnd);
    *sndr=newSndr;
    return 1;
}

void OTExtSndr_Destroy(OTExtSndr* sndr){
    for(int i=0;i<sndr->k;i++){
        AESRandom_Destroy(sndr->seededRnd[i]);
    }
    free(sndr->seededRnd);
    free(sndr->s);
    
    MD_Destroy(sndr->H);
    
    NPOTRcvr_Destroy(sndr->baseOTRcvr);
    free(sndr->row);
    free(sndr->hashTemp);
    free(sndr->allZeroes);
    
}

void OTExtSndr_ReInitialise(OTExtSndr* sndr, uint8_t** toSend, int32_t m){
    sndr->toSend=toSend;
    sndr->m=m;
}

uint8_t** OTExtSndr_step3_1(OTExtSndr* sndr, uint8_t*** received, int32_t recStrLen,EC_POINT** grs){
    uint8_t ** out = (uint8_t **)calloc(sndr->k, sizeof(uint8_t*));
    for(int i=0;i<sndr->k;i++){
        OTExtSndr_step3_1Single(sndr, received[i],recStrLen, grs[i],i,sndr->H,out);
    }
    return out;
}

void OTExtSndr_step3_1Single(OTExtSndr* sndr, uint8_t** received, int32_t recStrLen,EC_POINT* gr,int32_t i,MessageDigest* H, uint8_t** out){
    NPOTRcvr_RStep2Single(sndr->baseOTRcvr, received, recStrLen, gr, i, H, out);
    sndr->seededRnd[i]=getSeededRandomInstance(out[i], sndr->k);
}

BitMatrix* OTExtSndr_step3_2(OTExtSndr* sndr,uint8_t** input){
    BitMatrix* bm;
    BM_Create(&bm, sndr->m,sndr->k);
    for(int i=0;i<sndr->k;i++){
        OTExtSndr_step3_2Single(sndr, input[i], i, bm);
    }
    return bm;
}

void OTExtSndr_step3_2Single(OTExtSndr* sndr,uint8_t* input, int32_t i, BitMatrix* out){
    
    if(getBit(sndr->s, i, sndr->sLeadingZeroes)){
        //1
        AESRandom_NextBytes(sndr->seededRnd[i], input, out->byteLenCol);
    }else{
        //0
        //uint8_t* ba =calloc(out->byteLenCol, sizeof(uint8_t));
        memset(input, 0, out->byteLenCol);
        AESRandom_NextBytes(sndr->seededRnd[i], input, out->byteLenCol);
        //input=ba;
    }
    //printBytes(input, out->byteLenCol);
    
    BM_SetColumn(out, i, input, out->byteLenCol);
}

uint8_t** OTExtSndr_step3_3(OTExtSndr* sndr,BitMatrix* Q){
    for(int j=0;j<sndr->m;j++){
        OTExtSndr_step3_3Single(sndr, Q, j, sndr->H);
    }
    return sndr->toSend;
}

uint8_t* OTExtSndr_step3_3Single(OTExtSndr* sndr,BitMatrix* Q, int32_t j,MessageDigest* md){
    memset(sndr->row, 0, sndr->sByteLen);
    BM_GetRow(Q, j, sndr->row,  sndr->sByteLen);
    //printBytes(sndr->row, sndr->sByteLen);
    //printBytes(sndr->s, sndr->sByteLen);
    xorByteArray(sndr->row, sndr->s, sndr->sByteLen);
   // printf("bytelenrow=%d\n",sndr->sByteLen);
    
    //printBytes(sndr->row, sndr->sByteLen);
    
    md->Init(md->ctx);
    md->update(md->ctx,sndr->row,sndr->sByteLen);
    md->update(md->ctx,&j,4);
    md->Final(sndr->hashTemp,md->ctx);
    //printBytes(sndr->hashTemp, md->digestLen);
    
    if(sndr->toSend[j]!=NULL){
        xorByteArray(sndr->toSend[j], sndr->hashTemp, sndr->lByteLength);
    }else{
        sndr->toSend[j]=(uint8_t*)calloc(sndr->lByteLength, sizeof(uint8_t));
        memcpy(sndr->toSend[j], sndr->hashTemp, sndr->lByteLength);
    }
    return sndr->toSend[j];
    
}

uint8_t* OTExtSndr_step3_3SingleMT(OTExtSndr* sndr,BitMatrix* Q, int32_t j,MessageDigest* md,uint8_t* rowBuf, uint8_t* hashBuf){
    //memset(rowBuf, 0, sndr->sByteLen);
    memcpy(rowBuf, sndr->allZeroes, sndr->sByteLen);
    BM_GetRow(Q, j, rowBuf, sndr->sByteLen);
    xorByteArray(rowBuf, sndr->s, sndr->sByteLen);
    
    md->Init(md->ctx);
    md->update(md->ctx,rowBuf,sndr->sByteLen);
    md->update(md->ctx,&j,4);
    md->Final(hashBuf,md->ctx);
    
    if(sndr->toSend[j]!=NULL){
        xorByteArray(sndr->toSend[j], hashBuf, sndr->lByteLength);
    }else{
        sndr->toSend[j]=(uint8_t*)calloc(sndr->lByteLength, sizeof(uint8_t));
        memcpy(sndr->toSend[j], hashBuf, sndr->lByteLength);
    }
    return sndr->toSend[j];
}
