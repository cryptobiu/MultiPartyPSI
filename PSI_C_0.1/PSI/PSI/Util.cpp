//
//  Util.c
//  PSI
//
//  Created by Changyu Dong on 12/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>
#include "Util.h"


void writeSetToFile(char* path,uint8_t** set,int32_t num){
    FILE* f;
    f=fopen(path, "w");
    
    for(int32_t i=0;i<num;i++){
        fwrite(set[i], 1, 4, f);
    }
    
    fclose(f);
    
}
void printBAF(uint8_t* str, uint8_t byteLen, FILE* f){
    
    for(int i=0;i<byteLen;i++){
        fprintf(f,"%02X ", str[i]);
    }
    fprintf(f,"\n");
    
}
void write2DByteArrayToFile(char* path,uint8_t** set,int32_t num,int32_t byteLen){
    FILE* f;
    f=fopen(path, "w");
    
    for(int32_t i=0;i<num;i++){
        printBAF(set[i],byteLen,f);
        //fwrite(set[i], 1, byteLen, f);
    }
    
    fclose(f);
    
}

uint8_t** readInputFromFile(const char* path,int32_t* num){
    FILE* f;
    int32_t size;
    
    f=fopen(path, "r");
    
    assert(f!=NULL);
    
    fseek(f, 0L, SEEK_END);
    size=ftell(f);
    
    fseek(f, 0L, SEEK_SET);
    
    int32_t n=size/4;
    
    uint8_t** out =(uint8_t**)calloc(n, sizeof(uint8_t*));
    
    for(int32_t i=0;i<n;i++){
        uint8_t* e= (uint8_t*)calloc(4, sizeof(uint8_t));
        fread(e, 1, 4, f);
        out[i]=e;
    }
    
    fclose(f);
    *num=n;
    return out;
}

uint8_t** read2DByteArrayFromFile(const char* path,int32_t* num,int32_t colNum){
    FILE* f;
    int32_t size;
    
    f=fopen(path, "r");
    
    assert(f!=NULL);
    
    fseek(f, 0L, SEEK_END);
    size=ftell(f);
    
    fseek(f, 0L, SEEK_SET);
    
    int32_t colByteLen=size/colNum;
    
    uint8_t** out =(uint8_t**)calloc(colNum, sizeof(uint8_t*));
    
    for(int32_t i=0;i<colNum;i++){
        uint8_t* e= (uint8_t*)calloc(colByteLen, sizeof(uint8_t));
        fread(e, 1, colByteLen, f);
        out[i]=e;
    }
    
    fclose(f);
    *num=colByteLen;
    return out;
}

int32_t* setToInt(uint8_t** set, int32_t n){
    int32_t* out=(int32_t*)calloc(n, sizeof(int32_t));
    for(int i=0;i<n;i++){
        out[i]= *(int32_t*)set[i];
    }
    return out;
}

uint8_t** randomIntSet(int32_t n){
    AESRandom * rnd=NULL;
    AESRandom_Create(&rnd, 16);
    
    CHTbl htbl;
    
    intHash_init(&htbl, n);
    
    uint8_t** out = (uint8_t**)calloc(n, sizeof(uint8_t*));
    assert(out!=NULL);
    for(int32_t i=0;i<n;i++){
        uint8_t* e= (uint8_t*)calloc(4, sizeof(uint8_t));
        assert(e!=NULL);
        int32_t s=0;
        do{
            s=AESRandom_NextInt(rnd);
        }while(intHash_lookup(&htbl, s)==0);
        intHash_insert(&htbl, s);
        memcpy(e, &s, 4);
        out[i]=e;
    }
    
    //    for(int32_t i=0;i<n;i++){
    //        printf("%d\n",*(int32_t*)out[i]);
    //    }
    AESRandom_Destroy(rnd);
    //chtbl_destroy(&htbl);
    return out;
}

int32_t* randomIntArray(int32_t n){
    AESRandom * rnd=NULL;
    AESRandom_Create(&rnd, 16);
    
    CHTbl htbl;
    
    intHash_init(&htbl, n);
    
    int32_t* out = (int32_t*)calloc(n, sizeof(int32_t));
    assert(out!=NULL);
    for(int32_t i=0;i<n;i++){
        //uint8_t* e= calloc(4, sizeof(uint8_t));
        //assert(e!=NULL);
        int32_t s=0;
        do{
            s=AESRandom_NextInt(rnd);
        }while(intHash_lookup(&htbl, s)==0);
        intHash_insert(&htbl, s);
       // memcpy(e, &s, 4);
        out[i]=s;
    }
    
    //    for(int32_t i=0;i<n;i++){
    //        printf("%d\n",*(int32_t*)out[i]);
    //    }
    AESRandom_Destroy(rnd);
    //chtbl_destroy(&htbl);
    return out;
}

//uint8_t** randomIntSet(int32_t n){
//    AESRandom * rnd=NULL;
//    AESRandom_Create(&rnd, 16);
//    
//    uint8_t** out = calloc(n, sizeof(uint8_t*));
//    assert(out!=NULL);
//    for(int32_t i=0;i<n;i++){
//        uint8_t* e= calloc(4, sizeof(uint8_t));
//        assert(e!=NULL);
//        int32_t s=AESRandom_NextInt(rnd);
//        memcpy(e, &s, 4);
//        out[i]=e;
//    }
//    
//    //    for(int32_t i=0;i<n;i++){
//    //        printf("%d\n",*(int32_t*)out[i]);
//    //    }
//    AESRandom_Destroy(rnd);
//    return out;
//}

void verifyInterSection(uint8_t** set,uint8_t** recved,int32_t setSize, List* L){
    
    int32_t* setC = setToInt(set, setSize);
    int32_t* setS= setToInt(recved, setSize);
    
    CHTbl htbl;
    
    intHash_init(&htbl, setSize);
    
    for(int i=0;i<setSize;i++){
        //printf("%d,%d\n",i,setC[i]);
        intHash_insert(&htbl, setC[i]);
    }
    
    List L2;
    list_init(&L2, &free);
    
    for(int i=0;i<setSize;i++){
        //in the intersection
        if(intHash_lookup(&htbl, setS[i])==0){
            list_ins_next(&L2, NULL, recved[i]);
        };
    }
    
    printf("size of L2=%d\n",list_size(&L2));
    printf("size of L1=%d\n",list_size(L));
    
    if(list_size(&L2)!=list_size(L)){
        printf("Incorrect intersection!\n");
        abort();
    }
    
    uint8_t** int1=(uint8_t**)calloc(list_size(L), sizeof(uint8_t*));
    uint8_t** int2=(uint8_t**)calloc(list_size(&L2), sizeof(uint8_t*));
    
    ListElmt * e1 = L->head;
    ListElmt * e2= L2.head;
    
    for(int i=0;i<list_size(&L2);i++){
        int1[i]=(uint8_t*)e1->data;
        int2[i]=(uint8_t*)e2->data;
        e1=e1->next;
        e2=e2->next;
    }
    
    
    for(int i=0;i<list_size(&L2);i++){
        if(!contains(int1,int2[i],list_size(&L2),defaultDatalen)){
            printf("Incorrect intersection!\n");
            //abort();
        }
    }
    
    printf("Intersection correct!\n");
    printf("Should contain:\n");
    for(int i=0;i<list_size(&L2);i++){
        printBytes(int2[i], defaultDatalen);
    }
    
    printf("Contains:\n");
    for(int i=0;i<list_size(&L2);i++){
        printBytes(int1[i], defaultDatalen);
    }
    
}