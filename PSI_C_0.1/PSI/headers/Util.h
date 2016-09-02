//
//  Util.h
//  PSI
//
//  Created by Changyu Dong on 03/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_Util_h
#define PSI_Util_h
#include <stdint.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <assert.h>
#include "AESRandom.h"
#include "list.h"
#include "intHashTbl.h"
#include "param.h"
#include <unistd.h>

//xor len bytes in the two byte arrrays and store the result in the first array. Must ensure not overflow the buffer;
//inline void xorByteArray(uint8_t* inout, uint8_t* in2,uint32_t len){
//    for(uint32_t i=0;i<len;i++){
//        inout[i]^=in2[i];
//    }
//};

inline void xorByteArray(uint8_t* inOut, uint8_t* in2,uint32_t len) __attribute__((always_inline));
inline void xorByteArray(uint8_t* inOut, uint8_t* in2,uint32_t len){
    if(len==10){
        uint64_t* inOut64= (uint64_t*)inOut;
        uint64_t* in264=(uint64_t*)in2;
        inOut64[0]^=in264[0];
        inOut[8]^=in2[8];
        inOut[9]^=in2[9];
        return;
    }
    if(len==16){
        uint64_t* inOut64= (uint64_t*)inOut;
        uint64_t* in264=(uint64_t*)in2;
        inOut64[0]^=in264[0];
        inOut64[1]^=in264[1];
        return;
    }
    if(len==24){
        uint64_t* inOut64= (uint64_t*)inOut;
        uint64_t* in264=(uint64_t*)in2;
        inOut64[0]^=in264[0];
        inOut64[1]^=in264[1];
        inOut64[2]^=in264[2];
        return;
    }
    if(len==32){
        uint64_t* inOut64= (uint64_t*)inOut;
        uint64_t* in264=(uint64_t*)in2;
        inOut64[0]^=in264[0];
        inOut64[1]^=in264[1];
        inOut64[2]^=in264[2];
        inOut64[3]^=in264[3];
        return;
    }
    
    if(len==20){
        uint32_t* inOut32 =(uint32_t*)inOut;
        uint32_t* in232 = (uint32_t*)in2;
        inOut32[0]^=in232[0];
        inOut32[1]^=in232[1];
        inOut32[2]^=in232[2];
        inOut32[3]^=in232[3];
        inOut32[4]^=in232[4];
        return;
    }
    
    
    int32_t intLen= len>>2;
    int32_t r=len&3;
    
    if(r==0){
        uint32_t* inOut32 =(uint32_t*)inOut;
        uint32_t* in232 = (uint32_t*)in2;
        for(int32_t i=0;i<intLen;i++){
            inOut32[i]^=in232[i];
        }
    }else{
        
        for(uint32_t i=0;i<r;i++){
            inOut[i]^=in2[i];
        }
        uint32_t* inOut32 =(uint32_t*)(inOut+r);
        uint32_t* in232 = (uint32_t*)(in2+r);
        for(int32_t i=0;i<intLen;i++){
            inOut32[i]^=in232[i];
        }
    }
};

//compare len bytes in the two input byte arrays from the beginning. Return 0 is they are not equal, return 1 otherwise.
//inline int compareByteArray(uint8_t* in1, uint8_t* in2,uint32_t len){
//    for(uint32_t i=0;i<len;i++){
//        if(in1[i]!=in2[i]){
//            return 0;
//        }
//    }
//    return 1;
//    
//}

inline int compareByteArray(uint8_t* in1, uint8_t* in2,uint32_t len){
    int32_t intLen= len>>3;
    int32_t r=len&7;
    
    if(r==0){
        uint32_t* in132 =(uint32_t*)in1;
        uint32_t* in232 = (uint32_t*)in2;
        for(int32_t i=0;i<intLen;i++){
            if(in132[i]!=in232[i])
                return 0;
        }
        return 1;
    }else{
        
        for(uint32_t i=0;i<r;i++){
            if(in1[i]!=in2[i]){
                return 0;
            }
        }
        uint32_t* in132 =(uint32_t*)(in1+r);
        uint32_t* in232 = (uint32_t*)(in2+r);
        for(int32_t i=0;i<intLen;i++){
            if(in132[i]!=in232[i])
                return 0;
        }
        return 1;
    }
    
};

//how many leading zeroes in a byte array that stores len bits.
inline uint32_t getLeadingZeroes(uint32_t len){
    uint32_t r = len&7;
    if(r==0){
        return 0;
    }
    else{
        return 8-r;
    }
}
//how many bytes are needed to store len bits.
inline uint32_t getByteLenByBitLen(uint32_t len){
    uint32_t r = len&7;
    if(r==0){
        return len>>3;
    }
    else{
        return (len>>3)+1;
    }
};

inline void printBytes(uint8_t* bytes, uint32_t len){
    for(int i=0;i<len;i++){
        printf("%02X ", bytes[i]);
    }
    printf("\n");
};

inline void print2DBytes(uint8_t** bytes, uint32_t d1len, uint32_t d2len){
    for(int i=0;i<d1len;i++){
        printBytes(bytes[i], d2len);
    }
};


inline uint8_t* intToByteArray(int32_t* i){
    return (uint8_t*) i;
};

inline void setBit(uint8_t* bitMap, int32_t i, int32_t leadingZeroes){
    unsigned int intLoc = ((i+leadingZeroes)>>3);
    bitMap[intLoc] |= (1 << (7-((i+leadingZeroes) & 7)));
};

inline int getBit(uint8_t* bitMap, int32_t i, int32_t leadingZeroes){
        return (bitMap[(i+leadingZeroes) >> 3] & (1 << (7-((i+leadingZeroes) & 7))))==0?0:1;
};

//check the first i elements in indHis whether idx is already there
inline int exists(int32_t idx, int32_t* indHis, int32_t i){
    for (int j = 0; j < i; j++) {
        if (indHis[j] == idx)
            return 1;
    }
    return 0;
};

inline int num_cores()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
};

inline void receiveBulk(int sock, size_t toReceive, uint8_t* buf){
    size_t receivedLen=0;
    size_t step=1024;
    size_t recByte=0;
    for(receivedLen=0;receivedLen<toReceive;){
        if(receivedLen+step>=toReceive){
            step=toReceive-receivedLen;
        }
        recByte=recv(sock, buf+receivedLen, step, 0);
        assert(recByte>=0);
        receivedLen+=recByte;
    }
};

inline int32_t contains(uint8_t** set,uint8_t* e,int32_t setSize, int32_t eByteLen){
    for(int i=0;i<setSize;i++){
        if(compareByteArray(set[i], e, eByteLen)){
            return 1;
        }
    }
    return 0;
};

inline int32_t containsInt(int32_t* set,int32_t e,int32_t setSize){
    for(int i=0;i<setSize;i++){
        if(set[i]==e){
            return 1;
        }
    }
    return 0;
};

//Ls is an array of list, combine all element into L
inline void combineList(List* Ls,List* L,int32_t num){
    //printf("num=%d\n",num);
    for(int32_t i=0;i<num;i++){
        //printf("size of list %d is %d\n",i,Ls[i].size);
        if(Ls[i].size!=0){
            ListElmt* e= Ls[i].head;
            do{
                list_ins_next(L, NULL, e->data);
                e=e->next;
                //printf("size of combined list=%d",L->size);
            }while(e!=NULL);
        }
    }
}



void verifyInterSection(uint8_t** set,uint8_t** recved,int32_t setSize, List* L);

//generate a random set of n integers 
uint8_t** randomIntSet(int32_t n);
int32_t* randomIntArray(int32_t n);

int32_t* setToInt(uint8_t** set, int32_t n);

uint8_t** readInputFromFile(const char* path,int32_t* num);

void writeSetToFile(char* path,uint8_t** set,int32_t num);

void write2DByteArrayToFile(char* path,uint8_t** set,int32_t num,int32_t byteLen);

uint8_t** read2DByteArrayFromFile(const char* path,int32_t* num,int32_t colNum);

#endif
