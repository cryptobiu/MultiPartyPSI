//
//  MessageDigest.h
//  PSI
//
//  Created by Changyu Dong on 06/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_MessageDigest_h
#define PSI_MessageDigest_h

#include <openssl/sha.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

typedef struct MessageDigest{
    int32_t digestLen;
    void* ctx;
    int (*Init)(void* c);
    int (*update)(void* c, const void* data, size_t len);
    int (*Final)(uint8_t* md, void* c);
}MessageDigest;

int MD_Create(MessageDigest** md, int32_t strength);

void MD_Digest(MessageDigest* md,const void* data, int32_t len, uint8_t* out);
void MD_Destroy(MessageDigest* md);
MessageDigest* getHashInstance(int32_t k);

#endif
