//
// Created by root on 7/26/16.
//

#include <CryptoInfra/Key.hpp>
#include "Followers/SimpleHashingNaiveFollower.h"
#include "ot-psi.h"
#include "primitives/Prg.hpp"

#define KEY_SIZE 16

void SimpleHashingNaiveFollower::run() {
    xor_masks(m_followerSet.m_masks.get(), m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    uint8_t *seed = new uint8_t[KEY_SIZE];
    RAND_bytes(seed,KEY_SIZE);
    SecretKey key(seed, KEY_SIZE, "PrgFromOpenSSLAES");
    PrgFromOpenSSLAES prg(ceil_divide(m_followerSet.m_numOfBins*m_followerSet.m_numOfHashFunctions*m_followerSet.m_maxBinSize*m_followerSet.m_maskSizeInBytes,16));
    prg.setKey(key);

    vector<uint8_t> *random = new vector<uint8_t>[m_followerSet.m_numOfHashFunctions];
    uint32_t *counter = new uint32_t[m_followerSet.m_numOfHashFunctions];
    for (uint i=0; i < m_followerSet.m_numOfHashFunctions; i++) {
        prg.getPRGBytes(random[i], 0, m_followerSet.m_numOfBins*m_followerSet.m_maxBinSize*m_followerSet.m_maskSizeInBytes);
    }


    uint32_t progress = 0;
    for (uint32_t k=0; k < m_followerSet.m_numOfBins; k++) {
        uint32_t numOfElementsInBin = m_followerSet.m_numOfElementsInBin.get()[k];
        if (numOfElementsInBin > m_followerSet.m_numOfHashFunctions*m_followerSet.m_maxBinSize) {
            PRINT() << "ERROR MAX SIZE IN BIN IS NOT BIG ENOUGH !!!!";
        }

        for (uint i=0; i < m_followerSet.m_numOfHashFunctions; i++) {
            counter[i]=0;
        }

        for (uint32_t i = 0; i < numOfElementsInBin; i++) {
            uint32_t hashIndex = m_followerSet.m_binToElementsToHashTable.get()[progress+i] % m_followerSet.m_numOfHashFunctions;
            //send the masks to the receiver
            memcpy(random[hashIndex].data()+ (k*m_followerSet.m_maxBinSize+counter[hashIndex])*m_followerSet.m_maskSizeInBytes,
                   m_followerSet.m_masks.get()+(progress+i)*m_followerSet.m_maskSizeInBytes, m_followerSet.m_maskSizeInBytes);
            counter[hashIndex]++;
        }

        progress = progress + numOfElementsInBin;
    }

    for (uint i=0; i < m_followerSet.m_numOfHashFunctions; i++) {
        send_masks(random[i].data(), m_followerSet.m_numOfBins*m_followerSet.m_maxBinSize, m_followerSet.m_maskSizeInBytes, m_leader);
    }

    delete[] random;
    delete[] counter;
}