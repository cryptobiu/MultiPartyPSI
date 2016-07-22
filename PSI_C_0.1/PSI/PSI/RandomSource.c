//
//  RandomSource.c
//  PSI
//
//  Created by Changyu Dong on 04/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>

#include "RandomSource.h"


int RandomSource_Create(RandomSource** rndSrc, int32_t numRows, int32_t Blocksize){
    RandomSource * rnd;
    
    rnd=malloc(sizeof(RandomSource));
    
    if(rnd==NULL)
        return 0;
    
    rnd->rndBytes=calloc(numRows,sizeof(row*));
    
    if(rnd->rndBytes==NULL)
        return 0;
    
    rnd->numRows=numRows;
    rnd->blockSize=Blocksize;
    
    rnd->index=0;
    rnd->start=0;
    
    *rndSrc=rnd;
    
    return 1;
}

void RandomSource_Destroy(RandomSource* rnd){
    for(uint32_t i=0;i<rnd->numRows;i++){
        free(rnd->rndBytes[i]->bytes);
        free(rnd->rndBytes[i]);
    }
    
    free(rnd);
}

//inline uint8_t* RandomSource_Take(RandomSource* rndSrc)
