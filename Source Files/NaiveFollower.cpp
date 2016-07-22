//
// Created by root on 7/21/16.
//

#include "../Header Files/NaiveFollower.h"
#include "../PSI/src/ot-based/ot-psi.h"

void NaiveFollower::run() {
    xor_masks(m_followerSet.m_hashTable, m_followerSet.m_elements, m_followerSet.m_numOfElements, m_followerSet.m_masks, m_followerSet.m_elementSizeInBytes,
              m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin);

    //send the masks to the receiver
    send_masks(m_followerSet.m_masks, m_followerSet.m_numOfElements * m_followerSet.m_numOfHashFunctions, m_followerSet.m_maskSizeInBytes, m_leader);
}