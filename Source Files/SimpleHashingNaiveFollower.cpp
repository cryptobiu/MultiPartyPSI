//
// Created by root on 7/26/16.
//

#include "SimpleHashingNaiveFollower.h"
#include "ot-psi.h"

void SimpleHashingNaiveFollower::run() {
    xor_masks(m_followerSet.m_masks.get(), m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    uint32_t progress = 0;
    for (uint32_t k=0; k < m_followerSet.m_numOfBins; k++) {
        uint32_t numOfElementsInBin = m_followerSet.m_numOfElementsInBin.get()[k];
        if (numOfElementsInBin > m_followerSet.m_maxBinSize) {
            PRINT() << "ERROR MAX SIZE IN BIN IS NOT BIG ENOUGH !!!!";
        }
        uint32_t numOfRandomBytes = (m_followerSet.m_maxBinSize-numOfElementsInBin)*m_followerSet.m_maskSizeInBytes;
        boost::shared_ptr<uint8_t> random(new uint8_t[numOfRandomBytes]);
        RAND_bytes(random.get(),numOfRandomBytes);

        //send the masks to the receiver
        send_masks(m_followerSet.m_masks.get()+progress*m_followerSet.m_maskSizeInBytes, numOfElementsInBin, m_followerSet.m_maskSizeInBytes, m_leader);
        send_masks(random.get(), m_followerSet.m_maxBinSize-numOfElementsInBin, m_followerSet.m_maskSizeInBytes, m_leader);

        progress = progress + numOfElementsInBin;
    }

}