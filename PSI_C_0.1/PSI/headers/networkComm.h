//
//  networkComm.h
//  PSI
//
//  Created by Changyu Dong on 12/03/2013.
//  Copyright (c) 2013 Changyu Dong. All rights reserved.
//

#ifndef PSI_networkComm_h
#define PSI_networkComm_h
#include <sys/types.h>
#include <sys/socket.h>
#include <openssl/ec.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "Util.h"

uint8_t** receiveArrayOfByteStrings(int32_t sock, int32_t numOfStrings, int32_t strByteLen);

void sendArrayOfByteStrings(int32_t sock, int32_t numOfStrings, int32_t strByteLen, uint8_t** strArray);

void send3DBitArray(int32_t sock, int32_t size1D, int32_t size2D, int32_t size3D,uint8_t*** strArray);

uint8_t*** receive3DBitArray(int32_t sock, int32_t size1D, int32_t size2D, int32_t size3D);

void sendArrayOfECPoints(int32_t sock, int32_t numOfPoints, int32_t pointByteLen, EC_GROUP* curve, EC_POINT** points);

EC_POINT** receiveArrayOfECPoints(int32_t socket, int32_t numOfPoints, int32_t pointByteLen, EC_GROUP* curve);
#endif
