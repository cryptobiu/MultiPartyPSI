//
//  GarbledBF.c
//  PSI
//
//  Created by Changyu Dong on 03/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//
#include "GarbledBF.h"
#include <stdio.h>

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
    finalShare =(uint8_t*)malloc(sizeof(uint8_t)*filter->m_GBFSigmaByteLen);
    memcpy(finalShare, ehash, filter->m_GBFSigmaByteLen);
    
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
            if (filter->bf[index]==0) {
                if (emptySlot == -1) {
                    emptySlot = index;
                } else {
                    //uint8_t* bytes;
                    //bytes=(uint8_t*)calloc(filter->m_GBFSigmaByteLen,sizeof(uint8_t));
                    //AESRandom_NextBytes(rnd, bytes, filter->m_GBFSigmaByteLen);
                    //memcpy(&filter->data[index*filter->m_GBFSigmaByteLen] ,bytes, filter->m_GBFSigmaByteLen);
                    xorByteArray(finalShare, &filter->data[index*filter->m_GBFSigmaByteLen], filter->m_GBFSigmaByteLen);
                    //free(bytes);
                    filter->bf[index] = 1;
                }
            } else {
                xorByteArray(finalShare, &filter->data[index*filter->m_GBFSigmaByteLen], filter->m_GBFSigmaByteLen);
            }
        }
        
    }
    // last thing to do is to put finalShare into the first empty slot

    memcpy(&filter->data[emptySlot*filter->m_GBFSigmaByteLen] ,finalShare, filter->m_GBFSigmaByteLen);
    filter->bf[emptySlot] = 1;
    free(finalShare);
}


void GBF_addMT(GarbledBF*filter, int32_t* indexes, int32_t hashNum,uint8_t* ehash,RandomSource* rndSrc){
    assert(hashNum==filter->k);
    
    uint8_t* finalShare;
    finalShare =(uint8_t*)malloc(sizeof(uint8_t)*filter->m_GBFSigmaByteLen);
    memcpy(finalShare, ehash, filter->m_GBFSigmaByteLen);
    
    int emptySlot=-1;
    
    for(int i=0;i<hashNum;i++){
        int32_t index=indexes[i];
        
        if (index>=0) {
            if (filter->data[index] == NULL) {
                if (emptySlot == -1) {
                    emptySlot = index;
                    //data[index]=new byte[this.sigma];
                } else {
                    memcpy(&filter->data[index*filter->m_GBFSigmaByteLen], RandomSource_Take(rndSrc), filter->m_GBFSigmaByteLen);
                    xorByteArray(finalShare, &filter->data[index*filter->m_GBFSigmaByteLen], filter->m_GBFSigmaByteLen);
                }
            } else {
                xorByteArray(finalShare, &filter->data[index*filter->m_GBFSigmaByteLen], filter->m_GBFSigmaByteLen);
            }
        }
    }
    memcpy(&filter->data[emptySlot*filter->m_GBFSigmaByteLen], finalShare, filter->m_GBFSigmaByteLen);
    //markt this slot is malloced.
   //setBit(filter->bitmap, emptySlot, filter->bitMapLeadingZeroes);
}


void GBF_doFinal(GarbledBF* filter,AESRandom* rnd){
    for (int i = 0; i < filter->m; i++) {
        if (filter->data[i] ==NULL) {
            AESRandom_NextBytes(rnd, &filter->data[i*filter->m_GBFSigmaByteLen], filter->m_GBFSigmaByteLen);
        }
    }
}

void GBF_doFinalMT(GarbledBF* filter,RandomSource* rnd){
    for (int i = 0; i < filter->m; i++) {
        if (filter->data[i] ==NULL) {
            memcpy(&filter->data[i*filter->m_GBFSigmaByteLen], RandomSource_Take(rnd), filter->m_GBFSigmaByteLen);
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
            xorByteArray(recovered, &filter->data[index*filter->m_GBFSigmaByteLen], filter->m_GBFSigmaByteLen);
        }
    }
    return compareByteArray(recovered, ehash, filter->m_GBFSigmaByteLen);
    
}

int GBF_query_With_Indexes(GarbledBF* filter,int32_t* indexes, int32_t hashNum,uint8_t* ehash){
    uint8_t *recovered=(uint8_t *)calloc(filter->m_GBFSigmaByteLen, sizeof(uint8_t));
    
    for(int32_t i=0;i<hashNum;i++){
        int32_t index =indexes[i];
        
        if(index>=0){
            xorByteArray(recovered, &filter->data[index*filter->m_GBFSigmaByteLen],filter->m_GBFSigmaByteLen);
        }
    }
    return compareByteArray(recovered, ehash, filter->m_GBFSigmaByteLen);
}

