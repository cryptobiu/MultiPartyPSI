//
//  server.h
//  PSI
//
//  Created by Changyu Dong on 12/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_server_h
#define PSI_server_h

#include <sys/socket.h>
#include <stdint.h>
#include <pthread.h>
#include "RangeHash.h"
#include "BF.h"
#include "GarbledBF.h"
#include "OTExtSndr.h"
#include "networkComm.h"
#include "serverTasks.h"
#include "MTBuilder.h"

typedef struct Server{
    int32_t socket;
    
    in_port_t port;
    int32_t n;
    int32_t secLev;
    uint8_t** set;
    uint8_t* masks;
    uint8_t** keys;
    RangeHash** hashFuncs;
    BFParameters* bfParam;
    GarbledBF* filter;
    OTExtSndr* sndr;
    
    int32_t multi;
    int32_t verify;
    int32_t partialTransfer;
    
    //number of threads
    int32_t threads;
    
    int32_t parts;
    
    int32_t stepBit;
    int32_t startBit;
    int32_t endBit;
    int32_t count;
    
    
}Server;


void Server_runProtocolMUlti(Server* sver);
void Server_runProtocolSingle(Server* sver);

#endif
