//
//  MessageDigest.c
//  PSI
//
//  Created by Changyu Dong on 06/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#include <stdio.h>

#include "MessageDigest.h"
int MD_Create(MessageDigest** md, int32_t strength){
    assert(strength==80||strength==128||strength==192||strength==256);
    
    MessageDigest* newMd= malloc(sizeof(MessageDigest));
    
    assert(newMd!=NULL);
    
    if(strength==80){
        //sha1
        SHA_CTX* ctx=malloc(sizeof(SHA_CTX));
        assert(ctx!=NULL);
        
        newMd->ctx=ctx;
        newMd->Init=&SHA1_Init;
        newMd->update=&SHA1_Update;
        newMd->Final=&SHA1_Final;
        newMd->digestLen=SHA_DIGEST_LENGTH;
    }else if(strength==128){
        //sha256
        SHA256_CTX* ctx=malloc(sizeof(SHA256_CTX));
        assert(ctx!=NULL);
        newMd->ctx=ctx;
        newMd->Init=&SHA256_Init;
        newMd->update=&SHA256_Update;
        newMd->Final=&SHA256_Final;
        newMd->digestLen=SHA256_DIGEST_LENGTH;
        
    }else if(strength==192){
        //sha384
        SHA512_CTX* ctx=malloc(sizeof(SHA512_CTX));
        assert(ctx!=NULL);
        newMd->ctx=ctx;
        newMd->Init=&SHA384_Init;
        newMd->update=&SHA384_Update;
        newMd->Final=&SHA384_Final;
        newMd->digestLen=SHA384_DIGEST_LENGTH;
        
    }else if(strength==256){
        //sha512
        SHA512_CTX* ctx=malloc(sizeof(SHA512_CTX));
        assert(ctx!=NULL);
        newMd->ctx=ctx;
        newMd->Init=&SHA512_Init;
        newMd->update=&SHA512_Update;
        newMd->Final=&SHA512_Final;
        newMd->digestLen=SHA512_DIGEST_LENGTH;
    }
    *md=newMd;
    return 1;
}

void MD_Digest(MessageDigest* md,const void* data, int32_t len, uint8_t* out){
    md->Init(md->ctx);
    md->update(md->ctx,data,len);
    md->Final(out,md->ctx);
}

void MD_Destroy(MessageDigest* md){
    free(md->ctx);
    free(md);
}
MessageDigest* getHashInstance(int32_t k){
    assert(k==80||k==128||k==192||k==256);
    MessageDigest* md;
    
    MD_Create(&md, k);
    return md;
}
