//
//  MTBuilder.h
//  PSI
//
//  Created by Changyu Dong on 04/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_MTBuilder_h
#define PSI_MTBuilder_h


#include <pthread.h>
#include "BF.h"
#include "GarbledBF.h"
#include "AESRandom.h"
#include "RandomSource.h"
#include "Util.h"
#include "BuilderTasks.h"

/*
 find out how many cores on the computer
 */
//int32_t num_cores();

/*
 Encode a set into a bloom filter.
 filter: an initialised bloom filter,
 param: bloom filter parameters;
 hashKeys: keys used by range hashes, must have hashNum=k keys 
 hashNum: see hashKeys
 keyLen: the byte length of the keys. must be 10,16,24,32 bytes
 input: the input set, each element is a byte array. The length of each element is given by eLen, the total number of elements is given by inputLen.
 eLen: see input
inputLen: see input
 indexOut: a 2-d array for output indexes produced. Size of the first dimension is inputLen, size of the second dimension is hashNum.First dimension (indexOut[i]) must be initilised to NULL.
 */
int MTBuilder_BF(BF* filter, BFParameters *param,uint8_t** hashKeys,int32_t hashNum, int32_t keyLen,uint8_t** input, int32_t eLen, int32_t inputLen,int32_t** indexOut);

//MTBuilder_GBF();

int MTBuilder_GBF(GarbledBF*filter, BFParameters * param,uint8_t** hashKeys,int32_t hashNum, int32_t keyLen,uint8_t** input, int32_t eLen, int32_t inputLen,uint8_t** eHash, int32_t rndSeedLen);

/*
 enqueue hashThreads RangeHash arrays, each has hashNum rangehash functions (initialised with a key) 
 */
//static void initialiseHashQue(BFParameters *param, Queue* hashQueue, uint8_t** hashKeys,int32_t keyLen,int32_t hashNum,int32_t hashThreads);
//
static void* startCollectingIndexes(void* args);

#endif
