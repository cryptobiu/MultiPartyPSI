//
// Created by root on 7/21/16.
//

#include "NaiveFollower.h"
#include "ot-psi.h"

void NaiveFollower::run() {
    xor_masks(m_followerSet.m_hashTable.get(), m_followerSet.m_elements.get(), m_followerSet.m_numOfElements, m_followerSet.m_masks.get(), m_followerSet.m_elementSizeInBytes,
              m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    //send the masks to the receiver
    send_masks(m_followerSet.m_masks.get(), m_followerSet.m_numOfElements * m_followerSet.m_numOfHashFunctions, m_followerSet.m_maskSizeInBytes, m_leader);
}