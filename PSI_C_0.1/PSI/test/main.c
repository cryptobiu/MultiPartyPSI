//
//  main.c
//  PSI
//
//  Created by Changyu Dong on 24/02/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/rsa.h>

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
#include "chtbl.h"
#include "intHashTbl.h"
#include "networkComm.h"
#include "list.h"
//#include "MurmurHash3.h"



#define MEM_ALLOC_SIZE		1024

void BF_PerformanceTest();
void RangeHashCorrectnessTest();
void barrattReduct();
void AESPerformance();
void AESRandomTest();
void GBFCorrectnessTest();
void GBFMTCorrectnessTestSingle();
void BFMTTest();
void GBFMTTest();
void ECTest();
void testExpSpeed();
void testmodmultiplyspeed();
void MDTest();
void NPOTTest();
void BitMatrixTest();

void OTExtTest();
void ECByteTest();
void SocketTest();
void hashTableTest();
void randomSetTest();

void networkCommTest();

void setVerificationTest();

void multiPartDebug();

void pairwiseInterSpeed();

//void murmurTest();
/**
 * memory pool sample code
 * @remark Error checks omitted
 */
int main(int argc, const char *argv[])
{
    
    //murmurTest();
    //pairwiseInterSpeed();
    //multiPartDebug();
    //setVerificationTest();
    //networkCommTest();
    //randomSetTest();
    //hashTableTest();
    //SocketTest();
    //ECByteTest();
    //OTExtTest();
    //BitMatrixTest();
    //NPOTTest();
    //MDTest();
    //ECTest();
    //testExpSpeed();
    //testmodmultiplyspeed();
    //GBFMTTest();
    //BFMTTest();
    //GBFMTCorrectnessTestSingle();
    //RangeHashCorrectnessTest();
    //BF_PerformanceTest();
    //GBFCorrectnessTest();
    //barrattReduct();
    
    //AESPerformance();
    //AESRandomTest();
}

//void barrattReduct(){
//    uint64_t a = 0xfffffff45ed;
//    int32_t m= 10000001;
//
//    int64_t two3s=((uint64_t)1)<<48;
//    int32_t mPrime =two3s%m;
//    int64_t mu=two3s/m;
//    
//    int64_t nprime =(a&(two3s-1))+(a>>48)*mPrime;
//    
//    int32_t R=((nprime>>32)*mu)>>16;
//    R=R*m;
//    R=nprime-R;
//    while(R<0){
//        R=R+m;
//    }
//    
//    while(R>=m){
//        R=R-m;
//    }
//    
//    
//    printf("R is: %d",R);
//    printf("mod result %d",a%m);
//    
//    
//}

//void murmurTest(){
//    void* out= calloc(128/8, sizeof(uint8_t));
//    uint8_t in[4];
//    for(uint8_t i=0;i<4;i++){
//        in[i]=i;
//    }
//    struct timeval t1;
//    struct timeval t2;
//    gettimeofday(&t1, NULL);
//    for(int i=0;i<1000000;i++){
//        MurmurHash3_x64_128(in, 4, 1, out);
//    }
//    gettimeofday(&t2, NULL);
//    double cpu_time_used;
//    
//    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
//    printf("murmur3_x86_128 (ms): %f\n", cpu_time_used);
//}

void pairwiseInterSpeed(){
//    int32_t num=1048576;
//    int32_t* a= randomIntArray(num);
//    int32_t* b= randomIntArray(num);
    int32_t num=1048576;
    int32_t* a= randomIntArray(num*2);
    int32_t* b= a+num;
    //int32_t* result= calloc(1000, sizeof(int32_t));
    int32_t count=0;
    
    struct timeval t1;
    struct timeval t2;
    gettimeofday(&t1, NULL);
    
    for(int i=0;i<num;i++){
        if(containsInt(a,b[i],num)){
            //result[count]=b[i];
            count++;
        }
    }
    
    
    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("pairwise compare (ms): %f, has %d common\n", cpu_time_used,count);
    
}

void multiPartDebug(){
    int round =4;
    
    int tColLen;
    int decColLen;
    
    char path[64];
    
    
    
}

void testmodmultiplyspeed(){
    
    RSA* key = RSA_generate_key(3072, 3, NULL, NULL);
    
    BIGNUM ** a = malloc(sizeof(BIGNUM*)*1000000);
    BIGNUM ** b = malloc(sizeof(BIGNUM*)*1000000);
    
    for(int i=0;i<1000000;i++){
        a[i]=BN_new();
        BN_init(a[i]);
        b[i]=BN_new();
        BN_init(b[i]);
        BN_rand_range(a[i], key->n);
        BN_rand_range(b[i], key->n);
    }
    
    BN_CTX* bnctx=BN_CTX_new();
    BN_CTX_init(bnctx);
    
    BN_MONT_CTX* ctx = BN_MONT_CTX_new();
    BN_MONT_CTX_init(ctx);
    BN_MONT_CTX_set(ctx, key->n, bnctx);
    
    struct timeval t1;
    struct timeval t2;
    gettimeofday(&t1, NULL);


    BIGNUM* r=BN_new();
    BN_init(r);
    for(int i=0;i<1000000;i++){
        //BN_mod_mul(a[i], a[i], b[i],key->n,bnctx);
        BN_mod_mul_montgomery(r, a[i], b[i], ctx, bnctx);
    }
    
    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("%d bits strength,1m multiplication time used (ms): %f\n", 3072,cpu_time_used);
    
}

void setVerificationTest(){
    int n=100000;
//    uint8_t** set1 = calloc(n, sizeof(uint8_t*));
//    
//    for(int i=0;i<n;i++){
//        int k=i+1;
//        set1[i]= calloc(4, sizeof(uint8_t));
//        memcpy(set1[i], (uint8_t*)&k, 4);
//    }
//    
//    uint8_t** set2 = calloc(n, sizeof(uint8_t*));
//    
//    for(int i=n-100,j=0;i<2*n-100;i++,j++){
//        int k=i+1;
//        set2[j]= calloc(4, sizeof(uint8_t));
//        memcpy(set2[j], (uint8_t*)&k, 4);
//    }
//    
//    List* L =calloc(1, sizeof(List));
//    
//    list_init(L, &free);
//    
//    for(int i=n-100;i<n;i++){
//        list_ins_next(L, NULL, set1[i]);
//    }
    
    uint8_t** set1=randomIntSet(n);
    uint8_t** set2=randomIntSet(n);
    List* L =calloc(1, sizeof(List));
    list_init(L, &free);
    
    for(int i=0;i<n;i++){
        if(contains(set1, set2[i], n, defaultDatalen)){
            list_ins_next(L, NULL, set2[i]);
        }
    }
    
    
    verifyInterSection(set1, set2, n, L);
    
    
    
}

void* serverThrd(void* args){
    printf("server starts\n");
    int n=200000000;
    char* IP ="127.0.0.1";
    in_port_t port=6687;
    
    int32_t servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    int iSetOption=1;
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &iSetOption, sizeof(iSetOption));
    assert(servSock>=0);
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servAddr.sin_port=htons(port);
    
    int b=bind(servSock,(struct sockaddr*) &servAddr, sizeof(servAddr));
    assert(b>=0);
    
    b=listen(servSock,5);
    assert(b>=0);
    
    struct sockaddr_in cliAddr;
    socklen_t cliAddrLen = sizeof(cliAddr);
    int cliSock;
    //do{
    cliSock = accept(servSock,&cliAddr,&cliAddrLen);
    // }while(cliSock<0);
    assert(cliSock>=0);
    
//    //send & receive data
//    EC_GROUP* p192= EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
//    
//    BN_CTX* ctx=BN_CTX_new();
//    BIGNUM* r = BN_new();
//    BIGNUM* order=BN_new();
//    const EC_POINT* g=EC_GROUP_get0_generator(p192);
//    EC_GROUP_get_order(p192, order, NULL);
//    
//    EC_POINT** points = calloc(n, sizeof(EC_POINT*));
//    for(int i=0;i<n;i++){
//        EC_POINT* p= EC_POINT_new(p192);
//        BN_rand_range(r, order);
//        EC_POINT_mul(p192, p, NULL, g, r, ctx);
//        
//        points[i]=p;
//    }
//    
//    int32_t pointByteLen= (int32_t)EC_POINT_point2oct(p192, points[0], POINT_CONVERSION_COMPRESSED, NULL, 0, ctx);
//    
//    sendArrayOfECPoints(cliSock, n, pointByteLen, p192, points);
    
    int32_t k=80;
    
    int32_t byteLen=GBFSigmaByteLen;
    
    uint8_t** out = calloc(n, sizeof(uint8_t**));
    
    AESRandom* rnd;
    AESRandom_Create(&rnd, 16);
    
    for(int i=0;i<n;i++){
        out[i]= calloc(byteLen, sizeof(uint8_t));
        //out[i][0] = calloc(byteLen, sizeof(uint8_t));
        AESRandom_NextBytes(rnd, out[i], byteLen);
        //out[i][1] = calloc(byteLen, sizeof(uint8_t));
        //AESRandom_NextBytes(rnd, out[i][1], byteLen);
    }
    
    //send3DBitArray(cliSock, n, 2, byteLen, out);
    
    sendArrayOfByteStrings(cliSock, n, byteLen, out);
    
    uint8_t*** myArgs= (uint8_t***) args;
    
    *myArgs=out;
    close(cliSock);
    
    //sendArrayOfByteStrings(cliSock, n, byteLen, out);

    
   // EC_POINT*** myArgs= (EC_POINT***)args;
//    
    //*myArgs=points;
    
    return NULL;
    
}

void* clientThrd(void* args){
    printf("start client\n");
    int n=200000000;
    
    char* IP ="127.0.0.1";
    in_port_t port=6687;
    int sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(sock>=0);
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    
    int rtnVal = inet_pton(AF_INET, IP, &servAddr.sin_addr.s_addr);
    assert(rtnVal>0);
    servAddr.sin_port=htons(port);

    
    rtnVal=connect(sock, &servAddr, sizeof(servAddr));
    assert(rtnVal>=0);
    
//        //send & receive data
//        EC_GROUP* p192= EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
//    
//        BN_CTX* ctx=BN_CTX_new();
//        BIGNUM* r = BN_new();
//        BIGNUM* order=BN_new();
//        const EC_POINT* g=EC_GROUP_get0_generator(p192);
//        EC_GROUP_get_order(p192, order, NULL);
//    
//    int32_t byteLen=(int32_t) EC_POINT_point2oct(p192, g, POINT_CONVERSION_COMPRESSED, NULL, 0, ctx);
//    
//    EC_POINT** points=receiveArrayOfECPoints(sock, n, byteLen, p192);
//    
//    EC_POINT*** myArgs= (EC_POINT***)args;
//    //
//    *myArgs=points;
//    
//    return NULL;
    
    int32_t k=80;
    int32_t byteLen=GBFSigmaByteLen;
    
    uint8_t** recved= receiveArrayOfByteStrings(sock, n, byteLen);
    
//    for(int i=0;i<n;i++){
//        printf("c bit: ");
//        printBytes(recved[i], byteLen);
//    }
    
    uint8_t*** myArgs= (uint8_t***)args;
    
    *myArgs=recved;
    close(sock);
    
    return NULL;
    
    
//    //send & receive data
//    EC_GROUP* p192= EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
//    
//    BN_CTX* ctx=BN_CTX_new();
//    BIGNUM* r = BN_new();
//    BIGNUM* order=BN_new();
//    const EC_POINT* g=EC_GROUP_get0_generator(p192);
//    EC_GROUP_get_order(p192, order, NULL);
}

void networkCommTest(){
    pthread_t server;
    pthread_t client;
    
    uint8_t*** sout=NULL;
    uint8_t*** cout=NULL;
    
    pthread_create(&server, NULL, &serverThrd, &sout);
    pthread_create(&client, NULL, &clientThrd, &cout);
    
    pthread_join(server, NULL);
    pthread_join(client, NULL);
    
    int n=200000000;
    int k=80;
    int byteLen=GBFSigmaByteLen;
    
    for(int i=0;i<n;i++){
        if(!compareByteArray(sout[i], cout[i], byteLen)){
            printf("something is wrong\n");
        }
//        printBytes(sout[i][0], byteLen);
//        printBytes(cout[i][0], byteLen);
//        if(!compareByteArray(sout[i][1], cout[i][1], byteLen)){
//            printf("something is wrong 1\n");
//        }
//        printBytes(sout[i][1], byteLen);
//        printBytes(cout[i][1], byteLen);
//        printf("\n");
    }
    
    printf("all correct\n");
    
    
//    EC_GROUP* p192= EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
//    
//    BN_CTX* ctx=BN_CTX_new();
//    BIGNUM* r = BN_new();
//    BIGNUM* order=BN_new();
//    const EC_POINT* g=EC_GROUP_get0_generator(p192);
//    EC_GROUP_get_order(p192, order, NULL);
//    
//    int32_t byteLen=(int32_t) EC_POINT_point2oct(p192, g, POINT_CONVERSION_COMPRESSED, NULL, 0, ctx);
//    
//    for(int i=0;i<n;i++){
//        uint8_t* bufs= calloc(byteLen, sizeof(uint8_t));
//        uint8_t* bufc= calloc(byteLen, sizeof(uint8_t));
//        
//        EC_POINT_point2oct(p192, sout[i], POINT_CONVERSION_COMPRESSED, bufs, byteLen, ctx);
//        EC_POINT_point2oct(p192, cout[i], POINT_CONVERSION_COMPRESSED, bufc, byteLen, ctx);
//        printBytes(bufs, byteLen);
//        printBytes(bufc, byteLen);
//        if(!compareByteArray(bufs, bufc, byteLen)){
//            printf("something is wrong\n");
//            printBytes(bufs, byteLen);
//            printBytes(bufc, byteLen);
//        }
//        memset(bufs, 0, byteLen);
//        memset(bufc, 0, byteLen);
//    }
//    
//    printf("All correct\n");
    
}

void randomSetTest(){
    int32_t n=1000;
    uint8_t** set= randomIntSet(n);
    
    
    int32_t* out= setToInt(set, n);
//    printf("out\n");
//    for(int i=0;i<n;i++){
//        printf("%d\n",out[i]);
//    }
    
    char* path="./set.bin";
    writeSetToFile(path, set, n);
    int32_t num;
    uint8_t** read= readInputFromFile(path, &num);
    
    int32_t* in = setToInt(read, num);
    
    printf("in size= %d\n",num);
    for(int i=0;i<n;i++){
        if(in[i]!=out[i]){
            printf("something is wrong\n");
        }
        
       // printf("%d\n",in[i]);
    }
    printf("correct\n");
}

void hashTableTest(){
    CHTbl table;
    intHash_init(&table, 1000000);
    
    AESRandom* rnd;
    AESRandom_Create(&rnd, 16);
    
    int32_t rndNum[1000000];
    
    for(int i=0;i<1000000;i++){
        if(i%10000==0)
            printf("%d\n",i);
        int s=AESRandom_NextInt(rnd);
        rndNum[i]=s;
        int insert=intHash_insert(&table, s);
        
        if(exists(s, rndNum, i)){
            if(insert!=1){
                printf("insert not 1");
            }
        }else{
            if(insert!=0){
                printf("insert not 0");
            }
        }
    }
    
    printf("false negative test\n");
    for(int i=0;i<1000000;i++){
        if(intHash_lookup(&table, rndNum[i])!=0)
            printf("false negative!\n");
    }
    
//    printf("no false negative!\n");
//    
//    printf("false positive test\n");
//    for(int i=1000000;i<2000000;i++){
//        if(intHash_lookup(&table, i)==0)
//            printf("false positviee!\n");
//    }
//    
//    printf("no false positive!\n");
    
    
    
}

void SocketTest(){
    int n=500000000;
    int maxBuff=n*4;
    printf("start Server\n");
    char* IP ="127.0.0.1";
    in_port_t port=6688;
    
    int32_t servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert(servSock>=0);
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servAddr.sin_port=htons(port);
    
    int b=bind(servSock,(struct sockaddr*) &servAddr, sizeof(servAddr));
    assert(b>=0);
    
    b=listen(servSock,5);
    assert(b>=0);
    
    struct sockaddr_in cliAddr;
    socklen_t cliAddrLen = sizeof(cliAddr);
    int cliSock;
    //do{
    cliSock = accept(servSock,&cliAddr,&cliAddrLen);
   // }while(cliSock<0);
    assert(cliSock>=0);
    
    //send & receive data
    AESRandom* rnd ;
    AESRandom_Create(&rnd, 16);
    
    int32_t* toSend=calloc(n, sizeof(int32_t));
    for(int i=0;i<n;i++){
        toSend[i]=AESRandom_NextInt(rnd);
    }
    setsockopt(cliSock, SOL_SOCKET, SO_RCVBUF, &maxBuff, sizeof(maxBuff));
    int numSent =send(cliSock,(uint8_t*)toSend,4*n,0);
    
    
    int32_t* verify=calloc(n, sizeof(int32_t));
    
    //for(int i=0;i<n;i++){
    receiveBulk(cliSock, n*4, (int8_t*)verify);
        //recv(cliSock, recvd, 4, 0);
        //verify[i]=*(int32_t*)recvd;
    //}
    
    //int32_t* converted= (int32_t*)recvd;
    
    for(int i=0;i<n;i++){
        if(i%100000==0)
            printf("%d\n",toSend[i]);
        if(toSend[i]!=verify[i]){
            printf("send receive differs: send= %d, recved= %d", toSend[i],verify[i]);
        }
    }
    
    printf("correct\n");
    
//    EC_GROUP* p192= EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
//
//    BN_CTX* ctx=BN_CTX_new();
//    BIGNUM* r = BN_new();
//    BIGNUM* order=BN_new();
//    const EC_POINT* g=EC_GROUP_get0_generator(p192);
//    EC_GROUP_get_order(p192, order, NULL);
//    
//    uint8_t buf[25];
//    EC_POINT** ps = calloc(n,sizeof(EC_POINT*));
//    int32_t numSent;
//    for(int i=0;i<n;i++){
//        memset(buf, 0, 25);
//        ps[i]= EC_POINT_new(p192);
//        BN_rand_range(r, order);
//        EC_POINT_mul(p192, ps[i], NULL, g, r, ctx);
//        EC_POINT_point2oct(p192, ps[i], POINT_CONVERSION_COMPRESSED, buf, 25, ctx);
//        numSent=send(cliSock, buf, 25, 0);
//        assert(numSent>=0);
//        
//    }
//    
//    EC_POINT** ps2 = calloc(n,sizeof(EC_POINT*));
//    int numRecv;
//    for(int i=0;i<n;i++){
//        memset(buf, 0, 25);
//        numRecv=recv(cliSock, buf, 25, 0);
//        assert(numRecv>=0);
//        ps2[i]=EC_POINT_new(p192);
//        EC_POINT_oct2point(p192, ps2[i], buf, 25, ctx);
//    }
//    
//    for(int i=0;i<n;i++){
//        if(!EC_POINT_cmp(p192, ps[i], ps2[i], ctx)){
//            printf("not Correct!\n");
//            printf("p1=%s\n",EC_POINT_point2hex(p192, ps[i], POINT_CONVERSION_COMPRESSED, ctx));
//            printf("p2=%s\n",EC_POINT_point2hex(p192, ps2[i], POINT_CONVERSION_COMPRESSED, ctx));
//        }
//    }
    close(cliSock);
    
}

void ECByteTest(){
    EC_GROUP* p192= EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
    
    EC_POINT* point= EC_POINT_new(p192);
    EC_POINT_set_to_infinity(p192, point);
    
    int size =25;
    BN_CTX* ctx=BN_CTX_new();
    BIGNUM* r = BN_new();
    BIGNUM* order=BN_new();
    const EC_POINT* g=EC_GROUP_get0_generator(p192);
    EC_GROUP_get_order(p192, order, NULL);
    
    uint8_t buf[25];
    size_t len= EC_POINT_point2oct(p192, point, POINT_CONVERSION_COMPRESSED, buf, 25, NULL);
    printf("%zd\n",len);
    printBytes(buf, 25);
    

    
}

void OTExtTest(){
    int32_t n=1000;
    int32_t k=80;
    BFParameters* param;
    BF_GenerateParameters(&param, n, k);

    int32_t m=param->m;
    int32_t lbyteLen= GBFSigmaByteLen;
    //int32_t lbyteLen= getByteLenByBitLen(l);
    
    AESRandom* rnd= getRandomInstance(k);
    //initialise sender
    printf("initialise sender\n");
    uint8_t** toSend = calloc(m, sizeof(uint8_t*));
    uint8_t** toSendCopy = calloc(m, sizeof(uint8_t*));
    
    //two copies of sending strings
    //toSendCopy is used for verificaiton
    for(int i=0;i<m;i++){
        toSend[i]= calloc(lbyteLen, sizeof(uint8_t));
        toSendCopy[i]= calloc(lbyteLen, sizeof(uint8_t));
        AESRandom_NextBytes(rnd, toSend[i], lbyteLen);
        memcpy(toSendCopy[i], toSend[i], lbyteLen);
    }
    
    OTExtSndr* sndr;
    
    OTExtSndr_Create(&sndr, m, k, toSend, lbyteLen);
    
    
    //initialise receiver
    printf("initialise receiver\n");
    int32_t strByteLen= getByteLenByBitLen(m);
    int32_t leadingZeroes= getLeadingZeroes(m);
    uint8_t* str=calloc(strByteLen, sizeof(uint8_t));
    
    AESRandom_NextBytes(rnd, str, strByteLen);
    
    OTExtRcvr* rcvr;
    
    OTExtRcvr_Create(&rcvr, str, m, k);
    
    //start OT
     printf("OT starts\n");
    struct timeval t1;
    struct timeval t2;
    double cpu_time_used;
    
    gettimeofday(&t1, NULL);
    EC_POINT** cs = OTExtRcvr_step0(rcvr);
    gettimeofday(&t2, NULL);
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("step 0 time used (ms): %f\n", cpu_time_used);
    
    
    uint8_t* buf = calloc(rcvr->baseOTSndr->pointByteLen, sizeof(uint8_t));
    EC_POINT** cs2 = malloc(sizeof(EC_POINT*)*k);
    
    for(int i=0;i<k;i++){
        memset(buf, 0, rcvr->baseOTSndr->pointByteLen);
        EC_POINT_point2oct(rcvr->baseOTSndr->curve, cs[i], POINT_CONVERSION_COMPRESSED, buf, rcvr->baseOTSndr->pointByteLen, NULL);
        cs2[i]=EC_POINT_new(rcvr->baseOTSndr->curve);
        EC_POINT_oct2point(rcvr->baseOTSndr->curve, cs2[i], buf, rcvr->baseOTSndr->pointByteLen, NULL);
    }
    
    
    
    gettimeofday(&t1, NULL);
    EC_POINT** pk0s = OTExtSndr_step1(sndr, cs2, k);
    gettimeofday(&t2, NULL);
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("step 1 time used (ms): %f\n", cpu_time_used);
    
    uint8_t*** encrypted = calloc(k, sizeof(uint8_t**));
    for(int i=0;i<k;i++){
        encrypted[i]=calloc(2, sizeof(uint8_t*));
    }
    
    gettimeofday(&t1, NULL);
    EC_POINT** grs=OTExtRcvr_step2_1(rcvr, pk0s, encrypted);
    gettimeofday(&t2, NULL);
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("step 2-1 time used (ms): %f\n", cpu_time_used);
    
    //printf("T\n");
    //BM_print(rcvr->T);
    
    gettimeofday(&t1, NULL);
    uint8_t** matrix = OTExtRcvr_step2_2(rcvr);
    gettimeofday(&t2, NULL);
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("step 2-2 time used (ms): %f\n", cpu_time_used);

    uint8_t** matrixReceived = calloc(k, sizeof(uint8_t*));
    for(int i=0;i<k;i++){
        matrixReceived[i]=calloc(strByteLen, sizeof(uint8_t));
        memcpy(matrixReceived[i], matrix[i], strByteLen);
    }
    
    gettimeofday(&t1, NULL);
    
    OTExtSndr_step3_1(sndr, encrypted, k/8, grs);
    gettimeofday(&t2, NULL);
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("step 3-1 time used (ms): %f\n", cpu_time_used);

    
    gettimeofday(&t1, NULL);
    BitMatrix* Q = OTExtSndr_step3_2(sndr, matrixReceived);
    gettimeofday(&t2, NULL);
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("step 3-2 time used (ms): %f\n", cpu_time_used);
    
//    printf("Q\n");
//    BM_print(Q);
    
    gettimeofday(&t1, NULL);
    uint8_t** enctyptedToSend= OTExtSndr_step3_3(sndr, Q);
    gettimeofday(&t2, NULL);
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("step 3-3 time used (ms): %f\n", cpu_time_used);
    
    gettimeofday(&t1, NULL);
    OTExtRcvr_step4_1(rcvr);
    gettimeofday(&t2, NULL);
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("step 4-1 time used (ms): %f\n", cpu_time_used);
    
    
    
//    printf("Decrypted T\n");
//    BM_print(rcvr->T);
//
//    
//    
//    for(int i=0;i<k;i++){
//        xorByteArray(rcvr->T->data[i], rcvr->str, strByteLen);
//    }
//    
//    printf("Decrypted + xored T\n");
//    BM_print(rcvr->T);

    gettimeofday(&t1, NULL);    
    OTExtRcvr_step4_2(rcvr, enctyptedToSend, lbyteLen);
    gettimeofday(&t2, NULL);
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("step 4-2 time used (ms): %f\n", cpu_time_used);
    
    //verify the result
    for(int i=0;i<m;i++){
        if(getBit(str, i, leadingZeroes)){
            //1 should receive the same bit string as toSendcopy[i]
            if(compareByteArray(enctyptedToSend[i], toSendCopy[i], lbyteLen)){
                ;
            }else{
                printf("%d 1: not equal!\n",i);
            }
        }else{
            //1 should not receive the same bit string as toSendcopy[i]
            if(!compareByteArray(enctyptedToSend[i], toSendCopy[i], lbyteLen)){
                ;
            }else{
                printf("%d 0: equal!\n",i);
                printf("%d lbyteLen!\n",lbyteLen);
                printBytes(enctyptedToSend[i], lbyteLen);
                printBytes(toSendCopy[i], lbyteLen);
                printBytes(enctyptedToSend[i-1], lbyteLen);
                printBytes(toSendCopy[i-1], lbyteLen);
            }
        }
    }
    printf("Done!");
    
    
    //clean up
    
//    for(int i=0;i<k;i++){
//        EC_POINT_free(pk0s[i]);
//        free(encrypted[i][0]);
//        free(encrypted[1][1]);
//        free(matrixReceived[i]);
//        free(encrypted);
//        //free(seeds[i]);
//        //free(Q->data[i]);
//    }
//    BM_Destroy(Q);
//    free(seeds);
//    free(matrixReceived);
//    free(encrypted);
//    free(pk0s);
//    
//    OTExtSndr_Destroy(sndr);
//    OTExtRcvr_Destroy(rcvr);
//    AESRandom_Destroy(rnd);
//    free(str);
//    for(int i=0;i<m;i++){
//        free(toSend[i]);
//        free(toSendCopy[i]);
//        free(enctyptedToSend[i]);
//    }
//    free(enctyptedToSend);
//    free(toSendCopy);
//    free(toSend);
    
}

void BitMatrixTest(){
    int m =200;
    int k=8;
    
    BitMatrix* bm;
    
    BM_Create(&bm, m, k);
    
    int content=128;
    
    for(int i=0;i<k;i++){
        uint8_t* col = malloc(sizeof(uint8_t)*bm->byteLenCol);
        
        for(int j=0;j<bm->byteLenCol;j++){
            col[j]= content>>i;
        }
        printBytes(col, bm->byteLenCol);
        BM_SetColumn(bm, i, col, bm->byteLenCol);
    }
    
    for(int i=0;i<bm->m;i++){
        uint8_t* row= malloc(sizeof(uint8_t)*bm->byteLenRow);
        BM_GetRow(bm, i, row, bm->byteLenRow);
        printBytes(row, bm->byteLenRow);
        free(row);
    }
    
    for(int i=0;i<k;i++){
        free(bm->data[i]);
    }

    BM_Destroy(bm);
    
}

void NPOTTest(){
    //security level
    int32_t secLev = 256;
    //how many pair to send
    int32_t k =1000;
    //byte length of the strings
    int32_t l =20;

    
    AESRandom* rnd;
    AESRandom_Create(&rnd, 16);
    
    //selection string
    int32_t strByteLen = getByteLenByBitLen(k);
    int32_t leadingZeroes = getLeadingZeroes(k);
    
    uint8_t* str=calloc(strByteLen, sizeof(uint8_t));
    
    AESRandom_NextBytes(rnd, str, strByteLen);

    
    //strings to send;
    uint8_t*** toSend= malloc(sizeof(uint8_t**)* k);
    
    for(int i=0;i<k;i++){
        uint8_t** pair=malloc(sizeof(uint8_t*)*2);
        pair[0]= calloc(l, sizeof(uint8_t));
        pair[1]= calloc(l, sizeof(uint8_t));
        AESRandom_NextBytes(rnd, pair[0], l);
        AESRandom_NextBytes(rnd, pair[1], l);
        toSend[i]=pair;
        //printf("i=%d.\n",i);
        //printBytes(toSend[i][1], l);
        //printBytes(toSend[i][0], l);
    }

    

    //initialise receiver
    NPOTRcvr* rcvr;
    NPOTRcvr_Create(&rcvr, str, strByteLen, leadingZeroes, secLev);
    
    
    NPOTSndr* sndr;
    
    NPOTSndr_Create(&sndr, k, l, toSend, secLev);
    
    
    struct timeval t1;
    struct timeval t2;
    
    gettimeofday(&t1, NULL);
    
    
    //send cs to receiver
    EC_POINT** cs = NPOTSndr_getCs(sndr);
    
    EC_POINT** PKs= NPOTRcvr_RStep1(rcvr, cs, k);
    
    uint8_t*** encrypted = malloc(sizeof(uint8_t**)*k);
    
    for(int i=0;i<k;i++){
        encrypted[i]=malloc(sizeof(uint8_t*)*2);
    }
    
    EC_POINT** grs = NPOTSndr_Step(sndr, PKs, encrypted);
    
    uint8_t** decrypted = NPOTRcvr_RStep2(rcvr, encrypted,l, grs);
    
    
    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("time used (ms): %f\n", cpu_time_used);
    
    
    for(int i=0;i<k;i++){
        if(getBit(str, i, leadingZeroes)){
            if(compareByteArray(decrypted[i], toSend[i][1], l)){
                //printf("1 correct, i=%d.\n",i);
               // printf("1 correct, i=%d.\n",i);
                //printBytes(decrypted[i], l);
               // printBytes(toSend[i][1], l);
               // printBytes(toSend[i][0], l);
            }else{
                printf("1 incorrect, i=%d.\n",i);
            }
        }else{
            if(compareByteArray(decrypted[i], toSend[i][0], l)){
                //printf("0 correct, i=%d.\n",i);
                //printBytes(decrypted[i], l);
               // printBytes(toSend[i][0], l);
               // printBytes(toSend[i][1], l);
            }else{
                printf("0 incorrect, i=%d.\n",i);
            }
        }
    }
    
    printf("DONE!\n");
    
    
    AESRandom_Destroy(rnd);
    free(str);
    NPOTRcvr_Destroy(rcvr);
    NPOTSndr_Destroy(sndr);
    
    for(int i=0;i<k;i++){
        free(toSend[i][0]);
        free(toSend[i][1]);
        free(encrypted[i][0]);
         free(encrypted[i][1]);
         free(encrypted[i]);
        //needed when run in two processes
        //free(decrypted[i]);
        EC_POINT_free(PKs[i]);
    }
    
    free(toSend);
    free(encrypted);
    free(decrypted);
    
}


void MDTest(){
    MessageDigest * sha1;
    
    MD_Create(&sha1, 80);
    
    MessageDigest * sha256;
     MD_Create(&sha256, 128);
    MessageDigest * sha384;
     MD_Create(&sha384, 192);
    MessageDigest * sha512;
     MD_Create(&sha512, 256);
    
    uint8_t* sha1Out=malloc(sizeof(uint8_t)* sha1->digestLen);
    
    uint8_t* sha256Out=malloc(sizeof(uint8_t)* sha256->digestLen);
    
    uint8_t* sha384Out=malloc(sizeof(uint8_t)* sha384->digestLen);
    
    uint8_t* sha512Out=malloc(sizeof(uint8_t)* sha512->digestLen);
    
    
    int i=0;
    MD_Digest(sha1, &i, 4, sha1Out);
    MD_Digest(sha256, &i, 4, sha256Out);
    MD_Digest(sha384, &i, 4, sha384Out);
    MD_Digest(sha512, &i, 4, sha512Out);
    
    printBytes(sha1Out, sha1->digestLen);
    printBytes(sha256Out, sha256->digestLen);
    printBytes(sha384Out, sha384->digestLen);
    printBytes(sha512Out, sha512->digestLen);
    
    MD_Destroy(sha1);
    MD_Destroy(sha256);
    MD_Destroy(sha384);
    MD_Destroy(sha512);
    
    free(sha1Out);
    free(sha256Out);
    free(sha384Out);
    free(sha512Out);
    
    
}


#define testNum 1000
void testExpSpeed(){
    BN_CTX* ctx=BN_CTX_new();
    
    BIGNUM* p =BN_new();
    BIGNUM* q =BN_new();
    BIGNUM* g =BN_new();
    BIGNUM* r =BN_new();
    BN_hex2bn(&p, "bb6c938cceecba61bdf660ae1aff3c475f382c2715a6c8644f361e81455b3132595b29d608ae5dd9f80243253abad1b0eba4611db4c9391f479047dabadb1c652afab50155c3b9dbfb47d14fb626ae4bbed05edc4f10bd1f2f7d91d595f83d0af6f73f49a11e9a525d6c505d7aa5d393c06bf2cdb9378aa0684d8e3bec82c87805e7d7961901a55b32a3a5a4eadc75a65a311740c11e35f49a5c48a54ad499dd430356ba97a018dc706c055fafd85664e4a424102000b2d10098955885d708aa21e85f8496952725b71232858ab13ca678639d7aad91f7ed6033a24b6885f7f76bc669a71e0aa4e9958010da7ff2be9aeddad45baa9378b434b5590a09cd37da4e8807de8f6ad2f2431d83418009d894a70e35540b2214686da53f7b983df21e0679dc51b0195ba21f008c8ece6a29a07285f2f41ff563a1c3901bc10ab65e07b721afed8ab401e058155749f19b3c0a1fe01aea0d0b9f9d5c2a3f60177a4b922cc92ccb3dc71b1e38cd1a3484463d90854c86b85eb14f12a43ec5528066bf274b568373b37f7099adbd3ac5634eec68ca573d8f1fc2a1cec8e4a8e631f92dfe4943225eb9cb7ca3a9254b7eb1537b525b7f271f5606e0516dbe2aa3ec769c4ec488773a3956485da34a1129b6ae81573476c59abc686015db1a6ddd273c79ca33cc94b58b964ae926c4d26066bb24e2787e9fa44ab4ff8a2bf66a9d124af4d974a547ad15a48bf03eb146a791438521f650da500b0d762f7de882162cbb21503d659cce5e52b6ee3348bc511e4817f030bd5273390c5f80cacdcb245de15260538baabffd6d4ea8954d05e1308039ac69ffedf557ad0b654896c268f2181361d67f6d5c7d1e46f57e99d8f03387bd0cc43330a67b29abd91c73cf2dfb0ba8a49bb28e7b1f04cdd8105a6c376dba80b4987ddeb29b4e6dc74f50888e8962051635082b690226ccacf172941ced8a10adb788e2ee5372b338746b9690a6f40d919a8e54db4adf7b0c2278823a7309f68c2d2493cc1cddf98042aa3adc3e96d7c62e23d5c89009c89e15f2945103e06e0508be99582c5f282cb40ea871706e6517376679f9925bb0232cf1d0e9ccaac27b388171eb342cd569d1a62004699e1fed75d194dd7ddbaa6985779a6a7fe13ed5f7150177e7c5931d195764e8970791aa1f7df8fb318ea207631245e0509149c76ce27a063f46ddf9de0cad2db4176ed3b04c662a3b7a50d718252fe953dfe895c7a544f677212fba165a570536b55e725f594bbc136eb3cdb6131ba9c19b4badbc8075aa0ac3b14631c38301c0eba07799420455382c6d987623687fa9fe093cacaf2ad1a2ad284bb1d9de5752d2d4313e54fc2d1caeabe10bed800a86c24e8912c4e9eaf3a5908fd8cc41b99b1007b4f361dd9fea6ad6a607d7b17d168b0ea74e462a220d10dcba663a4675bfe82b4619865c60de9be86ec888e69c2dc74d1ce8b54c9dfeb2508b08d44819d8ab02e183ba58f08184088e04a6ff0144720b6d6d426733cab4aff6fd59b2a2245046d8277046b508533b9f0324062ba12bb469e6a3b4a02b0b0c0dd205c567a5ae0cf30193279dc8299d50ff0853c3d3ef21903444b75e4ad203f1951ec4b903820df3d857b548484949d44f5475c8ee1b88952ca90f2cfda20fe3564a4fb11d0e87c447b83b53e9f2e7ff6468f520df2ac5d2580e200aad18aa06afae734a6e625e1dc9fecd58bca28c8f460b610b3bd188f9f566e4bbc61d53a4a4bc0f384a9a5c6ea4eef06bc63bda677d755467009979f3544bc1c51d5c0d1c4dd56fd1932523fe35d0738c57b2a0ddaf69cbb42b19aeea0bd207bdc676622664f8c4b58f1d27ea140d08c15f32b7450230410336cdb4d8ca02eec58cb41ad6e3fbee6732f0024a8ca0b0b4ec92618e320ce7f8e9c958f5fc85305a5615109dd336b5315bb235f3b681fb35e8f1e833c55f621ed9b88cee0d3aa00cefdc36e98635f61dd9a9929ee8a10317a88ed46319903f0ff99d0a5486208290fc51295db2e7de384f0df8ad08d50c6b9cf0030c9291465f9edd23749df45bbc856516d18f8d22eadd249c6ab5b5f30ee27dc25f8808db4059eb64d8ffc02cab2f515f9fb97d222f0d982e7434018f87f46538a07ed10ea19cc4a1eb4d630a8be8e27e12cc1e4516b57588f0f5e1d458235fc8f642d8c5c6848fc93006ed9e1882e5ad21dca0b33be4838c74d0e66735076f998a971e91f3641f68b874922213f1f67eedd421beca3e1cfa75984a3d23d29410f2ac736c879bb21b5b96eb2b60a7aaabb879bef2c137a274c85ef33052d63f75a943f5dbe29b815e7e8593154d9f47a51742904ee42db363d329ebe457ba6ba9262ddc41b38cca1974e06a1232f6caf655c1db99adfac45f983ae5b2003a1087a4f5c2e053ab69b6a7cb833554c63f7262a76fa5805825ea156fa2c210ef20497f15cfc8e3c1cb95f75b2b4bd1244624a0b63abeb065c6f067e9c05dfabb3fbb0929c990c2064ee30ceb55bc36492ed8a7ba444ba12830a7d962df8ad4cb3239b3949312fa785ecf23ed6a56e5ea35ebf47452b42308bf71d710dcdf3ef31903ff03d464d98e0b50b693e12eba1ca978cebf68ae119b1295b8515fa0e50658a1b6eecf4b6d6974930f080f027673f21edee97606329257fa438f14d67ea771d1987d04eedfe80cf18bdd5eed85c7dd968f1af44e068730cbbd");
    BN_hex2bn(&q,"daf3c4a65e0dc5621b06fa317c9da6c41e2e954d253905c599047e6630e546d537bb2772839001401941c0beb5bd2ab6e618e5276ecbf5996fccbed6dc1d47b7");
    
    BN_hex2bn(&g,"9e513c11ab56f53d25299ae4aa318f4de563bc2f018e9bc1a6417def08558d7063667fbef7b6ba31f1d9e316be2f4fbdff97fe115f4050c2f36722457656217754e5186259784bf7bd16bf3bbcba499f857ea80cbd2dd3625cced90eb670785fdb343be5340a44c54cfbbb253fd26e918ffd30149d2e140ee98ac42fcd1371160eb87aa294406d8f6a19a36c1a63d6ed4bab2dcf2b27e5b48a898ce016524e5e20ab0fe8aed1278363b50ca386f25cf558b7b5757a5cda22c0f443c6d8bb4dd404aac1a41215aa3c8689a2ed1af1f18834057189ac2469923e7d714806427631f10f0aa957458ac1e49922ce1fa17ae13b1a8a2ef92fc86873b2f260d79588b3c0358ade99cf91117ea1bb3a1640034e2782f833cb8eb450f0ada9e76582fb7c4a8298c773d92179cef9cb06425e6ef94a12defe4e70c40fe8ddc376dba4024efd0293de1d07d60615b072a7764392a21afbff1b47af4101d63b7f5985c9a5c996382d10dc5b586d883be5390db8ba1c553c04433f1ccd2b7c47c55289fa86ac66a22cf3c4109c9013dce1c7510e107bdcb892a64486228157884a7e2f72c1357d788e6b63d84b8419e1c1e93a204e13c8e43b49031478dc53ab914ab1d0ca98a9d85eebd4b9ade885ec994e9693dd42e463bdb5dd2751a3dcb83622d6b7e0ff2a1e8ac73cc0cfdadce597566c08250fa71bccd9f9500831b85ddfd57ff6a3d8780a2e5fd9ac7bab9c326578c7458623ad54b8ecd101791f87704d8717d7de0b8eef9cf5bb5948047b3c60a6b02753bd00ba8ba8721b981b3cb387f6177f79d4e245982eaa6085c719c68590d83e14b880c4e0f7ba6e82c708c5a55a2b01f89b51ac44223279b004294707844ab88c7559c772813832275adae0041821ea4e839cffe259115d3e45140864401f360755f88e6a94ff4fcc753ab9da4916a62b97011d308bb6d05031ad035bd35d5df49db3372e4d66c77595fa642c822a50f34b9cfd469d36df43eb20a181b275ab1f14579604d0af629d534733c6f5ceddf17a7b9aff50474792fefe4c8c1ec02003a0dfe1819640c57a28ff68c2b2a9ec656a1eb70eec5b595fdf81ca150e624ee0657b06af5d37a79482b5b1ec1f2e318b0f0ab3252d75fa69662bf88e15775ad52a3f6523cb3bb3935550f7f17a9b32ce312056082476992e193641ccc1170dad5bc643992f10af26710e6c4f8ef9e5ed7f98700930b1415136afb47063a31c366c9474aa545537dca6ebdfafa50c4d51d32750d92605b5afff7427d4e157b585f04e8e023277886f004c0a8883e201382b2c6e1ebc21a781b530ccb86624c75426f3a9ab55c936054e688c68bc569ef8c6d4a93911d794c006c12b7636a354fafdedb694bd3676c2236ff15232c04988d796585e01853dd202016d006c635a9c38f86b8b630a814450d1d0ec462b8d8bb26ce94b1951b5c4f003a75954196bb9bcab9de3d12e62f71cda8b0e820c94765d79bdd58188558dc74aaacbc9309d16ca5dbec472b292bf401bdf609ac6092738128754224a383eb94c46cb7f43838f4e23d297aab6025946bbb40f3edb3126c9a6f6a2627d740c541d9323b1db84822935c28499bc469d9d52dc67720614c7b03369fa1819288de242993a38eb4a5211b8c1478dc85f5aadc83a0f049506ee97bba387261475b07c85b60675e63b6a116141b795b933f36034c2a8e68a7bd1ec2ceecf88d6e95c1c3d5430380caa881e1639ca33947de40015a6f6ae3cdd8cf3bad33398fea70457f42bcc85c92bcf6c6fb388683f9cfea81ca9eec3e223f9248523df6afe128489030004b2a7cc7279655c3a7610e7c2135510133d78039d478a397d5cc5fd7ec28d288079fd632cf5c7c8d1fb13a1b31ba760a86e9770bb688d9f54f232006d7f22d5086c5e77beb3a54be00dab8dbcac1622c81a89c4d1cf929537288d38e281ebf38d28881b769fb67bc7f1b53d39b7e4ff045ce0e88a451e7fec6dd2c393e13b7dce1806694b4cef5217eda5592aa21b190d62e0a2f49645a95463d3178944a4e6831d23b245723cf4851ad0fcb89cfa028d9d7dcfb94375f1957a7c7b4b88d197fd5ae0dd60e6915159347f4ad1e3ae58f874871f02a4cbd013907e24dd5a662c93a0a355284d1c72e89f0c3853c4c6f8ffc2070d8a6bd54b1c6ea615f6fd1f44e26b036b4fb9c6f436290ce04a5d1c1d5e6accca9da522f70fd38ec23b17697c092f73a05e201e48b70647d60b5d2545caea07ce5255365bf33ea5627913cbbbdf092a908387d4e24972c2702fbfb1a81533d2699a006d3673d2e349c720bc6c27a6935f244db6b5ac2fe8ff0e21d90c0ed4d3b1307250c2d6c6334647e28c7a6ed8a825d71da1a4042b95310a13b6a48645b5ae59b9d343470fa87eee364befe1d89f161d107fd9394ff94847508d43ebc0bf9b424e863444a348c4dd184e6ee865d52e99ce2989290b764881588c20f3ef7341819d403dc0afea39296a8fdc619bab41f021962868650abc5d910822ca6f9c5506c909c03090fdbaddc90bdfa9395614319ff0bf48d0a95c6da75ff7e64b8f63ef44e18dc779af5a4493c14ae6c2822b2b8042543164ac9ab40c1b883f19b52cf5462e0c933b42ed69f54e5eef14955a84ed6bebe4337ec1bfb853f2920f35ac6924974079f6267b182a8e283fe286c8bb12b6a42b01b4e815cc4c4fdecbca790fd");
    BN_rand_range(r, q);
    struct timeval t1;
    struct timeval t2;
    gettimeofday(&t1, NULL);
    for(int i=0;i<testNum;i++){
        BN_mod_exp(q, g, r, p, ctx);
    }
    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("%d bits strength, time used (ms): %f\n", 256,cpu_time_used);
    
    
}

void testECSpeed(EC_GROUP* group, int strength){
    const EC_POINT* G= EC_GROUP_get0_generator(group);
    EC_POINT* r= EC_POINT_new(group);
    BN_CTX* ctx=BN_CTX_new();
    
    BIGNUM** TV;
    
    TV=malloc(sizeof(BIGNUM*)*testNum);
    BIGNUM* order =BN_new();
    EC_GROUP_get_order(group, order, ctx);
    for(int i=0;i<testNum;i++){
        TV[i]=BN_new();
        BN_rand_range(TV[i],order);
    }
    struct timeval t1;
    struct timeval t2;
    
    gettimeofday(&t1, NULL);
    for(int i=0;i<testNum;i++){
        EC_POINT_mul(group, r, NULL, G, TV[i], ctx);
    }
    
    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("%d bits strength, time used (ms): %f\n", strength,cpu_time_used);
    
    BIGNUM* q= BN_new();
    EC_GROUP_get_order(group, q, ctx);
    
    BIGNUM* prod= BN_new();
    
    BN_mod_mul(prod, TV[0], TV[1], q, ctx);
    
    EC_POINT* r2= EC_POINT_new(group);
    
    EC_POINT_mul(group, r,NULL , G, TV[0], ctx);
    EC_POINT_mul(group, r,NULL , r, TV[0], ctx);
    EC_POINT_mul(group, r2, NULL, G, prod,ctx);
    
    if(EC_POINT_cmp(group, r, r2, ctx))
        printf("Multiplication is correct\n");
    else{
         printf("Multiplication is not correct\n");
    }
    
    
    
    for(int i=0;i<testNum;i++){
        BN_free(TV[i]);
    }
    free(TV);
    EC_POINT_free(r);
    EC_POINT_free(r2);
    BN_free(order);
    BN_free(prod);

    BN_CTX_free(ctx);
    
}

void ECTest(){

    EC_GROUP* p192= EC_GROUP_new_by_curve_name(NID_X9_62_prime192v1);
    EC_GROUP* p256= EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
    EC_GROUP* p384=EC_GROUP_new_by_curve_name(NID_secp384r1);
    EC_GROUP* p521= EC_GROUP_new_by_curve_name(NID_secp521r1);
    
    EC_GROUP* b163= EC_GROUP_new_by_curve_name( NID_sect163r2);
    EC_GROUP* b283= EC_GROUP_new_by_curve_name(NID_sect283r1);
    EC_GROUP* b571=EC_GROUP_new_by_curve_name(NID_sect571r1);
    
    testECSpeed(p192, 192);
    testECSpeed(p256, 256);
    testECSpeed(p384, 384);
    testECSpeed(p521, 521);
    testECSpeed(b163, 163);
    testECSpeed(b283, 283);
    testECSpeed(b571, 571);
    
    
}

void GBFMTTest(){
    printf("GBFMTTest()\n");
    GarbledBF* fil;
    int32_t n=1000000;
    int32_t k=80;
    
    printf("Create GBF\n");
    BFParameters *param;
    BF_GenerateParameters(&param, n, k);
    
    GBF_CreateForMT(&fil, param->m, param->k);
    
    uint8_t ** keys;
    
    keys=malloc(sizeof(uint8_t*)*k);
    RangeHash** hashes;
    hashes=malloc(sizeof(RangeHash)*k);
    
    //each key is a byte
    for(uint8_t i=0;i<k;i++){
        keys[i] = malloc(sizeof(uint8_t));
        keys[i][0]=i;
        //initialise rangehashes

        RangeHash_Create(&(hashes[i]), keys[i], 1, param->m);
    }
    
    //input
    uint32_t* is;
    uint8_t** input;
    uint8_t** ehash;
    is=calloc(n,sizeof(uint32_t));
    input=calloc(n,sizeof(uint8_t*));
    ehash=calloc(n,sizeof(uint8_t*));
    
    
    for(int i=0;i<n;i++){
        is[i]=i;
        input[i]=(uint8_t*)&(is[i]);
        uint8_t* bytes;
        bytes=malloc(sizeof(uint8_t*)*GBFSigmaByteLen);
        SHA1(input[i], 4, bytes);
        ehash[i]=bytes;
    }
    
    //free(is);
    struct timeval t1;
    struct timeval t2;
    
    gettimeofday(&t1, NULL);
    MTBuilder_GBF(fil, param, keys, k, 1, input, 4, n, ehash, 16);
    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("time used (ms): %f\n", cpu_time_used);
    
    printf("False negative test\n");
    
    for(int i=0;i<n;i++){
        if(i%100000==0)
            printf("test %d\n",i);
        if(!GBF_query(fil, hashes, param->k, (uint8_t*)&i, 4, ehash[i])){
            printf("False negative found. %d\n",i);
        };
    }
    printf("False negative test finished\n");
    

    
    printf("False positive test\n");
    
    uint8_t hash[GBFSigmaByteLen];
    for(int i=n-1;i<n*2;i++){
        if(i%100000==0)
            printf("test %d\n",i);
        SHA1((uint8_t*)&i, 4, hash);
        if(GBF_query(fil, hashes, param->k, (uint8_t*)&i, 4, hash)){
            printf("False positive found. %d\n",i);
        };
    }
    
    printf("False positive test finished\n");
    

    for(int i=0;i<n;i++){
        //free(input[i]);
        free(ehash[i]);
    }
    free(ehash);
    free(input);
    for(uint8_t i=0;i<k;i++){
        free(keys[i]);
        RangeHash_Destroy(hashes[i]);
    }
    free(keys);
    free(hashes);
    free(is);
    BF_Param_Destroy(param);
    GBF_Destroy(fil);
    

}

void BFMTTest(){
    printf("BFMTTest()\n");
    BF* fil;
    int32_t n=1000000;
    int32_t k=80;
    
    printf("Create BF\n");
    BFParameters *param;
    BF_GenerateParameters(&param, n, k);
    
    BF_Create(&fil, param->m);
    
    uint8_t ** keys;
    
    keys=malloc(sizeof(uint8_t*)*k);
    RangeHash** hashes;
   hashes=malloc(sizeof(RangeHash)*k);
    
    //each key is a byte
    for(uint8_t i=0;i<k;i++){
        keys[i] = malloc(sizeof(uint8_t));
        keys[i][0]=i;
        //initialise rangehashes
       RangeHash_Create(&(hashes[i]), keys[i], 1, param->m);
    }
    
    int32_t** indexes;
    
    indexes = malloc(sizeof(int32_t*)*n);
    
    uint32_t* is;
    uint8_t** input;
    is=calloc(n,sizeof(uint32_t));
    input=calloc(n,sizeof(uint8_t*));
    
    
    for(int i=0;i<n;i++){
        is[i]=i;
        input[i]=(uint8_t*)&(is[i]);
    }
    
    struct timeval t1;
    struct timeval t2;
    
    gettimeofday(&t1, NULL);
    MTBuilder_BF(fil, param, keys, k, 1, input, 4, n, indexes);
    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
        printf("time used (ms): %f\n", cpu_time_used);
    
    
    //BF_Print(fil);
    printf("False negative test\n");
    
    for(int i=0;i<n;i++){
        if(!BF_Query(fil,(uint8_t*) &i , 4, hashes, k)){
            printf("False negative!\n");
            return;
        }
    }
    printf("No False negative!\n");
    
    printf("False positive test\n");
    
    for(uint32_t i=n;i<10*n;i++){
        if(BF_Query(fil,(uint8_t*) &i , 4, hashes, k)){
            printf("False positive!\n");
            return;
        }
    }
    printf("No False positivetive!\n");
    
    
}

void GBFMTCorrectnessTestSingle(){
    printf("GBFMTCorrectnessTest()\n");
    GarbledBF* filter;
    int32_t n=1000000;
    int32_t k=80;
    
    printf("Create GBF\n");
    BFParameters *param;
    BF_GenerateParameters(&param, n, k);
    
    GBF_CreateForMT(&filter, param->m, param->k);
    
    uint8_t ** keys;
    
    keys=malloc(sizeof(uint8_t*)*k);
    RangeHash** hashes;
    hashes=malloc(sizeof(RangeHash)*k);
    
    //each key is a byte
    for(uint8_t i=0;i<k;i++){
        keys[i] = malloc(sizeof(uint8_t));
        keys[i][0]=i;
        //initialise rangehashes
        RangeHash_Create(&(hashes[i]), keys[i], 1, param->m);
    }
    
    //precompute indexes
    printf("precompute indexes\n");
    int32_t** indexes;
    
    indexes = malloc(sizeof(int32_t*)*n);
    
    for(int i=0;i<n;i++){
        int32_t* eIndexes;
        eIndexes= calloc(k, sizeof(int32_t));
        
        for(int j=0;j<k;j++){
            eIndexes[j]=RangeHash_Digest(hashes[j],(uint8_t*)&i, 4);
            
            if(exists(eIndexes[j], eIndexes, j)){
                eIndexes[j]=-1;
            }
        }
        indexes[i]=eIndexes;
        
    }
    
    
    //AESRandom
    AESRandom* rnd;
    uint8_t seed[16];
    
    for(uint8_t i=0;i<16;i++){
        seed[i]=i;
    }
    
    AESRandom_Create_With_Seed(&rnd, seed, 16);
    
    //precompute random bytes;
    printf("precompute random bytes\n");
    
    uint32_t blockSize = GBFSigmaByteLen;
    uint32_t totalBlocks = param->m-n;
    uint32_t numOfRows = 7000;
    uint32_t rowLength= (totalBlocks/numOfRows+1)*blockSize;
    
    RandomSource *rndSrc;
    
    RandomSource_Create(&rndSrc, numOfRows, blockSize);
    
    for(int i=0;i<numOfRows;i++){
        uint8_t* rowBytes;
        rowBytes = calloc(rowLength, sizeof(uint8_t));
        AESRandom_NextBytes(rnd, rowBytes, rowLength);
        RandomSource_Insert_Bytes(rndSrc, rowBytes, rowLength, i);
    }
    
    //compute hashes of elements
    printf("compute hashes of elements\n");
    uint8_t** ehash;
    ehash =calloc(n, sizeof(uint8_t*));
    
    
    for(int i=0;i<n;i++){
        uint8_t* bytes;
        bytes=calloc(GBFSigmaByteLen,sizeof(uint8_t));
        ehash[i]=bytes;
        SHA1((uint8_t*)&i, 4, ehash[i]);
    }
    
    printf("add element\n");
    
    struct timeval t1;
    struct timeval t2;
    
    gettimeofday(&t1, NULL);
    for(int i=0;i<n;i++){
        GBF_addMT(filter, indexes[i], k, ehash[i], rndSrc);
    }
    
    GBF_doFinalMT(filter, rndSrc);
    
    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("time used (ms): %f\n", cpu_time_used);
   
    printf("False negative test\n");
    
    for(int i=0;i<n;i++){
        if(i%100000==0)
            printf("test %d\n",i);
        if(!GBF_query(filter, hashes, param->k, (uint8_t*)&i, 4, ehash[i])){
            printf("False negative found. %d\n",i);
        };
    }
    printf("False negative test finished\n");
    
    printf("False negative test with index query\n");
    gettimeofday(&t1, NULL);
    for(int i=0;i<n;i++){
        if(i%100000==0)
            printf("test %d\n",i);
        if(!GBF_query_With_Indexes(filter, indexes[i], param->k, ehash[i])){
            printf("False negative found. %d\n",i);
        };

    }
    gettimeofday(&t2, NULL);
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("time used (ms): %f\n", cpu_time_used);
    printf("False negative test finished\n");
    
    printf("False positive test\n");
    
    uint8_t hash[GBFSigmaByteLen];
    for(int i=n-1;i<n*2;i++){
        if(i%100000==0)
            printf("test %d\n",i);
        SHA1((uint8_t*)&i, 4, hash);
        if(GBF_query(filter, hashes, param->k, (uint8_t*)&i, 4, hash)){
            printf("False positive found. %d\n",i);
        };
    }
    
    printf("False positive test finished\n");
    
    AESRandom_Destroy(rnd);
    RandomSource_Destroy(rndSrc);
    for(int i=0;i<n;i++){
        free(ehash[i]);
        free(indexes[i]);
    }
    free(indexes);
    free(ehash);
    for(uint8_t i=0;i<k;i++){
        free(keys[i]);
        RangeHash_Destroy(hashes[i]);
    }
    free(keys);
    free(hashes);
    BF_Param_Destroy(param);
    GBF_Destroy(filter);
    
}


void GBFCorrectnessTest(){
    GarbledBF* filter;
    int32_t n=1000000;
    int32_t k=80;
    
    BFParameters *param;
    BF_GenerateParameters(&param, n, k);
    
    GBF_Create(&filter, param->m, param->k);
    
    uint8_t ** keys;
    
    keys=malloc(sizeof(uint8_t*)*k);
    RangeHash** hashes;
    hashes=malloc(sizeof(RangeHash)*k);
    
    //each key is a byte
    for(uint8_t i=0;i<k;i++){
        keys[i] = malloc(sizeof(uint8_t));
        keys[i][0]=i;
        //initialise rangehashes
        RangeHash_Create(&(hashes[i]), keys[i], 1, param->m);
    }
    
    //AESRandom
    AESRandom* rnd;
    uint8_t seed[16];
    
    for(uint8_t i=0;i<16;i++){
        seed[i]=i;
    }
    
    AESRandom_Create_With_Seed(&rnd, seed, 16);
    
    uint8_t** ehash;
    ehash =calloc(n, sizeof(uint8_t*));
    
    
    for(int i=0;i<n;i++){
        uint8_t* bytes;
        bytes=calloc(GBFSigmaByteLen,sizeof(uint8_t));
        ehash[i]=bytes;
        SHA1((uint8_t*)&i, 4, ehash[i]);
    }
    
    struct timeval t1;
    struct timeval t2;
    
    gettimeofday(&t1, NULL);
    for(int i=0;i<n;i++){
        GBF_add(filter, hashes, k, (uint8_t*)&i, 4, ehash[i], rnd);
    }
        GBF_doFinal(filter, rnd);

    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("time used (ms): %f\n", cpu_time_used);
    

    
    printf("False negative test\n");
    
    for(int i=0;i<n;i++){
        if(!GBF_query(filter, hashes, param->k, (uint8_t*)&i, 4, ehash[i])){
            printf("False negative found. %d\n",i);
        };
    }
    printf("False negative test finished\n");
    printf("False positive test\n");
    
    uint8_t hash[GBFSigmaByteLen];
    for(int i=n;i<n*10;i++){
        SHA1((uint8_t*)&i, 4, hash);
        if(GBF_query(filter, hashes, param->k, (uint8_t*)&i, 4, hash)){
            printf("False positive found. %d\n",i);
        };
    }
    
    printf("False positive test finished\n");
    
    for(int i=0;i<n;i++){
        free(ehash[i]);
    }
    free(ehash);
    for(uint8_t i=0;i<k;i++){
        free(keys[i]);
        RangeHash_Destroy(hashes[i]);
    }
    free(keys);
    free(hashes);
    AESRandom_Destroy(rnd);
    BF_Param_Destroy(param);
    GBF_Destroy(filter);
    
    
    
}

void AESRandomTest(){
    AESRandom* rnd1;
    AESRandom* rnd2;
    
    const uint32_t dataLen=20;
    const uint32_t keyLen=16;
    uint8_t key[keyLen];
    uint8_t original[dataLen]={0};
    uint8_t data[dataLen]={0};
    uint8_t empty[dataLen]={0};
    for(uint8_t i=0;i<keyLen;i++){
        key[i]=i;
        data[i]=i;
        original[i]=i;
    }
    
    AESRandom_Create_With_Seed(&rnd1, key, keyLen);
    AESRandom_Create_With_Seed(&rnd2, key, keyLen);
    
    for(int i=0;i<1000;i++){
        AESRandom_NextBytes(rnd1, data, dataLen);
        AESRandom_NextBytes(rnd2, empty,dataLen);
        
        printf("DATA:\n");
        printBytes(data, dataLen);
        printf("EMPTRY:\n");
        printBytes(empty, dataLen);
        
        if(compareByteArray(data, original, dataLen)){
            printf("something is wrong. encrypted is the same as unencrypted.\n");
        }
        
        xorByteArray(data, empty, dataLen);
        printf("after Xor:\n");
        printf("DATA:\n");
        printBytes(data, dataLen);
        if(!compareByteArray(data, original, dataLen)){
            printf("something is wrong. decrypted is not the same as unencrypted.\n");
        }
        memset(empty, 0, dataLen);
        //memcpy(original, empty, dataLen);
    }
    
    printf("correct\n");
    
    
}

void AESPerformance(){
    EVP_CIPHER_CTX *ctx;
    
    uint8_t key[EVP_MAX_KEY_LENGTH];
    uint8_t iv[EVP_MAX_IV_LENGTH];
    
    for(uint8_t i=0;i<16;i++){
        key[i]=i;
    }
    
    if(!AES_Setup(&ctx, key, iv)){
        printf("something is wrong");
        return;
    };
    
    uint8_t out[20];
    uint8_t in[20]={0};
    int outl;
    
    struct timeval t1;
    struct timeval t2;
    
    gettimeofday(&t1, NULL);
    
    for(int i=0;i<1000000;i++){
        if(!AES_Enc(ctx, out,&outl , in, 20)){
        //RAND_bytes(out, 20);
 
            printf("something is wrong");
           return;
        };
    }
    
    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("time used (ms): %f\n", cpu_time_used);

    
    printf("done");
    
    EVP_CIPHER_CTX_free(ctx);
    
}

void RangeHashCorrectnessTest(){
        RangeHash *H;
    
        uint8_t key[32];
        for(uint8_t i=0;i<32;i++){
            key[i]=i;
        }
    RangeHash_Create(&H, key, 32, 65535);
    //
    //    clock_t t1, t2;
    //    t1 = clock();
    //
        uint8_t bytes[4];
        for(int i=0;i<1000000;i++){
            bytes[0]=((uint8_t * )&i)[3];
            bytes[1]=((uint8_t * )&i)[2];
            bytes[2]=((uint8_t * )&i)[1];
            bytes[3]=((uint8_t * )&i)[0];

          printf("%5d\n",RangeHash_Digest(H, bytes, 4));
        }
    //    t2 = clock();
    //    double cpu_time_used;
    //
    //    cpu_time_used = ((double) (t2 - t1)*1000) / CLOCKS_PER_SEC;
    //    printf("time used (ms): %f\n", cpu_time_used);

       
        RangeHash_Destroy(H);

    
}

void BF_PerformanceTest(){
    BF* filter;
    BFParameters* param;
    
    int n =1000000;
    int k=80;
    BF_GenerateParameters(&param, n, k);
    
    BF_Create(&filter, param->m);
    printf("m: %d\n", param->m);
    
    uint8_t ** keys;
    
    keys=malloc(sizeof(uint8_t*)*k);
    RangeHash** hashes;
    hashes=malloc(sizeof(RangeHash)*k);
    
    //each key is a byte
    for(uint8_t i=0;i<k;i++){
        keys[i] = malloc(sizeof(uint8_t));
        keys[i][0]=i;
        //initialise rangehashes
        RangeHash_Create(&(hashes[i]), keys[i], 1, param->m);
    }
    
    struct timeval t1;
    struct timeval t2;
    
    gettimeofday(&t1, NULL);
    
    for(int i=0;i<n;i++){
        BF_Add(filter, (uint8_t*) &i, 4, hashes, k);
    }
    
    gettimeofday(&t2, NULL);
    double cpu_time_used;
    
    cpu_time_used = (double)(t2.tv_sec-t1.tv_sec)*1000+(double)(t2.tv_usec-t1.tv_usec)/1000;
    printf("time used (ms): %f\n", cpu_time_used);

 
    printf("False negative test\n");
    
    for(int i=0;i<n;i++){
        if(!BF_Query(filter,(uint8_t*) &i , 4, hashes, k)){
            printf("False negative!\n");
            return;
        }
    }
    printf("No False negative!\n");
    
    printf("False positive test\n");
    
    for(uint32_t i=n;i<10*n;i++){
        if(BF_Query(filter,(uint8_t*) &i , 4, hashes, k)){
            printf("False positive!\n");
            return;
        }
    }
        printf("No False positivetive!\n");
    
        //BF_Print(filter);
    
    for(int i=0;i<k;i++){
        free(keys[i]);
        RangeHash_Destroy(hashes[i]);
    }
    free(keys);
    free(hashes);
    BF_Param_Destroy(param);
    BF_Destroy(filter);
    
    //    printf("bit %d is %d \n",0,BF_GetBit(filter, 0));
    //    printf("bit %d is %d \n",998,BF_GetBit(filter, 998));
    //    BF_Print(filter);
    
}

