//
//  main.c
//  client
//
//  Created by Changyu Dong on 24/02/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <openssl/ec.h>
#include <openssl/bn.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "BF.h"
#include "RangeHash.h"
#include "AESRandom.h"
#include "Util.h"
#include "GarbledBF.h"
#include "RandomSource.h"
#include "MTBuilder.h"
#include "MessageDigest.h"
#include "NPOTRcvr.h"
#include "NPOTSndr.h"
#include "BitMatrix.h"
#include "OTExtRcvr.h"
#include "OTExtSndr.h"
#include "client.h"
#include "param.h"

Client* initClient(int argc, const char * argv[]);
void start(Client* clnt);

int main(int argc, const char * argv[])
{

    printf("Initialising client\n");
    Client* clnt= initClient(argc, argv);
    struct timeval t1;
    struct timeval t2;
    double cpu_time_used;
    gettimeofday(&t1, NULL);
    start(clnt);
    gettimeofday(&t2, NULL);
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("Total (ms): %f\n", cpu_time_used);
    close(clnt->socket);
    
}

void start(Client* clnt){
    if(clnt->multi==1){
        Client_runProtocolMulti(clnt);
    }else{
        Client_runProtocolSingle(clnt);
    }
}

Client* initClient(int argc, const char * argv[]){
    
    
    char* IP=NULL;
    
    in_port_t port=defaultPort;
    
    uint8_t** input=NULL;
    
    int32_t n=0;
    int32_t k=80;
    
    int32_t multi=1;
    int32_t verify=0;
    int32_t partial=0;
    int32_t parts=1;
    
    for(int i=1;i<argc;i+=2){
        if(strcmp(argv[i], "-a")==0){
            IP=argv[i+1];
        }else if(strcmp(argv[i], "-p")==0){
            port=strtol(argv[i+1],NULL,10);
        }else if(strcmp(argv[i], "-n")==0){
            n=(int32_t)strtol(argv[i+1],NULL,10);
            if(input!=NULL){
                printf("Cannot use -n and -i together.\n");
                abort();
            }else{
                input =randomIntSet(n);
            }
            
        }else if(strcmp(argv[i], "-i")==0){
            if(input!=NULL){
                printf("Cannot use -n and -i together.\n");
                abort();
            }else{
                input=readInputFromFile(argv[i+1],&n);
            }
        }else if(strcmp(argv[i], "-k")==0){
            k=(int32_t)strtol(argv[i+1],NULL,10);
            if(!(k==80||k==128||k==192||k==256)){
                printf("Security level %d is not supported.",k);
                abort();
            }
        }else if(strcmp(argv[i], "-t")==0){
            multi=(int32_t)strtol(argv[i+1],NULL,10);
        }else if (strcmp(argv[i], "-v")==0){
            verify=(int32_t)strtol(argv[i+1],NULL,10);
        }else if(strcmp(argv[i], "-d")==0){
            partial=1;
            parts=(int32_t)strtol(argv[i+1],NULL,10);
            
        }
    }
    
    if (input==NULL){
        n=1024*1024;
        input=randomIntSet(n);
    }
    
    uint8_t** eHash= calloc(n, sizeof(uint8_t*));
    
    MessageDigest* md;
    uint8_t* h;
    if (k==80||k==128){
        MD_Create(&md, 80);
        h= calloc(20, sizeof(uint8_t));
    }
    else{
        MD_Create(&md, 128);
        h= calloc(32, sizeof(uint8_t));
    }
    
    assert(md!=NULL);
    
    for(int i=0;i<n;i++){
        //uint8_t* h= calloc(GBFSigmaByteLen, sizeof(uint8_t));
        MD_Digest(md, input[i], defaultDatalen, h);
        eHash[i]=calloc(GBFSigmaByteLen, sizeof(uint8_t));
        memcpy(eHash[i],h,GBFSigmaByteLen);
    }
    
    Client* clnt=calloc(1, sizeof(Client));
    assert(clnt!=NULL);
    
    BF_GenerateParameters(&clnt->bfParam, n, k);
    
    clnt->secLev=k;
    clnt->set=input;
    clnt->n=n;
    clnt->eHash=eHash;
    GBF_Create(&clnt->recFilter, clnt->bfParam->m, clnt->bfParam->k);
    
    clnt->partialTrafer=partial;
    clnt->parts=parts;
    
    clnt->multi=multi;
    
    if(multi==1){
        int32_t cores= num_cores();
        clnt->threads=cores;
    }
    
    clnt->indexes=calloc(clnt->n, sizeof(int32_t*));
    for(int i=0;i<clnt->n;i++){
        int32_t* e= calloc(clnt->bfParam->k, sizeof(int32_t));
        clnt->indexes[i]=e;
    }
    
    clnt->verify=verify;
    
    
    int32_t r= clnt->bfParam->m%chunkSize;
    
    if(r==0){
        clnt->count=clnt->bfParam->m/chunkSize;
    }else{
        clnt->count=clnt->bfParam->m/chunkSize+1;
    }
    
    clnt->startBit=0;
    clnt->startByte=0;
    
    r=clnt->bfParam->m%(clnt->parts*8);
    
    if(r==0){
        clnt->stepByte=clnt->bfParam->m/(clnt->parts*8);
    }else{
        clnt->stepByte=clnt->bfParam->m/(clnt->parts*8)+1;
    }
    clnt->stepBit=clnt->stepByte*8;
    clnt->endBit=clnt->startBit+clnt->stepBit;
    clnt->endByte=clnt->startByte+clnt->stepByte;
    
    if(clnt->partialTrafer==1){
        OTExtRcvr_Create(&clnt->rcvr, NULL, clnt->stepBit, clnt->bfParam->k);
    }else{
        OTExtRcvr_Create(&clnt->rcvr, NULL, clnt->bfParam->m, clnt->bfParam->k);
    }
    

    int sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(sock>=0);
    
    clnt->socket=sock;
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    
    int rtnVal = inet_pton(AF_INET, IP, &servAddr.sin_addr.s_addr);
    assert(rtnVal>0);
    servAddr.sin_port=htons(port);
    
    rtnVal=connect(sock, &servAddr, sizeof(servAddr));
    assert(rtnVal>=0);
    
    return clnt;
}


void socketTest(){
    
    printf("start client\n");
    int n=500000000;
    int maxBuff=n*4;
    
    char* IP ="127.0.0.1";
    in_port_t port=6688;
    int sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(sock>=0);
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    
    int rtnVal = inet_pton(AF_INET, IP, &servAddr.sin_addr.s_addr);
    assert(rtnVal>0);
    servAddr.sin_port=htons(port);
    
    
    //send & receive data
    EC_GROUP* p192= EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
    
    BN_CTX* ctx=BN_CTX_new();
    BIGNUM* r = BN_new();
    BIGNUM* order=BN_new();
    const EC_POINT* g=EC_GROUP_get0_generator(p192);
    EC_GROUP_get_order(p192, order, NULL);
    
    rtnVal=connect(sock, &servAddr, sizeof(servAddr));
    assert(rtnVal>=0);
    //setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &maxBuff, sizeof(maxBuff));
    
    uint8_t* buf=calloc(n*4, sizeof(uint8_t));
    
    receiveBulk(sock, n*4, buf);
    
    printf("received!\n");
    //for(int i=0;i<n;i++){
    send(sock,buf,4*n,0);
    //}
    printf("sent!\n");
    int32_t* convert = (int32_t*) buf;
    for(int i=0;i<n;i++){
        
        if(i%100000==0)
            printf("%d\n",convert[i]);
    }
    
    //    uint8_t* buf[25];
    //    EC_POINT** ps2 = calloc(n,sizeof(EC_POINT*));
    //    int numRecv;
    //    for(int i=0;i<n;i++){
    //        memset(buf, 0, 25);
    //        numRecv=recv(sock, buf, 25, 0);
    //        assert(numRecv>=0);
    //        ps2[i]=EC_POINT_new(p192);
    //        EC_POINT_oct2point(p192, ps2[i], buf, 25, ctx);
    //    }
    //
    //    int32_t numSent;
    //    for(int i=0;i<n;i++){
    //        memset(buf, 0, 25);
    //        EC_POINT_point2oct(p192, ps2[i], POINT_CONVERSION_COMPRESSED, buf, 25, ctx);
    //        numSent=send(sock, buf, 25, 0);
    //        assert(numSent>=0);
    //
    //    }
    //    for(int i=0;i<n;i++)
    //        printf("%s\n",EC_POINT_point2hex(p192, ps2[i], POINT_CONVERSION_COMPRESSED, ctx));
}

