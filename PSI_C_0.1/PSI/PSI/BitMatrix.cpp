//
//  BitMatrix.c
//  PSI
//
//  Created by Changyu Dong on 07/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include "BitMatrix.h"

int BM_Create(BitMatrix** bm, int32_t m, int32_t k){
    BitMatrix * newBM=(BitMatrix *)calloc(1,sizeof(BitMatrix));
    
    assert(newBM!=NULL);
    
    newBM->m=m;
    newBM->k=k;
    
    int32_t r = m%8;
    if(r==0){
        newBM->byteLenCol=m/8;
        newBM->leadingZeroes=0;
    }
    else{
        newBM->byteLenCol=m/8+1;
        newBM->leadingZeroes=8-r;
    }
    
    r=k%8;
    
    if(r==0){
        newBM->byteLenRow=k/8;
        newBM->leadingZeroesRow=0;
    }
    else{
        newBM->byteLenRow=k/8+1;
        newBM->leadingZeroesRow=8-r;
    }
    
    newBM->data=(uint8_t**)calloc(k, sizeof(uint8_t*));
    assert(newBM->data!=NULL);
    
    uint8_t mask=128;
    for (int i=0;i<8;i++){
        newBM->masks[i]=(mask>>i);
    }
    
    *bm=newBM;
    return 1;
}

void BM_Destroy(BitMatrix* bm){
    free(bm->data);
    free(bm);
}

void BM_GetRow(BitMatrix* bm, int32_t i, uint8_t* result,int32_t rowLen){
    assert(i>=0 && i<bm->m);
    assert(rowLen==bm->byteLenRow);
    memset(result, 0, rowLen);
    
    //printBytes(result, rowLen);
    
    int32_t dataByteLoc = (i+bm->leadingZeroes)>>3;
    int32_t dataBitLoc= (i+bm->leadingZeroes)&7;
    
    int32_t byteLoc;
    int32_t bitInByte;
    int32_t index;
    
    for(int j=0;j<bm->k;j++){
        if((bm->data[j][dataByteLoc] & bm->masks[dataBitLoc]) !=0){
            index=j+bm->leadingZeroesRow;
            byteLoc=index>>3;
            bitInByte=index&7;
            
            //printf("index = %d, byteLoc= %d, bitInByte= %d\n", index, byteLoc,bitInByte);
            result[byteLoc]= result[byteLoc]|bm->masks[bitInByte];
        }
        //printBytes(bm->masks, 8);
    }
    
    
}

void BM_print(BitMatrix* bm){
    for(int i=0;i<bm->k;i++){
        printBytes(bm->data[i], bm->byteLenCol);
    }
}
