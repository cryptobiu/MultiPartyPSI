//
// Created by root on 7/26/16.
//

#include "SimpleHashingNaiveFollower.h"
#include "ot-psi.h"

void SimpleHashingNaiveFollower::run() {
    xor_masks(m_followerSet.m_masks.get(), m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    //send the masks to the receiver
    send_masks(m_followerSet.m_masks.get(), m_followerSet.m_numOfElements * m_followerSet.m_numOfHashFunctions, m_followerSet.m_maskSizeInBytes, m_leader);
}