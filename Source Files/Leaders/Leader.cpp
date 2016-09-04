//
// Created by root on 7/25/16.
//

#include <Util.h>
#include "Leaders/Leader.h"

void *Leader::checkElementsInThread(void *elInfo) {
    vector<uint32_t> intersection;

    ElementInfo *elementInfo = reinterpret_cast<ElementInfo*>(elInfo);

    PRINT() << "start pos " << elementInfo->startPos << std::endl;
    PRINT() << "end pos " << elementInfo->endPos << std::endl;
    for (uint32_t i = elementInfo->startPos; i < elementInfo->endPos; i++) {
        int32_t binIndex = elementInfo->hash[i].binIndex;
        int32_t tableIndex = elementInfo->hash[i].tableIndex;
        uint32_t hashFuncIndex = elementInfo->hash[i].hashedBy;

        if (binIndex==-1) {
            PRINT() << "Element " << i << "was not inserted to bin" << std::endl;
            continue;
        }

        if (tableIndex==-1) {
            PRINT() << "Element " << i << "was not inserted to hash table" << std::endl;
            continue;
        }

        uint8_t* secret = &elementInfo->secretShare[binIndex*elementInfo->maskSizeInBytes];

        if (elementInfo->leader->isElementInAllSets(i, binIndex, tableIndex, hashFuncIndex, secret, elementInfo->threadSpecificInfo)) {
            intersection.push_back(i);
        }
    }

    elementInfo->numFound = intersection.size();
}

uint32_t Leader::run() {

    receiveServerData();

    uint32_t cores = num_cores();
    PRINT() << "number of cores is " << cores << std::endl;

    ElementInfo *elementInfos = new ElementInfo[cores];
    vector<pthread_t> check_threads;

    for (uint32_t i=0; i < cores; i++) {
        pthread_t check_thread;
        elementInfos[i].hash = m_hashInfo.get();
        elementInfos[i].maskSizeInBytes = m_maskSizeInBytes;
        elementInfos[i].secretShare = m_secretShare.get();
        elementInfos[i].leader = this;
        elementInfos[i].startPos = i*m_setSize/cores;
        elementInfos[i].endPos = (i+1)*m_setSize/cores;
        if (i == (cores-1)) {
            elementInfos[i].endPos = m_setSize;
        }

        elementInfos[i].threadSpecificInfo = getSpecificThreadInfo();

        if(pthread_create(&check_thread, NULL, &Leader::checkElementsInThread, (void*)&elementInfos[i])) {
            cerr << "Error in creating new pthread at check element!" << endl;
            exit(0);
        }

        check_threads.push_back(check_thread);
    }

    for (auto &check_thread : check_threads) {
        //meanwhile generate the hash table
        //GHashTable* map = otpsi_create_hash_table(ceil_divide(inbitlen,8), masks, neles, maskbytelen, perm);
        //intersect_size = otpsi_find_intersection(eleptr, result, ceil_divide(inbitlen,8), masks, neles, server_masks,
        //		neles * NUM_HASH_FUNCTIONS, maskbytelen, perm);
        //wait for receiving thread
        if(pthread_join(check_thread, NULL)) {
            cerr << "Error in joining pthread at check element!" << endl;
            exit(0);
        }
    }

    uint32_t intersectionSize = 0;
    for (uint32_t i=0; i < cores; i++) {
        PRINT() << "Found " << elementInfos[i].numFound << std::endl;
        intersectionSize = intersectionSize + elementInfos[i].numFound;
    }

    /*
    for (auto &elementInfo : elementInfos) {
        freeSpecificThreadSpecificInfo(reinterpret_cast<void*>(elementInfo->threadSpecificInfo));
    }
    */
    return intersectionSize;
}