//
//  RangeHash.c
//  PSI
//
//  Created by Changyu Dong on 24/02/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "RangeHash.h"



int RangeHash_Create(RangeHash** hash,uint8_t *key, int32_t keyLen, int32_t m){
    RangeHash * newHash;
    newHash = malloc(sizeof(RangeHash));

    if (newHash==NULL){
        return 0;
    }
    
    if(!(newHash->ctx=malloc(sizeof(SHA_CTX))))
        return 0;
    
    SHA1_Init(newHash->ctx);
    
    newHash->m=m;
    newHash->key=key;
    newHash->keyLen=keyLen;
    
    if(keyLen<RangeHash_DataBufLen){
        memcpy(newHash->dataBuf, key, keyLen);
    }
    
    *hash=newHash;
    return 1;
}


void RangeHash_Destroy(RangeHash *hash){
    free(hash->ctx);
    free(hash);
}

int32_t RangeHash_Digest(RangeHash *H, uint8_t *data, int32_t dataLen){
    uint32_t inputLen=dataLen+H->keyLen;
    
    if(inputLen>RangeHash_DataBufLen){
        uint8_t *input;   
        input=malloc(sizeof(uint8_t)*inputLen);
        
        //append data to the key
        memcpy(input, H->key, H->keyLen);
        memcpy(input+H->keyLen, data, dataLen);
        
        //digest
        //H->digest(input,inputLen,H->Buf);
        SHA1_Init(H->ctx);
        SHA1_Update(H->ctx, input, inputLen);
        SHA1_Final((uint8_t*)H->Buf, H->ctx);
        free(input);
   
    }else{
        memcpy(H->dataBuf+H->keyLen, data, dataLen);
        //H->digest(H->dataBuf,inputLen,H->Buf);
        SHA1_Init(H->ctx);
        SHA1_Update(H->ctx, H->dataBuf, inputLen);
        SHA1_Final((uint8_t*)H->Buf, H->ctx);
    }
    
    
    //mod to produce an int output
    uint32_t first;
    uint32_t second;
    uint64_t num;
    first= __bswap_32(H->Buf[0]);
    
    for(int i=0;i<RangeHash_BuffLen-1;i++){
        second=__bswap_32(H->Buf[i+1]);
        //little endian
        num = (((uint64_t)first))|((uint64_t)second<<32);
        first= num%H->m;
        //first=divl(first,second,H->m);
    }
    return first;
    

    
    
}