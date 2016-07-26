//
// Created by root on 7/25/16.
//

#include "Leader.h"

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