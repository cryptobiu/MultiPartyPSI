//
//  NPOTRcvr.c
//  PSI
//
//  Created by Changyu Dong on 06/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>

#include "NPOTRcvr.h"
int NPOTRcvr_Create(NPOTRcvr** rcvr, uint8_t* str, int32_t strLen,int32_t LeadingZeroes, int32_t secLev){
    assert(secLev==80||secLev==128||secLev==192||secLev==256);
    NPOTRcvr* newRcvr=(NPOTRcvr*)malloc(sizeof(NPOTRcvr));
    assert(newRcvr!=NULL);
    
    newRcvr->str=str;
    newRcvr->strByteLen=strLen;
    newRcvr->leadingZeroes=LeadingZeroes;
    newRcvr->strBitLen=strLen*8-LeadingZeroes;
    newRcvr->ks= (BIGNUM**)malloc(sizeof(BIGNUM*)*newRcvr->strBitLen);
    
    //newRcvr->ctx=BN_CTX_new();
    newRcvr->md= (MessageDigest*)malloc(sizeof(MessageDigest));
    assert(newRcvr->md!=NULL);
    MD_Create(&newRcvr->md, secLev);
    
    newRcvr->rnd=(AESRandom*)malloc(sizeof(AESRandom));
    assert(newRcvr->rnd!=NULL);
    
    if(secLev==80){

        AESRandom_Create(&newRcvr->rnd, 16);
        
        newRcvr->curve=EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
        //newRcvr->bufSize=48;
         newRcvr->pointByteLen=25;
    }else if(secLev==128){
        AESRandom_Create(&newRcvr->rnd, 16);
        newRcvr->curve=EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
        //newRcvr->bufSize=64;
        newRcvr->pointByteLen=33;
    }else if(secLev==192){
        AESRandom_Create(&newRcvr->rnd, 24);
        
        newRcvr->curve=EC_GROUP_new_by_curve_name(NID_secp384r1);
        //newRcvr->bufSize=96;
        newRcvr->pointByteLen=49;
    }else if(secLev==256){
        
        AESRandom_Create(&newRcvr->rnd, 32);
        
        newRcvr->curve=EC_GROUP_new_by_curve_name(NID_secp521r1);
       // newRcvr->bufSize=131;
        newRcvr->pointByteLen=67;
    }

    newRcvr->order= BN_new();
    EC_GROUP_get_order(newRcvr->curve, newRcvr->order, NULL);
    
    newRcvr->g= EC_POINT_dup(EC_GROUP_get0_generator(newRcvr->curve),newRcvr->curve);
    
    for(int i=0;i<newRcvr->strBitLen;i++){
        newRcvr->ks[i]=BN_new();
        BN_rand_range(newRcvr->ks[i], newRcvr->order);
        
        //printf("%s\n",BN_bn2hex(newRcvr->ks[i]));
    }
    
    *rcvr=newRcvr;
    return 1;
    
}

void NPOTRcvr_Destroy(NPOTRcvr* rcvr){
    MD_Destroy(rcvr->md);
    AESRandom_Destroy(rcvr->rnd);
    
    BN_free(rcvr->order);
    EC_GROUP_free(rcvr->curve);
    //BN_CTX_free(rcvr->ctx);
    for(int32_t i=0;i<rcvr->strBitLen;i++){
        BN_free(rcvr->ks[i]);
    }
    free(rcvr->ks);
    free(rcvr);

}

EC_POINT** NPOTRcvr_RStep1(NPOTRcvr* rcvr, EC_POINT** Cs,int32_t CsLen){
    EC_POINT** PK= (EC_POINT**)malloc(sizeof(EC_POINT*)*rcvr->strBitLen);
    
    for(int32_t i=0;i<rcvr->strBitLen;i++){
        NPOTRcvr_RStep1Single(rcvr,Cs[i],i, PK);
    }
    
    return PK;
    
}

void NPOTRcvr_RStep1Single(NPOTRcvr* rcvr, EC_POINT* C, int32_t i, EC_POINT** PK){
    BN_CTX* ctx=BN_CTX_new();
    PK[i]= EC_POINT_new(rcvr->curve);
    if(getBit(rcvr->str, i, rcvr->leadingZeroes)){
        EC_POINT_mul(rcvr->curve, PK[i], NULL, rcvr->g, rcvr->ks[i], ctx);
        EC_POINT_invert(rcvr->curve, PK[i], ctx);
        EC_POINT_add(rcvr->curve, PK[i], C, PK[i], ctx);
    }else{
        EC_POINT_mul(rcvr->curve, PK[i], NULL, rcvr->g, rcvr->ks[i], ctx);
    }
     free(ctx);
}

uint8_t** NPOTRcvr_RStep2(NPOTRcvr* rcvr, uint8_t*** received, int32_t recLen, EC_POINT** grs){
    uint8_t** result = (uint8_t**)malloc(sizeof(uint8_t*)*rcvr->strBitLen);
    
    for(int i=0;i<rcvr->strBitLen;i++){
        NPOTRcvr_RStep2Single(rcvr,received[i],recLen,grs[i],i,rcvr->md,result);
    }
    return result;
    
}
void NPOTRcvr_RStep2Single(NPOTRcvr* rcvr, uint8_t** received,int32_t recLen,EC_POINT* gr, int32_t i, MessageDigest* H, uint8_t** result){
    EC_POINT* pkr = EC_POINT_new(rcvr->curve);
    BN_CTX* ctx= BN_CTX_new();
    EC_POINT_mul(rcvr->curve, pkr, NULL, gr, rcvr->ks[i], ctx);
    
    uint8_t* buf = (uint8_t*)calloc(rcvr->pointByteLen,sizeof(uint8_t));
    
    size_t len= EC_POINT_point2oct(rcvr->curve, pkr, POINT_CONVERSION_COMPRESSED, buf, rcvr->pointByteLen, ctx);
    
    H->Init(H->ctx);
    H->update(H->ctx,buf,len);
    uint8_t* digest=(uint8_t*)malloc(sizeof(uint8_t)*H->digestLen);
    if(getBit(rcvr->str, i, rcvr->leadingZeroes)){
        uint8_t one=1;
        H->update(H->ctx,&one,1);
        H->Final(digest,H->ctx);
        
        result[i]=received[1];
        xorByteArray(result[i], digest, recLen);
        
    }else{
        uint8_t zero=0;
        H->update(H->ctx,&zero,1);
        H->Final(digest,H->ctx);
        
        result[i]=received[0];
        xorByteArray(result[i], digest, recLen);
    }
    
    free(buf);
    free(digest);
     free(ctx);
    
    
}
