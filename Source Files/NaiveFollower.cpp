//
// Created by root on 7/21/16.
//

#include "NaiveFollower.h"
#include "ot-psi.h"

void NaiveFollower::run() {
    xor_masks(m_followerSet.m_masks.get(), m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    boost::shared_ptr<uint8_t> ordered(new uint8_t[m_followerSet.m_numOfElements * m_followerSet.m_numOfHashFunctions*m_followerSet.m_maskSizeInBytes]);

    uint8_t *ptr = ordered.get();

    for (uint32_t i = 0; i < m_followerSet.m_numOfHashFunctions; i++) {
        for (uint32_t j = 0; j < m_followerSet.m_numOfElements; j++) {
            uint32_t index = m_followerSet.m_elements_to_hash_table.get()[j * m_followerSet.m_numOfHashFunctions + i];

            memcpy(ptr, m_followerSet.m_masks.get() + index * m_followerSet.m_maskSizeInBytes, m_followerSet.m_maskSizeInBytes);
            ptr = ptr + m_followerSet.m_maskSizeInBytes;
        }
    }
    //send the masks to the receiver
    send_masks(ordered.get(), m_followerSet.m_numOfElements * m_followerSet.m_numOfHashFunctions, m_followerSet.m_maskSizeInBytes, m_leader);
}