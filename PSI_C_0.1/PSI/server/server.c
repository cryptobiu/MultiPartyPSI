//
//  server.c
//  PSI
//
//  Created by Changyu Dong on 12/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>

#include "server.h"

void generateHashKeys(Server* sver){
    AESRandom* rnd;
    AESRandom_Create(&rnd, 16);
    sver->keys=calloc(sver->bfParam->k, sizeof(uint8_t*));
    int32_t keyLen=sver->secLev/8;
    for(int i=0;i<sver->bfParam->k;i++){
        uint8_t* e= calloc(keyLen, sizeof(uint8_t));
        AESRandom_NextBytes(rnd, e, keyLen);
        sver->keys[i]=e;
    }
    AESRandom_Destroy(rnd);
}

void buildGBF(Server* sver){
    sver->hashFuncs= calloc(sver->bfParam->k, sizeof(RangeHash*));
    for(int i=0;i<sver->bfParam->k;i++){
        RangeHash_Create(&sver->hashFuncs[i], sver->keys[i], sver->secLev/8, sver->bfParam->m);
    }
    AESRandom* rnd;
    
    AESRandom_Create(&rnd, sver->secLev/8);
    for(int i=0;i<sver->n;i++){
        GBF_add(sver->filter, sver->hashFuncs, sver->bfParam->k, sver->set[i], defaultDatalen, sver->eHash[i], rnd);
        
    }
    //GBF_doFinal(sver->filter, rnd);
    AESRandom_Destroy(rnd);
}


void multiPartST(Server* sver,int32_t round){
    printf("round %d\n",round);
   //printf("startbit: %d\n",sver->startBit);
    //printf("endbit: %d\n", sver->endBit);
   // printf("stepbit: %d\n",sver->stepBit);
    
    uint8_t** data=receiveArrayOfByteStrings(sver->socket, sver->secLev, sver->stepBit/8);
    
    //char path [64];
    //snprintf(path, 64, "received%d",round);
    
    //write2DByteArrayToFile(path, data, sver->secLev, sver->stepBit/8);
    
    
    BitMatrix* Q =OTExtSndr_step3_2(sver->sndr, data);
   // snprintf(path, 64, "Q%d",round);
    //write2DByteArrayToFile(path, Q->data, sver->secLev, sver->stepBit/8);
    

    printf("bit matrix Q built\n");
    

    //snprintf(path, 64, "GBF%d",round);
    
    //write2DByteArrayToFile(path, sver->sndr->toSend, sver->stepBit, GBFSigmaByteLen);
    

    uint8_t** shared = calloc(sver->stepBit, sizeof(uint8_t*));
    
    Server_outputThreadArgs* args= calloc(1, sizeof(Server_outputThreadArgs));
    args->shared=shared;
    args->size=sver->stepBit;
    args->socket=sver->socket;
    
    pthread_t outputT;
    pthread_create(&outputT, NULL, &Server_outputThread, args);
    
    MessageDigest* md= getHashInstance(sver->secLev);
    
    for(int i=0;i<sver->stepBit;i++){
        uint8_t* ba =OTExtSndr_step3_3Single(sver->sndr, Q, i, md);
        shared[i]=ba;
    }
    printf("Wait for join\n");
    
    pthread_join(outputT, NULL);
    
    printf("GBF sent\n");
    

    free(Q->data[0]);
    BM_Destroy(Q);
    free(data);
    free(shared);
    
    MD_Destroy(md);

    //snprintf(path, 64, "GBFSend%d",round);

     //   write2DByteArrayToFile(path, shared, sver->stepBit, GBFSigmaByteLen);
    
}

void onePartST(Server* sver){

    uint8_t** data =receiveArrayOfByteStrings(sver->socket, sver->secLev, getByteLenByBitLen(sver->stepBit));
    
    BitMatrix* Q= OTExtSndr_step3_2(sver->sndr, data);
    

    printf("bit matrix Q built\n");
    
    

    uint8_t** shared = calloc(sver->bfParam->m, sizeof(uint8_t*));
    
    Server_outputThreadArgs* args= calloc(1, sizeof(Server_outputThreadArgs));
    args->shared=shared;
    args->size=sver->bfParam->m;
    args->socket=sver->socket;
    
    pthread_t outputT;
    pthread_create(&outputT, NULL, &Server_outputThread, args);
    
    
    
    
    MessageDigest* md= getHashInstance(sver->secLev);
    
    for(int i=0;i<sver->bfParam->m;i++){
        uint8_t* ba =OTExtSndr_step3_3Single(sver->sndr, Q, i, md);
        shared[i]=ba;
    }
    printf("Wait for join\n");
    
    pthread_join(outputT, NULL);


    printf("GBF sent\n");

}

void Server_runProtocolSingle(Server* sver){

    
    printf("Start protocol in single thread mode.\n");
    
    generateHashKeys(sver);
    
    sendArrayOfByteStrings(sver->socket, sver->bfParam->k, sver->secLev/8, sver->keys);
    
    printf("Hash keys sent.\n");
    
    
    buildGBF(sver);
    
    //print2DBytes(sver->filter->data, sver->filter->m, GBFSigmaByteLen);
    

    printf("Garbled Bloom filter ready\n");
    
    
    EC_POINT** cs =receiveArrayOfECPoints(sver->socket, sver->secLev, sver->sndr->baseOTRcvr->pointByteLen, sver->sndr->baseOTRcvr->curve);
    
    printf("Cs received.\n");
    
    EC_POINT** pk0s =OTExtSndr_step1(sver->sndr, cs, sver->secLev);
    
    sendArrayOfECPoints(sver->socket, sver->secLev,sver->sndr->baseOTRcvr->pointByteLen, sver->sndr->baseOTRcvr->curve, pk0s);

    printf("pk0s sent\n");
    

    uint8_t*** received = receive3DBitArray(sver->socket, sver->secLev, 2, sver->sndr->seedByteLen);
    
    EC_POINT** grs= receiveArrayOfECPoints(sver->socket, sver->secLev, sver->sndr->baseOTRcvr->pointByteLen, sver->sndr->baseOTRcvr->curve);
    
    uint8_t** seeds =OTExtSndr_step3_1(sver->sndr, received, sver->sndr->seedByteLen, grs);
    printf("seeds received\n");
    
    if(sver->partialTransfer){
        uint8_t** data= sver->filter->data;
        //print2DBytes(data, sver->stepBit, GBFSigmaByteLen);
        for(int j=0;j<sver->parts;j++){
            //uint8_t** toSend=calloc(sver->stepBit, sizeof(uint8_t*));
            //for(int i=0;i<sver->stepBit;i++){
            //    toSend[i]=data[sver->startBit+i];
           // }
            uint8_t** toSend=data+sver->startBit;
            //print2DBytes(toSend, sver->stepBit, GBFSigmaByteLen);
            OTExtSndr_ReInitialise(sver->sndr, toSend, sver->stepBit);
            multiPartST(sver,j);
            sver->startBit=sver->endBit;
            sver->endBit=sver->startBit+sver->stepBit;
            if(sver->endBit>sver->bfParam->m){
                sver->endBit=sver->bfParam->m;
            }
            sver->stepBit=sver->endBit-sver->startBit;
            
        }
        
    }else{
        onePartST(sver);
    }
    
    
    if(sver->verify){
        sendArrayOfByteStrings(sver->socket, sver->n, defaultDatalen, sver->set);
    }
}

void multiPart(Server* sver, int32_t round,pthread_t* GBFbuilT){
    printf("start round %d\n",round);

    //printf("To receive %d colomes\n",sver->secLev);
    //printf("Each colume is %d bytes \n",getByteLenByBitLen(sver->stepBit));
    //printf("strpbit is %d \n",sver->stepBit);
    uint8_t** data =receiveArrayOfByteStrings(sver->socket, sver->secLev, getByteLenByBitLen(sver->stepBit));
    
    //write2DByteArrayToFile("recved", data, sver->secLev, getByteLenByBitLen(sver->stepBit));
    
    // write2DByteArrayToFile("GBF", sver->filter->data,sver->bfParam->m,GBFSigmaByteLen);
    
    BitMatrix* Q;
    BM_Create(&Q, sver->stepBit, sver->secLev);
    
    //pthread_t Ts[sver->secLev];
    pthread_t* Ts=calloc(sver->threads, sizeof(pthread_t));
    for(int32_t i=0;i<sver->threads;i++){
        Server_step3_2TaskArgs* args= malloc(sizeof(Server_step3_2TaskArgs));
        args->i=i;
        args->input=data;
        args->out=Q;
        args->sndr=sver->sndr;
        args->trdNum=sver->threads;
        
        pthread_create(&Ts[i], NULL, &Server_step3_2Task, args);
        
    }
    
    for(int i=0;i<sver->threads;i++){
        pthread_join(Ts[i], NULL);
    }
    
    printf("bit matrix Q built\n");
    
    pthread_join(*GBFbuilT, NULL);
    

    uint8_t** shared = calloc(sver->stepBit, sizeof(uint8_t*));
    
    Server_outputThreadArgs* args= calloc(1, sizeof(Server_outputThreadArgs));
    args->shared=shared;
    args->size=sver->stepBit;
    args->socket=sver->socket;

    for(int32_t i=0;i<sver->threads-1;i++){
        Server_step3_3TaskArgs* args= malloc(sizeof(Server_step3_3TaskArgs));
        args->i=i;
        args->Q=Q;
        args->shared=shared;
        args->sndr=sver->sndr;
        args->trdNum=sver->threads-1;
        pthread_create(&Ts[i], NULL, &Server_step3_3Task, args);
    }

    Server_outputThread(args);
    

    printf("GBF sent\n");
    
    for(int i=0;i<sver->secLev;i++){
        free(Q->data[i]);
    }
    BM_Destroy(Q);
    free(data);
    free(shared);
    free(Ts);
    
}

void onePart(Server* sver,pthread_t* GBFbuilT){

    uint8_t** data =receiveArrayOfByteStrings(sver->socket, sver->secLev, getByteLenByBitLen(sver->bfParam->m));
    
    //write2DByteArrayToFile("recved", data, sver->secLev, getByteLenByBitLen(sver->stepBit));
    
   // write2DByteArrayToFile("GBF", sver->filter->data,sver->bfParam->m,GBFSigmaByteLen);
    
    BitMatrix* Q;
    BM_Create(&Q, sver->bfParam->m, sver->secLev);
    
    //pthread_t Ts[sver->secLev];
    pthread_t* Ts=calloc(sver->threads, sizeof(pthread_t));
    
    for(int32_t i=0;i<sver->threads;i++){
        Server_step3_2TaskArgs* args= malloc(sizeof(Server_step3_2TaskArgs));
        args->i=i;
        args->input=data;
        args->out=Q;
        args->sndr=sver->sndr;
        args->trdNum=sver->threads;
        
        pthread_create(&Ts[i], NULL, &Server_step3_2Task, args);
        
    }
    
    for(int i=0;i<sver->threads;i++){
        pthread_join(Ts[i], NULL);
    }


    printf("bit matrix Q built\n");

    pthread_join(*GBFbuilT, NULL);
    

    uint8_t** shared = calloc(sver->bfParam->m, sizeof(uint8_t*));
    
    Server_outputThreadArgs* args= calloc(1, sizeof(Server_outputThreadArgs));
    args->shared=shared;
    args->size=sver->bfParam->m;
    args->socket=sver->socket;
    
    //pthread_t outputT;
    //pthread_create(&outputT, NULL, &Server_outputThread, args);
    
    
    
    
//    MessageDigest* md= getHashInstance(sver->secLev);
//    
//    for(int i=0;i<sver->bfParam->m;i++){
//        uint8_t* ba =OTExtSndr_step3_3Single(sver->sndr, Q, i, md);
//        shared[i]=ba;
//    }
//    printf("Wait for join\n");
//    
//    pthread_join(outputT, NULL);
//    
//    gettimeofday(&t2, NULL);
//    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
//    printf("GBF sent (ms): %f\n", cpu_time_used);
    
    
    
    

    for(int32_t i=0;i<sver->threads-1;i++){
        Server_step3_3TaskArgs* args= malloc(sizeof(Server_step3_3TaskArgs));
        args->i=i;
        args->Q=Q;
        args->shared=shared;
        args->sndr=sver->sndr;
        args->trdNum=sver->threads-1;
        pthread_create(&Ts[i], NULL, &Server_step3_3Task, args);
    }
    
   // for(int i=0;i<sver->threads;i++){
   //     pthread_join(Ts[i], NULL);
   // }
    //pthread_join(outputT,NULL);
    Server_outputThread(args);
    
    printf("GBF sent (ms)\n");
}

void Server_runProtocolMUlti(Server* sver){
    
    printf("Start protocol in multi-thread mode.\n");
    
    generateHashKeys(sver);
    
    sendArrayOfByteStrings(sver->socket, sver->bfParam->k, sver->secLev/8, sver->keys);
    
    printf("Hash keys sent.\n");
    
   // gettimeofday(&t1, NULL);
    
    //MTBuilder_GBF(sver->filter, sver->bfParam, sver->keys, sver->bfParam->k, sver->secLev/8, sver->set, defaultDatalen, sver->bfParam->n, sver->eHash, sver->secLev/8);
    //printf("GBF m = %d .\n",sver->bfParam->m);
    Server_BuildGBFArgs* gbfargs =malloc(sizeof(Server_BuildGBFArgs));
    gbfargs->filter=sver->filter;
    gbfargs->bfParam=sver->bfParam;
    gbfargs->keys=sver->keys;
    gbfargs->k=sver->bfParam->k;
    gbfargs->keyByteLen=sver->secLev/8;
    gbfargs->set=sver->set;
    gbfargs->eByteLen=defaultDatalen;
    gbfargs->eNum=sver->bfParam->n;
    gbfargs->ehash=sver->eHash;
    gbfargs->rndSeedLen=sver->secLev/8;
    
    pthread_t GBFbuilT;
    
    pthread_create(&GBFbuilT, NULL, &Server_BuildGBF, gbfargs);
    
    //gettimeofday(&t2, NULL);
    //cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("Start building Garbled Bloom filter\n");
    

    
    EC_POINT** cs =receiveArrayOfECPoints(sver->socket, sver->secLev, sver->sndr->baseOTRcvr->pointByteLen, sver->sndr->baseOTRcvr->curve);
    
    printf("Cs received.\n");
    
    EC_POINT** pk0s =calloc(sver->secLev, sizeof(EC_POINT*));
    //pthread_t Ts[sver->secLev];
    pthread_t* Ts= calloc(sver->threads, sizeof(pthread_t));
    
    for(int32_t i=0;i<sver->threads;i++){
        Server_step1TaskArgs * args=malloc(sizeof(Server_step1TaskArgs));
        args->cs=cs;
        args->i=i;
        args->pks=pk0s;
        args->sndr=sver->sndr;
        args->trdNum=sver->threads;
        
        pthread_create(&Ts[i], NULL, &Server_step1Task, args);
        
    }
    
    for(int i=0;i<sver->threads;i++){
        pthread_join(Ts[i], NULL);
    }
    
    sendArrayOfECPoints(sver->socket, sver->secLev,sver->sndr->baseOTRcvr->pointByteLen, sver->sndr->baseOTRcvr->curve, pk0s);

    printf("pk0s sent\n");
    
    uint8_t*** received = receive3DBitArray(sver->socket, sver->secLev, 2, sver->sndr->seedByteLen);
    
    EC_POINT** grs= receiveArrayOfECPoints(sver->socket, sver->secLev, sver->sndr->baseOTRcvr->pointByteLen, sver->sndr->baseOTRcvr->curve);
    
    uint8_t** seeds=calloc(sver->secLev, sizeof(uint8_t*));
    for(int32_t i=0;i<sver->threads;i++){
        Server_step3_1TaskArgs * args=malloc(sizeof(Server_step3_1TaskArgs));
        args->grs=grs;
        args->i=i;
        args->received=received;
        args->result=seeds;
        args->sndr=sver->sndr;
        args->trdNum=sver->threads;
        
        pthread_create(&Ts[i], NULL, &Server_step3_1Task, args);
        
    }
    
    for(int i=0;i<sver->threads;i++){
        pthread_join(Ts[i], NULL);
    }
    

    printf("seeds received\n");
    
    if(sver->partialTransfer){
        uint8_t** data= sver->filter->data;
        //print2DBytes(data, sver->stepBit, GBFSigmaByteLen);
        for(int j=0;j<sver->parts;j++){
            //uint8_t** toSend=calloc(sver->stepBit, sizeof(uint8_t*));
            //for(int i=0;i<sver->stepBit;i++){
            //    toSend[i]=data[sver->startBit+i];
            // }
            uint8_t** toSend=data+sver->startBit;
            //print2DBytes(toSend, sver->stepBit, GBFSigmaByteLen);
            OTExtSndr_ReInitialise(sver->sndr, toSend, sver->stepBit);
            multiPart(sver,j,&GBFbuilT);
            sver->startBit=sver->endBit;
            sver->endBit=sver->startBit+sver->stepBit;
            if(sver->endBit>sver->bfParam->m){
                sver->endBit=sver->bfParam->m;
            }
            sver->stepBit=sver->endBit-sver->startBit;
            
        }
        
    }else{
        onePart(sver,&GBFbuilT);
    }
    
    
    if(sver->verify){
        sendArrayOfByteStrings(sver->socket, sver->n, defaultDatalen, sver->set);
    }
    
}