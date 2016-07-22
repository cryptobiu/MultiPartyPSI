//
//  GarbledBF.c
//  PSI
//
//  Created by Changyu Dong on 03/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//
#include "GarbledBF.h"
#include <stdio.h>


int GBF_Create(GarbledBF** filter, int32_t m, int32_t k){
    GarbledBF* gbf;
    
    if(!(gbf=(GarbledBF*)malloc(sizeof(GarbledBF)))){
        return 0;
    }
    gbf->m=m;
    gbf->k=k;
    gbf->indexes=(int32_t*)calloc(k, sizeof(int32_t));
    //gbf->MT=0;
    //gbf->bitmap=NULL;
    
    if(!(gbf->data= (uint8_t**)calloc(m,sizeof(uint8_t*)))){
        return 0;
    }
    
    *filter=gbf;
    return 1;
}

int GBF_CreateForMT(GarbledBF** filter,int32_t m, int32_t k){
    GarbledBF* gbf;
    
    if(!GBF_Create(&gbf,m,k)){
        return 0;
    };
    
    //gbf->MT=1;
   // gbf->bitMapByteLen=getByteLenByBitLen(gbf->m);
    //gbf->bitMapLeadingZeroes=getLeadingZeroes(gbf->m);
   // gbf->bitmap=calloc(gbf->bitMapByteLen,sizeof(uint8_t));
    
    //if(gbf->bitmap==NULL)
   //     return 0;
    
    *filter=gbf;
    return 1;
}

void GBF_Destroy(GarbledBF* filter){
    
    
//    if(filter->MT){
//        for(int i=0;i<filter->m;i++){
//            if(getBit(filter->bitmap, i, filter->bitMapLeadingZeroes)){
//                free(filter->data[i]);
//            }
//        }
//        free(filter->bitmap);
//    }else{
//        for(int i=0;i<filter->m;i++){
//            free(filter->data[i]);
//        }
//    }
    free(filter->indexes);
    
    free(filter);
}

void GBF_add(GarbledBF*filter, const std::vector<boost::shared_ptr<RangeHash>> &hashes, int32_t hashNum, uint8_t* element,int32_t eLen, uint8_t* ehash,AESRandom* rnd){
    
    assert(filter->k == hashNum);
    
    uint8_t* finalShare;
    finalShare =(uint8_t*)malloc(sizeof(uint8_t)*GBFSigmaByteLen);
    memcpy(finalShare, ehash, GBFSigmaByteLen);
    
    int emptySlot=-1;
    
    int32_t* indHis=filter->indexes;
    //memset(indHis, 0, hashNum);
    
    for (int i = 0; i < hashNum; i++) {
        // hash to range [0,m-1], convert to index of array
        //int index = hashes[i].digest(element);
        int index = RangeHash_Digest(hashes[i].get(), element, eLen);
        indHis[i] = index;
        if (!exists(index, indHis, i)) {
            // if the slot is empty, further check whether this is the first
            // empty one encountered in
            // the loop. If so record the index and leave it empty,
            // otherwise generate a random
            // byte array (a share) to occupy the slot and XOR the share
            // with finalShare.
            
            // if the slot is not empty, just xor the content with
            // finalShare.
            if (filter->data[index]==NULL) {
                if (emptySlot == -1) {
                    emptySlot = index;
                } else {
                    uint8_t* bytes;
                    bytes=(uint8_t*)calloc(GBFSigmaByteLen,sizeof(uint8_t));
                    AESRandom_NextBytes(rnd, bytes, GBFSigmaByteLen);
                    filter->data[index] = bytes;
                    xorByteArray(finalShare, bytes, GBFSigmaByteLen);
                }
            } else {
                xorByteArray(finalShare, filter->data[index], GBFSigmaByteLen);
            }
        }
        
    }
    // last thing to do is to put finalShare into the first empty slot
    
    filter->data[emptySlot]=finalShare;
}


void GBF_addMT(GarbledBF*filter, int32_t* indexes, int32_t hashNum,uint8_t* ehash,RandomSource* rndSrc){
    assert(hashNum==filter->k);
    
    uint8_t* finalShare;
    finalShare =(uint8_t*)malloc(sizeof(uint8_t)*GBFSigmaByteLen);
    memcpy(finalShare, ehash, GBFSigmaByteLen);
    
    int emptySlot=-1;
    
    for(int i=0;i<hashNum;i++){
        int32_t index=indexes[i];
        
        if (index>=0) {
            if (filter->data[index] == NULL) {
                if (emptySlot == -1) {
                    emptySlot = index;
                    //data[index]=new byte[this.sigma];
                } else {
                    filter->data[index] = RandomSource_Take(rndSrc);
                    xorByteArray(finalShare, filter->data[index], GBFSigmaByteLen);
                }
            } else {
                xorByteArray(finalShare, filter->data[index], GBFSigmaByteLen);
            }
        }
    }
    filter->data[emptySlot] = finalShare;
    //markt this slot is malloced.
   //setBit(filter->bitmap, emptySlot, filter->bitMapLeadingZeroes);
}


void GBF_doFinal(GarbledBF* filter,AESRandom* rnd){
    for (int i = 0; i < filter->m; i++) {
        if (filter->data[i] ==NULL) {
            filter->data[i] = (uint8_t*)calloc(GBFSigmaByteLen,sizeof(uint8_t));
            AESRandom_NextBytes(rnd, filter->data[i], GBFSigmaByteLen);
        }
    }
}

void GBF_doFinalMT(GarbledBF* filter,RandomSource* rnd){
    for (int i = 0; i < filter->m; i++) {
        if (filter->data[i] ==NULL) {
            filter->data[i] = RandomSource_Take(rnd);
        }
    }
}

int GBF_query(GarbledBF* filter, RangeHash** hashes, int32_t hashNum, uint8_t* element,int32_t eLen, uint8_t* ehash){
    
    assert(hashNum==filter->k);
    
    uint8_t recovered[GBFSigmaByteLen]={0};
    int32_t* indexes = filter->indexes;
    //memset(indexes,0,hashNum);
    
    for (int i = 0; i < hashNum; i++) {
        int32_t index=RangeHash_Digest(hashes[i], element, eLen);
        indexes[i]=index;

        if (!exists(index, indexes, i)) {
            xorByteArray(recovered, filter->data[index], GBFSigmaByteLen);
        }
    }
    return compareByteArray(recovered, ehash, GBFSigmaByteLen);
    
}

int GBF_query_With_Indexes(GarbledBF* filter,int32_t* indexes, int32_t hashNum,uint8_t* ehash){
    uint8_t recovered[GBFSigmaByteLen]={0};
    
    for(int32_t i=0;i<hashNum;i++){
        int32_t index =indexes[i];
        
        if(index>=0){
            xorByteArray(recovered, filter->data[index],GBFSigmaByteLen);
        }
    }
    return compareByteArray(recovered, ehash, GBFSigmaByteLen);
}

