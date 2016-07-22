//
//  client.h
//  PSI
//
//  Created by Changyu Dong on 12/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_client_h
#define PSI_client_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
#include "networkComm.h"
#include "param.h"
#include "list.h"
#include "intHashTbl.h"
#include "clientTasks.h"


typedef struct Client{
    int32_t socket;
    //how many elements in the set
    int32_t n;
    //security level
    int32_t secLev;
    
    //set, array of bit arrays
    uint8_t** set;
    //hashes of elements in set
    uint8_t** eHash;
    //range hash keys
    uint8_t** keys;
    //n*k integers
    int32_t** indexes;
    
    //bf parameters
    BFParameters* bfParam;
    BF* filter;
    GarbledBF* recFilter;
    
    OTExtRcvr* rcvr;
    
    //booleans
    // multithreading or not
    int32_t multi;
    //verify result or not
    int32_t verify;
    //split OT into blocks or not
    int32_t partialTrafer;
    
    //number of threads
    int32_t threads;
    //number of blocks in OT
    int32_t parts;
    
    
    //paramters used if OT is split
    //bit length and byte length
    int32_t stepBit;
    int32_t stepByte;
    int32_t startBit;
    int32_t startByte;
    int32_t endBit;
    int32_t endByte;
    
    // multi threading parameters
    int32_t count;
    
    //int32_t querySize;
    //int32_t queryCount;
    
    
}Client;

void Client_runProtocolSingle(Client* clnt);

void Client_runProtocolMulti(Client* clnt);

#endif
