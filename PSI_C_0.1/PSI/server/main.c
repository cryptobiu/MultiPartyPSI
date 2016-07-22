//
//  main.c
//  test
//
//  Created by Changyu Dong on 08/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

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

#include "param.h"

#include "server.h"
Server* initServer(int argc, const char * argv[]);

void start(Server* sevr);

int main(int argc, const char * argv[])
{

    // insert code here...
    printf("Initialising Server\n");
    Server* sevr= initServer(argc, argv);
    
    start(sevr);
    close(sevr->socket);
}

void start(Server* sver){
    int32_t servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    int iSetOption=1;
    setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &iSetOption, sizeof(iSetOption));
    
    assert(servSock>=0);
    
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family=AF_INET;
    servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servAddr.sin_port=htons(sver->port);
    
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
    
    sver->socket=cliSock;
    
    printf("Client connected\n");
    
    if(sver->multi){
        Server_runProtocolMUlti(sver);
        
    }else{
        Server_runProtocolSingle(sver);
    }
    
}

