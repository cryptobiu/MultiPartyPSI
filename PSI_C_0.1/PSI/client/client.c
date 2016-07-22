//
//  client.c
//  PSI
//
//  Created by Changyu Dong on 12/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>

#include "client.h"
void buildBF(Client* clnt){
    RangeHash** hashFuncs = calloc(clnt->bfParam->k, sizeof(RangeHash*));
    int32_t keyLen= clnt->secLev/8;
    
    for(int i=0;i<clnt->bfParam->k;i++){
        RangeHash_Create(&hashFuncs[i], clnt->keys[i], keyLen, clnt->bfParam->m);
    }
    
    BF_Create(&clnt->filter,clnt->bfParam->m);
    for(int i=0;i<clnt->n;i++){
        for(int j=0;j<clnt->bfParam->k;j++){
            clnt->indexes[i][j]=RangeHash_Digest(hashFuncs[j], clnt->set[i],defaultDatalen);
            if(exists(clnt->indexes[i][j], clnt->indexes[i], j)){
                clnt->indexes[i][j]=-1;
            }else{
                BF_SetBit(clnt->filter, clnt->indexes[i][j]);
            }
        }
    }
    
}

void multiPartST(Client* clnt,uint8_t** gbpPart,int32_t round){
    printf("round %d\n",round);
    //printf("startbit: %d\n",clnt->startBit);
   // printf("endbit: %d\n", clnt->endBit);
   // printf("stepbit: %d\n",clnt->stepBit);
    //printBytes(clnt->rcvr->str, clnt->stepByte);
    
    //char path[64];

    
    //snprintf(path, 64, "T%d",round);
    //write2DByteArrayToFile(path, clnt->rcvr->T->data, clnt->secLev, clnt->rcvr->T->byteLenCol);
    
    uint8_t** matrix = OTExtRcvr_step2_2(clnt->rcvr);
    
    //snprintf(path, 64, "encrypted%d",round);
    //write2DByteArrayToFile(path, clnt->rcvr->T->data, clnt->secLev, clnt->rcvr->T->byteLenCol);
    
    sendArrayOfByteStrings(clnt->socket, clnt->secLev, clnt->rcvr->T->byteLenCol, matrix);

    printf("Bitmtrix sent\n");
    

    OTExtRcvr_step4_1(clnt->rcvr);
    
    //snprintf(path, 64, "decrypted%d",round);
    //write2DByteArrayToFile(path, clnt->rcvr->T->data, clnt->secLev, clnt->rcvr->T->byteLenCol);
    

    printf("Bitmtrix decrypted\n");
    
    //uint8_t** shared = calloc(clnt->stepBit, sizeof(uint8_t*));
    uint8_t** shared=gbpPart;
    Client_inputThreadArgs* args= calloc(1, sizeof(Client_inputThreadArgs));
    args->shared=shared;
    args->socket=clnt->socket;
    args->size=clnt->stepBit;
    
    pthread_t inputT;
    
    pthread_create(&inputT, NULL, &Client_inputThread, args);
    
    MessageDigest* H = getHashInstance(clnt->secLev);
    
    for(int i=0;i<clnt->stepBit;i++){
        while(shared[i]==NULL){
            usleep(1);
        }
        
        if(getBit(clnt->rcvr->str, i,clnt->rcvr->leadingZeroes)){
            OTExtRcvr_step4_2Single(clnt->rcvr, shared[i], GBFSigmaByteLen, i, H);
        }else{
            free(shared[i]);
        }
        
        //gbpPart[i]=shared[i];
    }
    
    //snprintf(path, 64, "gbfdecrypted%d",round);
    //write2DByteArrayToFile(path, gbpPart, clnt->stepBit, GBFSigmaByteLen);
    
    printf("Intersection GBF part %d created\n", round);
    
    //free(shared);
    MD_Destroy(H);
}

void onePartST(Client* clnt){
    
    uint8_t** matrix = OTExtRcvr_step2_2(clnt->rcvr);
    
    sendArrayOfByteStrings(clnt->socket, clnt->secLev, clnt->rcvr->T->byteLenCol, matrix);
    printf("Bitmtrix sent\n");
    

    OTExtRcvr_step4_1(clnt->rcvr);
    printf("Bitmtrix decrypted\n");
    
    uint8_t** shared = calloc(clnt->bfParam->m, sizeof(uint8_t*));
    
    Client_inputThreadArgs* args= calloc(1, sizeof(Client_inputThreadArgs));
    args->shared=shared;
    args->socket=clnt->socket;
    args->size=clnt->bfParam->m;
    
    pthread_t inputT;
    
    pthread_create(&inputT, NULL, &Client_inputThread, args);
    
    MessageDigest* H = getHashInstance(clnt->secLev);
    
    for(int i=0;i<clnt->bfParam->m;i++){
        while(shared[i]==NULL){
            usleep(1);
        }
        
        if(BF_GetBit(clnt->filter, i)){
            OTExtRcvr_step4_2Single(clnt->rcvr, shared[i], GBFSigmaByteLen, i, H);
        }
        
        GBF_set(clnt->recFilter, i, shared[i]);
    }
    printf("Intersection GBF created\n");
    
}



void Client_runProtocolSingle(Client* clnt){
    printf("Start protocol in single thread mode.\n");
    
    
    clnt->keys= receiveArrayOfByteStrings(clnt->socket, clnt->bfParam->k, clnt->secLev/8);
    
    printf("Hash keys received .\n");
    

    
    buildBF(clnt);
    printf("Bloom filter ready\n");
    
    
    EC_POINT** cs = OTExtRcvr_step0(clnt->rcvr);
    
    sendArrayOfECPoints(clnt->socket, clnt->secLev, clnt->rcvr->baseOTSndr->pointByteLen, clnt->rcvr->baseOTSndr->curve, cs);

    printf("CS sent\n");

    EC_POINT** pk0s = receiveArrayOfECPoints(clnt->socket, clnt->secLev, clnt->rcvr->baseOTSndr->pointByteLen, clnt->rcvr->baseOTSndr->curve);

    printf("PK0s received\n");
    
    
    uint8_t*** seeds= calloc(clnt->secLev, sizeof(uint8_t**));
    for(int i=0;i<clnt->secLev;i++){
        seeds[i]= calloc(2, sizeof(uint8_t*));
    }
    
    EC_POINT** grs = OTExtRcvr_step2_1(clnt->rcvr, pk0s, seeds);
    
    send3DBitArray(clnt->socket, clnt->secLev, 2, clnt->rcvr->seedByteLen, seeds);
    
    sendArrayOfECPoints(clnt->socket, clnt->secLev, clnt->rcvr->baseOTSndr->pointByteLen, clnt->rcvr->baseOTSndr->curve, grs);
    
    printf("Encrypted seeds sent\n");
    
    if(clnt->partialTrafer==1){
        uint8_t** gbfData=clnt->recFilter->data;
        int32_t selectionLen=clnt->endByte-clnt->startByte;
        uint8_t* selection = calloc(selectionLen, sizeof(uint8_t));
        
        memcpy(selection, clnt->filter->data+clnt->startByte, selectionLen);
        
        clnt->rcvr->str=selection;
        
        for(int i=0;i<clnt->parts;i++){
            //uint8_t** gbpPart = calloc(clnt->stepBit, sizeof(uint8_t*));
            uint8_t** gbpPart =gbfData+clnt->startBit;
            multiPartST(clnt,gbpPart,i);
            
            //for(int j= clnt->startBit;j<clnt->endBit;j++){
            //    gbfData[j]=gbpPart[j-clnt->startBit];
            //}
            
            clnt->startBit=clnt->endBit;
            clnt->startByte=clnt->endByte;
            clnt->endBit=clnt->startBit+clnt->stepBit;
            
            if(clnt->endBit>clnt->bfParam->m){
                clnt->endBit=clnt->bfParam->m;
            }
            clnt->endByte=clnt->endBit/8;
            clnt->stepBit=clnt->endBit-clnt->startBit;
            clnt->stepByte=clnt->endByte-clnt->startByte;
            
            free(clnt->rcvr->str);
            clnt->rcvr->str=NULL;
            
            selectionLen=clnt->endByte-clnt->startByte;
            selection = calloc(selectionLen, sizeof(uint8_t));
            
            memcpy(selection, clnt->filter->data+clnt->startByte, selectionLen);
            
            if(clnt->stepBit>0){
                OTExtRcvr_ReInitialise(clnt->rcvr, clnt->stepBit, selection, selectionLen);
            }
            
        }
        
    }else{
        clnt->rcvr->str=clnt->filter->data;
        onePartST(clnt);
    }
    
 
    List L;
    
    list_init(&L, &free);
    
    for(int i=0;i<clnt->n;i++){
        if(GBF_query_With_Indexes(clnt->recFilter, clnt->indexes[i], clnt->bfParam->k, clnt->eHash[i])){
            list_ins_next(&L, NULL, clnt->set[i]);
        }
    }

    printf("Intersection computed\n");
    
    
    if(clnt->verify){
        
        uint8_t** recved= receiveArrayOfByteStrings(clnt->socket, clnt->n, defaultDatalen);

        printf("Sets received for verification\n");
        
        verifyInterSection(clnt->set,recved,clnt->n, &L);
    }
    
}

void multiPart(Client* clnt,uint8_t** gbpPart,int32_t round){
    printf("round %d\n",round);
    //printBytes(clnt->filter->data, clnt->filter->byteLen);

    //pthread_t Ts[clnt->secLev];
    pthread_t * Ts =calloc(clnt->threads, sizeof(pthread_t));
    //write2DByteArrayToFile("T", clnt->rcvr->T->data, clnt->rcvr->T->k, clnt->rcvr->T->byteLenCol);
    
    for(int32_t i=0;i<clnt->threads;i++){
        Client_Step2_2TaskArgs* args= calloc(1, sizeof(Client_Step2_2TaskArgs));
        args->i=i;
        args->rcvr=clnt->rcvr;
        args->thrNum=clnt->threads;
        args->toSend=NULL;
        
        pthread_create(&Ts[i], NULL, &Client_Step2_2Task, args);
    }
    
    for(int i=0;i<clnt->threads;i++){
        pthread_join(Ts[i], NULL);
    }
    
    uint8_t** matrix = clnt->rcvr->T->data;
    
    //write2DByteArrayToFile("encrypted", matrix, clnt->rcvr->T->k, clnt->rcvr->T->byteLenCol);
    

    //printf("To send %d colomes\n",clnt->secLev);
    //printf("Each colume is %d bytes \n",clnt->rcvr->T->byteLenCol);
    //printf("strpbit is %d \n",clnt->stepBit);
    sendArrayOfByteStrings(clnt->socket, clnt->secLev, clnt->rcvr->T->byteLenCol, matrix);
    printf("Bitmtrix sent\n");
    
    for(int32_t i=0;i<clnt->threads;i++){
        Client_Step4_1TaskArgs* args= malloc(sizeof(Client_Step4_1TaskArgs));
        
        args->i=i;
        args->rcvr=clnt->rcvr;
        args->thrNum=clnt->threads;
        
        pthread_create(&Ts[i], NULL, &Client_Step4_1Task, args);
    }
    
    for(int i=0;i<clnt->threads;i++){
        pthread_join(Ts[i], NULL);
    }

    printf("Bitmtrix decrypted\n");
    
 
    uint8_t** shared= gbpPart;
    
    Client_inputThreadArgs* args= calloc(1, sizeof(Client_inputThreadArgs));
    args->shared=shared;
    args->socket=clnt->socket;
    args->size=clnt->stepBit;
    
    //pthread_t inputT;
    
    //pthread_create(&inputT, NULL, &Client_inputThread, args);
    
    
    for(int32_t i=0;i<clnt->threads-1;i++){
        Client_Step4_2TaskArgs* args=malloc(sizeof(Client_Step4_2TaskArgs));
        
        args->filter=clnt->filter;
        args->i=i;
        args->rcvr=clnt->rcvr;
        args->shared=shared;
        args->thrNum=clnt->threads-1;
        args->partial=clnt->partialTrafer;
        
        pthread_create(&Ts[i], NULL, &Client_Step4_2Task, args);
    }
    Client_inputThread(args);
    for(int i=0;i<clnt->threads;i++){
        pthread_join(Ts[i], NULL);
    }
    
    //write2DByteArrayToFile("recvGBF", clnt->recFilter->data,clnt->bfParam->m,GBFSigmaByteLen);
    

    printf("Intersection GBF created\n");
}

void onePart(Client* clnt){
    //printBytes(clnt->filter->data, clnt->filter->byteLen);

    //pthread_t Ts[clnt->secLev];
    pthread_t * Ts =calloc(clnt->threads, sizeof(pthread_t));
    //write2DByteArrayToFile("T", clnt->rcvr->T->data, clnt->rcvr->T->k, clnt->rcvr->T->byteLenCol);
    
    for(int32_t i=0;i<clnt->threads;i++){
        Client_Step2_2TaskArgs* args= calloc(1, sizeof(Client_Step2_2TaskArgs));
        args->i=i;
        args->rcvr=clnt->rcvr;
        args->thrNum=clnt->threads;
        args->toSend=NULL;
        
        pthread_create(&Ts[i], NULL, &Client_Step2_2Task, args);
    }
    
    for(int i=0;i<clnt->threads;i++){
        pthread_join(Ts[i], NULL);
    }
    
    
    
    uint8_t** matrix = clnt->rcvr->T->data;
    
    //write2DByteArrayToFile("encrypted", matrix, clnt->rcvr->T->k, clnt->rcvr->T->byteLenCol);
    
    sendArrayOfByteStrings(clnt->socket, clnt->secLev, clnt->rcvr->T->byteLenCol, matrix);

    printf("Bitmtrix sent\n");
    

    for(int32_t i=0;i<clnt->threads;i++){
        Client_Step4_1TaskArgs* args= malloc(sizeof(Client_Step4_1TaskArgs));
        
        args->i=i;
        args->rcvr=clnt->rcvr;
        args->thrNum=clnt->threads;
        
        pthread_create(&Ts[i], NULL, &Client_Step4_1Task, args);
    }
    
    for(int i=0;i<clnt->threads;i++){
        pthread_join(Ts[i], NULL);
    }

    printf("Bitmtrix decrypted\n");
    
    //write2DByteArrayToFile("decrypted", clnt->rcvr->T->data, clnt->rcvr->T->k, clnt->rcvr->T->byteLenCol);
    
    //uint8_t** shared = calloc(clnt->bfParam->m, sizeof(uint8_t*));
    
    
    
    
    
//    uint8_t** shared = calloc(clnt->bfParam->m, sizeof(uint8_t*));
//    
//    Client_inputThreadArgs* args= calloc(1, sizeof(Client_inputThreadArgs));
//    args->shared=shared;
//    args->socket=clnt->socket;
//    args->size=clnt->bfParam->m;
//    
//    pthread_t inputT;
//    
//    pthread_create(&inputT, NULL, &Client_inputThread, args);
//    
//    MessageDigest* H = getHashInstance(clnt->secLev);
//    
//    for(int i=0;i<clnt->bfParam->m;i++){
//        while(shared[i]==NULL){
//            usleep(1);
//        }
//        
//        if(BF_GetBit(clnt->filter, i)){
//            OTExtRcvr_step4_2Single(clnt->rcvr, shared[i], GBFSigmaByteLen, i, H);
//        }
//        
//        GBF_set(clnt->recFilter, i, shared[i]);
//    }
//    gettimeofday(&t2, NULL);
//    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
//    printf("Intersection GBF created (ms): %f\n", cpu_time_used);
    
    
    
    
    
    
    
    uint8_t** shared= clnt->recFilter->data;
    
    Client_inputThreadArgs* args= calloc(1, sizeof(Client_inputThreadArgs));
    args->shared=shared;
    args->socket=clnt->socket;
    args->size=clnt->bfParam->m;
    
    //pthread_t inputT;
    
    //pthread_create(&inputT, NULL, &Client_inputThread, args);
    
    
    for(int32_t i=0;i<clnt->threads-1;i++){
        Client_Step4_2TaskArgs* args=malloc(sizeof(Client_Step4_2TaskArgs));
        
        args->filter=clnt->filter;
        args->i=i;
        args->rcvr=clnt->rcvr;
        args->shared=shared;
        args->thrNum=clnt->threads-1;
        args->partial=clnt->partialTrafer;
        
        pthread_create(&Ts[i], NULL, &Client_Step4_2Task, args);
    }
    Client_inputThread(args);
    for(int i=0;i<clnt->threads;i++){
        pthread_join(Ts[i], NULL);
    }
    
    //write2DByteArrayToFile("recvGBF", clnt->recFilter->data,clnt->bfParam->m,GBFSigmaByteLen);
    
    printf("Intersection GBF created\n");
    
}



void Client_runProtocolMulti(Client* clnt){
    
    printf("Start protocol in multi-thread mode.\n");
    
    clnt->keys= receiveArrayOfByteStrings(clnt->socket, clnt->bfParam->k, clnt->secLev/8);
    
    printf("Hash keys received.\n");
    //printf("BF m = %d .\n",clnt->bfParam->m);

    
    BF_Create(&clnt->filter, clnt->bfParam->m);
    
    MTBuilder_BF(clnt->filter, clnt->bfParam, clnt->keys, clnt->bfParam->k, clnt->secLev/8, clnt->set, defaultDatalen, clnt->n, clnt->indexes);
    

    printf("Bloom filter ready\n");
    

    
    EC_POINT** cs = OTExtRcvr_step0(clnt->rcvr);
    
    sendArrayOfECPoints(clnt->socket, clnt->secLev, clnt->rcvr->baseOTSndr->pointByteLen, clnt->rcvr->baseOTSndr->curve, cs);
    

    printf("CS sent\n");
    

    EC_POINT** pk0s = receiveArrayOfECPoints(clnt->socket, clnt->secLev, clnt->rcvr->baseOTSndr->pointByteLen, clnt->rcvr->baseOTSndr->curve);

    printf("PK0s received\n");
    
    
    

    uint8_t*** seeds= calloc(clnt->secLev, sizeof(uint8_t**));
    for(int i=0;i<clnt->secLev;i++){
        seeds[i]= calloc(2, sizeof(uint8_t*));
    }
    
    EC_POINT** grs= calloc(clnt->secLev, sizeof(EC_POINT*));
    
    //EC_POINT** grs = OTExtRcvr_step2_1(clnt->rcvr, pk0s, seeds);
    //pthread_t Ts[clnt->secLev];
    
    pthread_t* Ts=calloc(clnt->threads, sizeof(pthread_t));
    
    for(int32_t i=0;i<clnt->threads;i++){
        Client_Step2_1TaskArgs* args= malloc(sizeof(Client_Step2_1TaskArgs));
        
        args->encrypted=seeds;
        args->grs=grs;
        args->i=i;
        args->pk0s=pk0s;
        args->rcvr=clnt->rcvr;
        args->thrNum=clnt->threads;
        
        pthread_create(&Ts[i], NULL, &Client_Step2_1Task, args);
        
    }
    
    for(int i=0;i<clnt->threads;i++){
        pthread_join(Ts[i], NULL);
    }
    
    
    send3DBitArray(clnt->socket, clnt->secLev, 2, clnt->rcvr->seedByteLen, seeds);
    
    sendArrayOfECPoints(clnt->socket, clnt->secLev, clnt->rcvr->baseOTSndr->pointByteLen, clnt->rcvr->baseOTSndr->curve, grs);
    

    printf("Encrypted seeds sent\n");
    
    if(clnt->partialTrafer){
        uint8_t** gbfData=clnt->recFilter->data;
        int32_t selectionLen=clnt->endByte-clnt->startByte;
        uint8_t* selection = calloc(selectionLen, sizeof(uint8_t));
        
        memcpy(selection, clnt->filter->data+clnt->startByte, selectionLen);
        
        clnt->rcvr->str=selection;
        
        for(int i=0;i<clnt->parts;i++){
            //uint8_t** gbpPart = calloc(clnt->stepBit, sizeof(uint8_t*));
            uint8_t** gbpPart =gbfData+clnt->startBit;
            multiPart(clnt,gbpPart,i);
            
            //for(int j= clnt->startBit;j<clnt->endBit;j++){
            //    gbfData[j]=gbpPart[j-clnt->startBit];
            //}
            
            clnt->startBit=clnt->endBit;
            clnt->startByte=clnt->endByte;
            clnt->endBit=clnt->startBit+clnt->stepBit;
            
            if(clnt->endBit>clnt->bfParam->m){
                clnt->endBit=clnt->bfParam->m;
            }
            clnt->endByte=clnt->endBit/8;
            clnt->stepBit=clnt->endBit-clnt->startBit;
            clnt->stepByte=clnt->endByte-clnt->startByte;
            
            free(clnt->rcvr->str);
            clnt->rcvr->str=NULL;
            
            selectionLen=clnt->endByte-clnt->startByte;
            selection = calloc(selectionLen, sizeof(uint8_t));
            
            memcpy(selection, clnt->filter->data+clnt->startByte, selectionLen);
            
            if(clnt->stepBit>0){
                //OTExtRcvr_ReInitialise(clnt->rcvr, clnt->stepBit, selection, selectionLen);
                OTExtRcvr_ReInitialiseMT(clnt->rcvr, clnt->stepBit, selection, selectionLen);
            }
            
        }
        
    }else{
        clnt->rcvr->str=clnt->filter->data;
        onePart(clnt);
        
    }
    
    
    List* Ls= calloc(clnt->threads, sizeof(List));
    
    for(int32_t i=0;i<clnt->threads;i++){
        //List L;
        list_init(&Ls[i], &free);
        //Ls[i]=&L;
        Client_QueryTaskArgs* args= malloc(sizeof(Client_QueryTaskArgs));
        args->eHashes=clnt->eHash;
        args->filter=clnt->recFilter;
        args->n=clnt->n;
        args->hashNum=clnt->bfParam->k;
        args->i=i;
        args->indexes=clnt->indexes;
        args->list=&Ls[i];
        args->set=clnt->set;
        args->thrNum=clnt->threads;
        
        pthread_create(&Ts[i], NULL, &Client_QueryTask, args);
    }
    
    for(int i=0;i<clnt->threads;i++){
        pthread_join(Ts[i], NULL);
    }
    

    printf("Intersection computed\n");
    
    
    if(clnt->verify){
        //
        List L;
        list_init(&L, &free);
        combineList(Ls,&L,clnt->threads);
        
        uint8_t** recved= receiveArrayOfByteStrings(clnt->socket, clnt->n, defaultDatalen);
        
        printf("Sets received for verification\n");
        
        verifyInterSection(clnt->set,recved,clnt->n, &L);
    }
    
//    List L;
//    
//    list_init(&L, &free);
//    
//    for(int i=0;i<clnt->bfParam->n;i++){
//        if(GBF_query_With_Indexes(clnt->recFilter, clnt->indexes[i], clnt->bfParam->k, clnt->eHash[i])){
//            list_ins_next(&L, NULL, clnt->set[i]);
//        }
//    }
//    
//
//    
//    
//    gettimeofday(&t2, NULL);
//    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
//    printf("Intersection computed (ms): %f\n", cpu_time_used);
//    
//    
//    if(clnt->verify){
//        gettimeofday(&t1, NULL);
////        
////        List L;
////        list_init(&L, &free);
////        combineList(Ls,&L,clnt->threads);
//        
//        uint8_t** recved= receiveArrayOfByteStrings(clnt->socket, clnt->bfParam->n, defaultDatalen);
//        
//        gettimeofday(&t2, NULL);
//        cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
//        printf("Sets received for verification (ms): %f\n", cpu_time_used);
//        
//        verifyInterSection(clnt->set,recved,clnt->bfParam->n, &L);
//    }
}