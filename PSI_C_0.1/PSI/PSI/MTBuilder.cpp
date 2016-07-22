//
//  MTBuilder.c
//  PSI
//
//  Created by Changyu Dong on 04/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include "MTBuilder.h"

#define bunch 100000

typedef struct indexArgs{
    GarbledBF* filter;
    int32_t** indexes;
    int32_t inputNum;
    int32_t hashNum;
    uint8_t** eHash;
    RandomSource* rndSrc;
}indexArgs;



int MTBuilder_GBF(GarbledBF*filter, BFParameters * param,uint8_t** hashKeys,int32_t hashNum, int32_t keyLen,uint8_t** input, int32_t eLen, int32_t inputLen,uint8_t** eHash,int32_t rndSeedLen){
    
    assert(hashNum==param->k);
    
    // how many cores
    int32_t cores= num_cores();

    
    //create an empty random source
    int32_t blockSize = GBFSigmaByteLen;
    int32_t totalBlocks = param->m-inputLen;
    int32_t numOfRows = bunch;
    int32_t rowLength= (totalBlocks/numOfRows+1)*blockSize;

    RandomSource *rndSrc;    
    RandomSource_Create(&rndSrc, numOfRows, blockSize);
    
    //indexes array
    int32_t** indexes;
    indexes = (int32_t**)calloc(inputLen, sizeof(int32_t*));
    assert(indexes!=NULL);
    
    // prepare for the thread that adds element to the GBF
    indexArgs* iArgs;
    iArgs= (indexArgs*)malloc(sizeof(indexArgs));
    assert(iArgs!=NULL);
    
    iArgs->filter=filter;
    iArgs->indexes=indexes;
    iArgs->inputNum=inputLen;
    iArgs->hashNum=hashNum;
    iArgs->eHash=eHash;
    iArgs->rndSrc=rndSrc;
    
    pthread_t buildThread;
    pthread_create(&buildThread, NULL, &startCollectingIndexes, iArgs);
    //set to max priority
    int32_t policy;
    struct sched_param sch_param;
    
    pthread_getschedparam(buildThread, &policy, &sch_param);
    sch_param.sched_priority = sched_get_priority_max(policy);
    pthread_setschedparam(buildThread, policy, &sch_param);
    
    //create thread for index and random gnereation
    pthread_t* idxThreads;
    idxThreads=(pthread_t*)malloc(sizeof(pthread_t)*cores);
    assert(idxThreads!=NULL);
    
    pthread_t* rndThreads;
    rndThreads=(pthread_t*)malloc(sizeof(pthread_t)*cores);
    assert(rndThreads!=NULL);
    
    for(int32_t i=0;i<cores;i++){
        GBFIdxProducerArgs* args;
        args=(GBFIdxProducerArgs*)malloc(sizeof(GBFIdxProducerArgs));
        assert(args!=NULL);
        args->dataLen=eLen;
        args->hashKeys=hashKeys;
        args->hashNum=hashNum;
        args->i=i;
        args->inputs=input;
        args->keyLen=keyLen;
        args->m=param->m;
        args->max=inputLen;
        args->out=indexes;
        args->threadNum=cores;
        pthread_create(&idxThreads[i], NULL, &GBFIndexProducer, args);
        
        rndProducerArgs* rndArgs;
        rndArgs=(rndProducerArgs*)malloc(sizeof(rndProducerArgs));
        assert(rndArgs!=NULL);
        rndArgs->i=i;
        rndArgs->rnd=rndSrc;
        rndArgs->rowLen=rowLength;
        rndArgs->seedLen=rndSeedLen;
        rndArgs->threadNum=cores;
        
        pthread_create(&rndThreads[i], NULL, &GBFRndProducer, rndArgs);
    }
    
//    for(int i=0;i<cores;i++){
//        pthread_join(idxThreads[i], NULL);
//        pthread_join(rndThreads[i], NULL);
//    }
    
    //wait to finish
    pthread_join(buildThread, NULL);
    
    for(int i=0;i<inputLen;i++){
        free(indexes[i]);
    }
    free(indexes);
    free(idxThreads);
    free(rndThreads);
    
    return 1;
    
}

int MTBuilder_BF(BF* filter, BFParameters *param,uint8_t** hashKeys,int32_t hashNum, int32_t keyLen,uint8_t** input, int32_t eLen, int32_t inputLen,int32_t** indexOut){
    assert(hashNum==param->k);

    // how many cores
    int32_t cores= num_cores();
    
    pthread_t* threads;
    
    threads=(pthread_t*)malloc(sizeof(pthread_t)*cores);
    for(int i=0;i<cores;i++){
        BFProducerArgs* args;
        args=(BFProducerArgs*)malloc(sizeof(BFProducerArgs));
        assert(args!=NULL);
        
        args->dataLen=eLen;
        args->filter=filter;
        args->hashKeys=hashKeys;
        args->hashNum=hashNum;
        args->i=i;
        args->inputs=input;
        args->keyLen=keyLen;
        args->m=param->m;
        args->max=inputLen;
        args->out=indexOut;
        args->threadNum=cores;
        
        pthread_create(&threads[i], NULL, &BFProducer, args);
    }

    
    for(int i=0;i<cores;i++){
        pthread_join(threads[i], NULL);
    }
    
    free(threads);
    
    
    
//    //create a threadPool
//    
//    threadpool_t* hashPool = threadpool_create(cores, inputLen, 0);
//    
//    //create a queue of rangeHashfunction[]
//    Queue* hashQueue;
//    
//    Queue_Create(&hashQueue);
//    
//    initialiseHashQue(param, hashQueue, hashKeys, keyLen, hashNum, cores);
//    
//    indexArgs* myArgs;
//    
//    myArgs= malloc(sizeof(indexArgs));
//    
//    myArgs->filter=filter;
//    myArgs->hashNum=hashNum;
//    myArgs->indexes=indexOut;
//    myArgs->inputNum=inputLen;
//    
//    pthread_t colThread;
//    pthread_create(&colThread, NULL, &startCollectingIndexes, myArgs);
//    
//    t2=clock();
//    
//    msec = ((double) (t2 - t1)*1000) / CLOCKS_PER_SEC;
//    printf("Before loop time used (ms): %f\n", msec);
//    
//    t1=clock();
//    for(int32_t i=0;i<inputLen;i+=bunch){
//        // args will be freed in the thread
//        hashProducerArgs* args;
//        args=malloc(sizeof(hashProducerArgs));
//        assert(args!=NULL);
//    
//        args->inputs=input;
//        args->dataLen=eLen;
//        args->hashNum=hashNum;
//        args->hashQueue=hashQueue;
//        args->numToProduce=bunch;
//        args->max=inputLen;
//        args->i=i;
//        args->out=indexOut;
//        //printf("%d\n",i);
//        
//        //enqueue tasks
//        //thr_pool_queue(hashPool, &HashProducer, args);
//        threadpool_add(hashPool, &HashProducer, args, 0);
//    }
//    t2=clock();
//    
//    msec = ((double) (t2 - t1)*1000) / CLOCKS_PER_SEC;
//    printf("after loop time used (ms): %f\n", msec);
//    
////    //start construct BF
////    
////    for(int32_t i=0;i<inputLen;i++){
////        //wait
////        while(indexOut[i]==NULL){
////            usleep(1);
////        }
////        
////        BF_AddMT(filter, indexOut[i], hashNum);
////        
////    }
//    //BF_Print(filter);

    //clean the queue before destroy it
//    printf("start clean up\n");
//    Node* n=Queue_get(hashQueue);
//    while(n->payload!=NULL){
//        for(int i=0;i<hashNum;i++){
//            RangeHash_Destroy(((RangeHash**)n->payload)[i]);
//        }
//        free(n->payload);
//        n->payload=NULL;
//        Queue_Put(hashQueue, n);
//        n=Queue_get(hashQueue);
//    }
//    
//    Queue_Destroy(hashQueue);
//    //thr_pool_destroy(hashPool);
//    threadpool_destroy(hashPool, 0);
    
    return 1;
}


static void* startCollectingIndexes(void* args){
    indexArgs* myArgs=(indexArgs*)args;
    for(int i=0;i<myArgs->inputNum;i++){
        while(myArgs->indexes[i]==NULL){
            usleep(1);
        }
        GBF_addMT(myArgs->filter, myArgs->indexes[i], myArgs->hashNum, myArgs->eHash[i], myArgs->rndSrc);
        //BF_AddMT(myArgs->filter, myArgs->indexes[i], myArgs->hashNum);
    }
    GBF_doFinalMT(myArgs->filter, myArgs->rndSrc);
    free(myArgs);
    return NULL;
}
//
//static void initialiseHashQue(BFParameters *param, Queue* hashQueue, uint8_t** hashKeys,int32_t keyLen,int32_t hashNum,int32_t hashThreads){
//    for(int32_t i=0;i<hashThreads;i++){
//        RangeHash** hashes;
//        hashes = malloc(sizeof(RangeHash*)*hashNum);
//        assert(hashes!=NULL);
//        for(int32_t j=0;j<hashNum;j++){
//            assert(RangeHash_Create(&hashes[j], hashKeys[j], keyLen, param->m)==1);
//        }
//        Node * n;
//        assert(Node_Create(&n, hashes)==1);
//        Queue_Put(hashQueue, n);
//    }

    
//}