//
// Created by root on 7/21/16.
//

#include "NaiveLeader.h"
#include "common/defs.h"

vector<uint32_t> NaiveLeader::run() {
    vector<uint32_t> intersection;
    for (uint32_t i = 0; i < m_setSize; i++) {
        if (isElementInAllSets(i)) {
            // std::cout << "Input " << *(uint32_t*)(&m_elements[i]) << " is in the intersection" << std::endl;
            intersection.push_back(i);
            //intersection.push_back(*(uint32_t*)(&m_elements[i]));
        }
    }

    return intersection;
}

bool NaiveLeader::isElementInAllSets(uint32_t index) {

    uint32_t binIndex = 0;
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        if (m_binIds[i] == index + 1) {
            // std::cout << "Element number " << index << " was found at " << i << std::endl;
            binIndex = i;
            break;
        }
    }

    uint32_t newIndex = 0;
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        if (m_perm[i] == index) {
            newIndex = i;
            break;
        }
    }

    uint8_t* secret = &m_secretShare[binIndex*m_maskSizeInBytes];

    for (auto &party : m_parties) {
        XOR(secret, m_leaderResults[party.first-1]+newIndex*m_maskSizeInBytes, m_maskSizeInBytes);
    }

    // 1 is always the leader Id
    return isZeroXOR(secret,1);
}

bool NaiveLeader::isZeroXOR(uint8_t *formerShare, uint32_t partyNum) {
    if (partyNum < m_parties.size()+1) {
        uint8_t *partyResult = m_partiesResults[partyNum];
        for (uint32_t i = 0; i < m_setSize *m_numOfHashFunctions; i++) {
            XOR(formerShare,partyResult+i*m_maskSizeInBytes, m_maskSizeInBytes);
            if (isZeroXOR(formerShare,partyNum+1)) {
                return true;
            }
            XOR(formerShare,partyResult+i*m_maskSizeInBytes, m_maskSizeInBytes);
        }
        return false;
    }

    for (uint32_t i = 0; i < m_maskSizeInBytes; i++) {
        if (formerShare[i] != 0) {
            return false;
        }
    }
    return true;
};