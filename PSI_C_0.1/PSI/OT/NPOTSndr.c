//
//  NPOTSndr.c
//  PSI
//
//  Created by Changyu Dong on 06/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include "NPOTSndr.h"

int NPOTSndr_Create(NPOTSndr** sndr, int32_t k, int32_t sendByteLen, uint8_t*** toSend, int32_t secLev){
    NPOTSndr* newSndr = malloc(sizeof(NPOTSndr));
    
    assert(newSndr!=NULL);
    
    newSndr->k=k;
    newSndr->sendByteLen=sendByteLen;
    newSndr->toSend=toSend;
    
    //newSndr->ctx=BN_CTX_new();
    newSndr->md= malloc(sizeof(MessageDigest));
    assert(newSndr->md!=NULL);
    
    MD_Create(&newSndr->md, secLev);
    

    
    if(secLev==80){

        newSndr->curve=EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
       // newSndr->bufSize=48;
        newSndr->pointByteLen=25;
    }else if(secLev==128){

        newSndr->curve=EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
        //newSndr->bufSize=64;
        newSndr->pointByteLen=33;
    }else if(secLev==192){
        
        newSndr->curve=EC_GROUP_new_by_curve_name(NID_secp384r1);
        //newSndr->bufSize=96;
        newSndr->pointByteLen=49;
    }else if(secLev==256){
                
        newSndr->curve=EC_GROUP_new_by_curve_name(NID_secp521r1);
        //newSndr->bufSize=131;
        newSndr->pointByteLen=67;
    }
    BN_CTX* ctx=BN_CTX_new();
    newSndr->order= BN_new();
    EC_GROUP_get_order(newSndr->curve, newSndr->order, ctx);
    
    newSndr->g=EC_POINT_dup(EC_GROUP_get0_generator(newSndr->curve),newSndr->curve);
    
    newSndr->rs=malloc(sizeof(BIGNUM*)*newSndr->k);
    assert(newSndr->rs!=NULL);
    
    newSndr->cs=malloc(sizeof(EC_POINT*)*newSndr->k);
    assert(newSndr->cs!=NULL);
    
    newSndr->crs=malloc(sizeof(EC_POINT*)*newSndr->k);
    assert(newSndr->crs!=NULL);
    
    newSndr->grs=malloc(sizeof(EC_POINT*)*newSndr->k);
    assert(newSndr->grs!=NULL);

    
    for(int i=0;i<newSndr->k;i++){
        newSndr->rs[i] = BN_new();
        BN_rand_range(newSndr->rs[i], newSndr->order);
        
       // printf("r1: %s\n",BN_bn2hex(newSndr->rs[i]));

        
        newSndr->cs[i] = EC_POINT_new(newSndr->curve);
        EC_POINT_mul(newSndr->curve, newSndr->cs[i], NULL, newSndr->g, newSndr->rs[i], ctx);
        
       // printf("cs: %s\n",EC_POINT_point2hex(newSndr->curve, newSndr->cs[i], POINT_CONVERSION_COMPRESSED, ctx));
        
        BN_rand_range(newSndr->rs[i], newSndr->order);
        
        //printf("r2: %s\n",BN_bn2hex(newSndr->rs[i]));
        
        newSndr->crs[i] = EC_POINT_new(newSndr->curve);
        EC_POINT_mul(newSndr->curve, newSndr->crs[i], NULL, newSndr->cs[i], newSndr->rs[i], ctx);
        
       // printf("crs: %s\n",EC_POINT_point2hex(newSndr->curve, newSndr->crs[i], POINT_CONVERSION_COMPRESSED, ctx));
        
        newSndr->grs[i] = EC_POINT_new(newSndr->curve);
        EC_POINT_mul(newSndr->curve, newSndr->grs[i], NULL, newSndr->g, newSndr->rs[i], ctx);
        //printf("grs: %s\n",EC_POINT_point2hex(newSndr->curve, newSndr->crs[i], POINT_CONVERSION_COMPRESSED, ctx));
    }
    //printf("g: %s\n",EC_POINT_point2hex(newSndr->curve, newSndr->g, POINT_CONVERSION_COMPRESSED, ctx));
    *sndr=newSndr;
    BN_CTX_free(ctx);
    return 1;
}

void NPOTSndr_Destroy(NPOTSndr* sndr){
    EC_GROUP_free(sndr->curve);
    //EC_POINT_free(sndr->g);
    BN_free(sndr->order);
    //BN_CTX_free(sndr->ctx);
    
    MD_Destroy(sndr->md);
    
    for(int i=0;i<sndr->k;i++){
        BN_free(sndr->rs[i]);
        EC_POINT_free(sndr->cs[i]);
        EC_POINT_free(sndr->crs[i]);
        EC_POINT_free(sndr->grs[i]);
    }
    free(sndr->rs);
    free(sndr->crs);
    free(sndr->cs);
    free(sndr->grs);
    
}

EC_POINT** NPOTSndr_Step(NPOTSndr* sndr,EC_POINT** PK0s, uint8_t*** encrypted){
    for(int i=0;i<sndr->k;i++){
        NPOTSndr_StepSingle(sndr,PK0s[i],i,encrypted, sndr->md);
    }
    return sndr->grs;
}

EC_POINT* NPOTSndr_StepSingle(NPOTSndr* sndr,EC_POINT* PK0,int32_t i, uint8_t*** encrypted, MessageDigest* H){
    BN_CTX* ctx= BN_CTX_new();
    
    BIGNUM* R= sndr->rs[i];
    
    EC_POINT * pk0r = EC_POINT_new(sndr->curve);
    
    EC_POINT_mul(sndr->curve, pk0r, NULL, PK0, R, ctx);
    
    EC_POINT * pk1r=EC_POINT_dup(pk0r, sndr->curve);
    
    EC_POINT_invert(sndr->curve, pk1r, ctx);
    
    EC_POINT_add(sndr->curve, pk1r, pk1r, sndr->crs[i], ctx);
    
    uint8_t* buf=calloc(sndr->pointByteLen, sizeof(uint8_t));
    
    size_t len= EC_POINT_point2oct(sndr->curve, pk0r, POINT_CONVERSION_COMPRESSED, buf, sndr->pointByteLen, ctx);
    
    //printf("Point byte length: %zd\n",len);
    

    uint8_t* digest=malloc(sizeof(uint8_t)*H->digestLen);
    uint8_t zero =0;
    H->Init(H->ctx);
    H->update(H->ctx,buf,len);
    H->update(H->ctx,&zero,1);
    H->Final(digest,H->ctx);
    
    encrypted[i][0]= malloc(sizeof(uint8_t)*sndr->sendByteLen);
    memcpy(encrypted[i][0], sndr->toSend[i][0], sndr->sendByteLen);
    
    xorByteArray(encrypted[i][0], digest, sndr->sendByteLen);
    
    
    len= EC_POINT_point2oct(sndr->curve, pk1r, POINT_CONVERSION_COMPRESSED, buf, sndr->pointByteLen, ctx);
    
    uint8_t one =1;
    H->Init(H->ctx);
    H->update(H->ctx,buf,len);
    H->update(H->ctx,&one,1);
    H->Final(digest,H->ctx);
    
    encrypted[i][1]= malloc(sizeof(uint8_t)*sndr->sendByteLen);
    memcpy(encrypted[i][1], sndr->toSend[i][1], sndr->sendByteLen);
    
    xorByteArray(encrypted[i][1], digest, sndr->sendByteLen);
    
    EC_POINT_free(pk0r);
    EC_POINT_free(pk1r);
    free(buf);
    free(digest);
    free(ctx);
    
    return sndr->grs[i];
    
}