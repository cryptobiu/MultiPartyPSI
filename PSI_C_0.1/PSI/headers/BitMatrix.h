//
//  BitMatrix.h
//  PSI
//
//  Created by Changyu Dong on 07/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_BitMatrix_h
#define PSI_BitMatrix_h

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Util.h"

typedef struct BitMatrix{
    uint8_t** data;
    int32_t leadingZeroes;
    int32_t m;
    int32_t k;
    int32_t byteLenCol;
    int32_t byteLenRow;
    int32_t leadingZeroesRow;
    uint8_t masks[8];
}BitMatrix;

int BM_Create(BitMatrix** bm, int32_t m, int32_t k);

void BM_Destroy(BitMatrix* bm);

inline void BM_SetColumn(BitMatrix* bm, int32_t i, uint8_t* col,int32_t colLen){
    //printf("i= %d\n",i);
    assert(i>=0&&i<bm->k);
    assert(colLen==bm->byteLenCol);
    
    bm->data[i]=col;
};

inline uint8_t* BM_GetColumn(BitMatrix* bm, int32_t i){
    assert(i>=0&&i<bm->k);
    
    return bm->data[i];
}

void BM_GetRow(BitMatrix* bm, int32_t i, uint8_t* result,int32_t rowLen);

void BM_print(BitMatrix* bm);

#endif
