//
//  networkComm.c
//  PSI
//
//  Created by Changyu Dong on 12/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include "networkComm.h"
//k keys each secLevl bits
uint8_t** receiveArrayOfByteStrings(int32_t sock, int32_t numOfStrings, int32_t strByteLen){
//    //total bytes
//    //int32_t byteLen= secLevl/8;
//    int64_t total = ((int64_t)numOfStrings)*((int64_t)strByteLen);
//    //printf("%lld\n",total);
//    //printf("%d\n",numOfStrings);
//    //printf("%d\n",strByteLen);
//    uint8_t* keyStr =calloc(total, sizeof(uint8_t));
//    printf("keyStr %p\n",keyStr);
//    
//    assert(keyStr!=NULL);
//    receiveBulk(sock, total, keyStr);
//    
//    uint8_t** out= calloc(numOfStrings, sizeof(uint8_t*));
//    
//    for(int i=0;i<numOfStrings;i++){
//        out[i]=keyStr+(i*strByteLen);
//    }
//    return  out;
    uint8_t** out= calloc(numOfStrings, sizeof(uint8_t*));
    for(int i=0;i<numOfStrings;i++){
        out[i]= calloc(strByteLen, sizeof(uint8_t));
        receiveBulk(sock, strByteLen, out[i]);
    }
    return out;
    
}

void sendArrayOfByteStrings(int32_t sock, int32_t numOfStrings, int32_t strByteLen, uint8_t** strArray){
    //int32_t byteLen= secLevl/8;
    
    int bunch =50;
    int total=bunch*strByteLen;
    uint8_t* e=calloc(total, sizeof(uint8_t));
    for(int i=0;i<numOfStrings;i+=bunch){
        if(i+bunch>=numOfStrings){
            bunch=numOfStrings-i;
            total=bunch*strByteLen;
        }
        for(int j=0;j<bunch;j++){
            memcpy(e+(j*strByteLen), strArray[i+j], strByteLen);
        }
        
        send(sock, e, total, 0);
    }
    free(e);
    
//    for(int i=0;i<numOfStrings;i++){
//        
//        
//        send(sock, strArray[i], strByteLen, 0);
//    }
}

uint8_t*** receive3DBitArray(int32_t sock, int32_t size1D, int32_t size2D, int32_t size3D){
    int32_t total= size1D*size2D*size3D;
    uint8_t* str=calloc(total, sizeof(uint8_t));
    assert(str!=NULL);
    
    receiveBulk(sock, total, str);
    
    uint8_t*** out = calloc(size1D, sizeof(uint8_t**));
    
    for(int i=0;i<size1D;i++){
        out[i]=calloc(size2D, sizeof(uint8_t*));
        for(int j=0;j<size2D;j++){
            out[i][j]=str+((i*size2D+j)*size3D);
        }
    }
    
    return out;
    
}

void send3DBitArray(int32_t sock, int32_t size1D, int32_t size2D, int32_t size3D,uint8_t*** strArray){
    for(int i=0;i<size1D;i++){
        for(int j=0;j<size2D;j++){
            send(sock, strArray[i][j], size3D, 0);
        }
    }
}

void sendArrayOfECPoints(int32_t socket, int32_t numOfPoints, int32_t pointByteLen, EC_GROUP* curve, EC_POINT** points){
    BN_CTX* ctx = BN_CTX_new();
    uint8_t* buf = calloc(pointByteLen, sizeof(uint8_t));
    int size;
    for(int i=0;i<numOfPoints;i++){
        size=(int32_t)EC_POINT_point2oct(curve, points[i], POINT_CONVERSION_COMPRESSED, buf, pointByteLen, ctx);
        assert(size==pointByteLen);
        send(socket, buf, pointByteLen, 0);
        
        memset(buf, 0, pointByteLen);
    }
    
    free(buf);
    BN_CTX_free(ctx);
}

EC_POINT** receiveArrayOfECPoints(int32_t socket, int32_t numOfPoints, int32_t pointByteLen, EC_GROUP* curve){
    BN_CTX* ctx = BN_CTX_new();
    
    int32_t totalByte= numOfPoints*pointByteLen;
    uint8_t* buf= calloc(totalByte, sizeof(uint8_t));
    
    receiveBulk(socket, totalByte, buf);
    
    EC_POINT** out = calloc(numOfPoints, sizeof(EC_POINT*));
    
    for(int32_t i=0;i<numOfPoints;i++){
        EC_POINT* p= EC_POINT_new(curve);
        
        EC_POINT_oct2point(curve, p, buf+(i*pointByteLen), pointByteLen, ctx);
        
        out[i]=p;
    }
    
    free(buf);
    BN_CTX_free(ctx);
    return out;
    
}