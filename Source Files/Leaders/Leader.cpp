//
// Created by root on 7/25/16.
//

#include <Util.h>
#include "Leaders/Leader.h"

void *Leader::checkElementsInThread(void *elInfo) {
    vector<uint32_t> intersection;

    ElementInfo *elementInfo = reinterpret_cast<ElementInfo*>(elInfo);
    bf_info *threadSpecificInfo = elementInfo->leader->getSpecificThreadInfo();

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

        if (elementInfo->leader->isElementInAllSets(i, binIndex, tableIndex, hashFuncIndex, secret, threadSpecificInfo)) {
            intersection.push_back(i);
        }
    }

    elementInfo->leader->freeSpecificThreadSpecificInfo(reinterpret_cast<void*>(threadSpecificInfo));

    elementInfo->numFound = intersection.size();
}

uint32_t Leader::run() {

    receiveServerData();

    vector<boost::shared_ptr<ElementInfo>> elementInfos;
    vector<pthread_t> check_threads;

    uint32_t numCores = min(m_numCores, m_setSize);
    for (uint32_t i=0; i < numCores; i++) {
        pthread_t check_thread;
        boost::shared_ptr<ElementInfo> elementInfo(new ElementInfo);
        elementInfo->hash = m_hashInfo.get();
        elementInfo->maskSizeInBytes = m_maskSizeInBytes;
        elementInfo->secretShare = m_secretShare.get();
        elementInfo->leader = this;
        elementInfo->startPos = i*m_setSize/numCores;
        elementInfo->endPos = (i+1)*m_setSize/numCores;
        if (i == (numCores-1)) {
            elementInfo->endPos = m_setSize;
        }

        if(pthread_create(&check_thread, NULL, &Leader::checkElementsInThread, (void*)elementInfo.get())) {
            cerr << "Error in creating new pthread at check element!" << endl;
            exit(0);
        }

        check_threads.push_back(check_thread);
        elementInfos.push_back(elementInfo);
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
    for (uint32_t i=0; i < numCores; i++) {
        PRINT() << "Found " << elementInfos[i]->numFound << std::endl;
        intersectionSize = intersectionSize + elementInfos[i]->numFound;
    }

    return intersectionSize;
}