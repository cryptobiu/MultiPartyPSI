//
// Created by root on 7/25/16.
//

#include "Leader.h"

vector<uint32_t> Leader::run() {

    receiveServerData();

    vector<uint32_t> intersection;
    for (uint32_t i = 0; i < m_setSize; i++) {

        uint32_t binIndex = m_hashInfo.get()[i].binIndex;
        uint32_t tableIndex = m_hashInfo.get()[i].tableIndex;
        uint32_t hashFuncIndex = m_hashInfo.get()[i].hashedBy;

        uint8_t* secret = &(m_secretShare.get()[binIndex*m_maskSizeInBytes]);

        if (isElementInAllSets(i, binIndex, tableIndex, hashFuncIndex, secret)) {
            intersection.push_back(i);
        }
    }

    return intersection;
}