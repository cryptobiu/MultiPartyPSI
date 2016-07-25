//
// Created by root on 7/25/16.
//

#include "Leader.h"

uint32_t Leader::getBinIndex(uint32_t index) {
    uint32_t binIndex = 0;
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        if ((m_binIds.get())[i] == index + 1) {
            binIndex = i;
            break;
        }
    }
    return binIndex;
}

uint32_t Leader::getIndexInHashTable(uint32_t index) {
    uint32_t newIndex = 0;
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        if ((m_perm.get())[i] == index) {
            newIndex = i;
            break;
        }
    }
    return newIndex;
}

vector<uint32_t> Leader::run() {

    receiveServerData();

    vector<uint32_t> intersection;
    for (uint32_t i = 0; i < m_setSize; i++) {
        if (isElementInAllSets(i)) {
            intersection.push_back(i);
        }
    }

    return intersection;
}