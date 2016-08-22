//
// Created by root on 7/26/16.
//

#include "SimpleHashingPolynomialFollower.h"
#include "PolynomialUtils.h"

void SimpleHashingPolynomialFollower::buildPolynomials(){

    for (uint32_t i = 0; i < m_followerSet.m_numOfHashFunctions; i++) {
        uint32_t elementIndex = 0;

        for (uint32_t k=0; k < m_followerSet.m_numOfBins; k++) {
            vec_GF2E inputs;
            vec_GF2E masks;

            uint32_t numOfElementsInBin = m_followerSet.m_numOfElementsInBin.get()[k];
            uint32_t numOfElementsOfHashInBin = 0;
            for (uint32_t j=elementIndex; j < elementIndex+numOfElementsInBin; j++) {
                if ((m_followerSet.m_binToElementsToHashTable.get()[j] % m_followerSet.m_numOfHashFunctions) != i) {
                    continue;
                }
                numOfElementsOfHashInBin++;

                uint32_t elementIndex = m_followerSet.m_binToElementsToHashTable.get()[j] / m_followerSet.m_numOfHashFunctions;

                NTL::GF2E input = PolynomialUtils::convertBytesToGF2E(m_followerSet.m_realElements.get()+elementIndex*m_followerSet.m_elementSizeInBytes, m_followerSet.m_elementSizeInBytes);
                inputs.append(input);

                NTL::GF2E mask = PolynomialUtils::convertBytesToGF2E(m_followerSet.m_masks.get()+j*m_followerSet.m_maskSizeInBytes, m_followerSet.m_maskSizeInBytes);
                masks.append(mask);
            }

            if (numOfElementsOfHashInBin > m_followerSet.m_maxBinSize) {
                PRINT() << "ERROR MAX SIZE IN BIN IS NOT BIG ENOUGH !!!!";
            }

            uint32_t numOfRandomElements = m_followerSet.m_maxBinSize-numOfElementsOfHashInBin;
            for (uint32_t j=0; j < numOfRandomElements; j++) {
                inputs.append(random_GF2E());
                masks.append(random_GF2E());
            }

            GF2EX polynomial = interpolate(inputs, masks);
            m_polynomials.push_back(polynomial);

            elementIndex = elementIndex + numOfElementsInBin;
        }
    }
}