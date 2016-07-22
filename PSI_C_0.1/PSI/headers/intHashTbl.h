//
//  intHashTbl.h
//  PSI
//
//  Created by Changyu Dong on 11/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_intHashTbl_h
#define PSI_intHashTbl_h

#include "chtbl.h"
#include <stdint.h>
static inline int match_int(const void *int1, const void *int2) {
    
    return (*(const int32_t *)int1 == *(const int32_t *)int2);
    
}

static inline int hash_int(const void *key) {
    return (*(const int32_t *)key>=0)?*(const int32_t *)key:-(*(const int32_t *)key);
}

inline int intHash_init(CHTbl* htbl,int buckets){
    return chtbl_init(htbl, buckets, hash_int, match_int, free);
}

inline int intHash_insert(CHTbl* htbl, int32_t e){
    int32_t* data = malloc(sizeof(int32_t));
    data[0]=e;
    return chtbl_insert(htbl, data);
}

inline int intHash_lookup(CHTbl* htbl, int32_t e){
    int32_t* data = malloc(sizeof(int32_t));
    data[0]=e;
    return chtbl_lookup(htbl, (void**)&data);
}

#endif
