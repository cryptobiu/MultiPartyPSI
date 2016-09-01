//
// Created by root on 7/26/16.
//

#include <CryptoInfra/Key.hpp>
#include "SimpleHashingNaiveFollower.h"
#include "ot-psi.h"
#include "primitives/Prg.hpp"

#define KEY_SIZE 16

void SimpleHashingNaiveFollower::run() {
    xor_masks(m_followerSet.m_masks.get(), m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    uint8_t *seed = new uint8_t[KEY_SIZE];
    RAND_bytes(seed,KEY_SIZE);
    SecretKey key(seed, KEY_SIZE, "PrgFromOpenSSLAES");
    PrgFromOpenSSLAES prg(ceil_divide(m_followerSet.m_numOfBins*m_followerSet.m_maxBinSize*m_followerSet.m_maskSizeInBytes,16));
    prg.setKey(key);

    uint32_t progress = 0;
    for (uint32_t k=0; k < m_followerSet.m_numOfBins; k++) {
        uint32_t numOfElementsInBin = m_followerSet.m_numOfElementsInBin.get()[k];
        if (numOfElementsInBin > m_followerSet.m_maxBinSize) {
            PRINT() << "ERROR MAX SIZE IN BIN IS NOT BIG ENOUGH !!!!";
        }
        uint32_t numOfRandomBytes = (m_followerSet.m_maxBinSize-numOfElementsInBin)*m_followerSet.m_maskSizeInBytes;
        //send the masks to the receiver
        send_masks(m_followerSet.m_masks.get()+progress*m_followerSet.m_maskSizeInBytes, numOfElementsInBin, m_followerSet.m_maskSizeInBytes, m_leader);

        if (numOfRandomBytes != 0) {
            vector<uint8_t> random;
            prg.getPRGBytes(random, progress*m_followerSet.m_maskSizeInBytes, numOfRandomBytes);

            send_masks(random.data(), m_followerSet.m_maxBinSize-numOfElementsInBin, m_followerSet.m_maskSizeInBytes, m_leader);
        }

        progress = progress + numOfElementsInBin;
    }

}