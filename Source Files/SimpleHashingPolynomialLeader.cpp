//
// Created by root on 7/26/16.
//

#include "SimpleHashingPolynomialLeader.h"
#include <NTL/GF2EX.h>
#include <PolynomialUtils.h>

void SimpleHashingPolynomialLeader::receiveServerData() {

    boost::shared_ptr<polynomial_rcv_ctx> rcv_ctxs(new polynomial_rcv_ctx[m_parties.size()+1]);
    for (auto &party : m_parties) {

        //receive_masks(server_masks, NUM_HASH_FUNCTIONS * neles, maskbytelen, sock[0]);
        //use a separate thread to receive the server's masks
        (rcv_ctxs.get())[party.first - 1].polynoms = new NTL::GF2EX*[m_numOfHashFunctions*m_numOfBins];

        (rcv_ctxs.get())[party.first - 1].numPolynomPerHashFunc = m_numOfBins;
        (rcv_ctxs.get())[party.first - 1].polynomSize = m_maxBinSize;
        (rcv_ctxs.get())[party.first - 1].numOfHashFunction = m_numOfHashFunctions;
        (rcv_ctxs.get())[party.first - 1].maskbytelen = m_maskSizeInBytes;
        (rcv_ctxs.get())[party.first - 1].sock = party.second.get();
    }

    receiveServerDataInThreads<polynomial_rcv_ctx>(rcv_ctxs, &PolynomialLeader::receivePolynomials);

    for (auto &party : m_parties) {
        m_partiesPolynomials[party.first] = std::vector<boost::shared_ptr<GF2EX>>();
        NTL::GF2EX** polynoms = (rcv_ctxs.get())[party.first - 1].polynoms;
        for (uint32_t i=0; i < m_numOfHashFunctions*m_numOfBins; i++) {
            m_partiesPolynomials[party.first].push_back(boost::shared_ptr<GF2EX>(polynoms[i]));
        }
    }
}

bool SimpleHashingPolynomialLeader::isElementInAllSets(uint32_t index, uint32_t binIndex, uint32_t tableIndex, uint32_t hashFuncIndex, uint8_t *secret) {

    for (auto &party : m_parties) {
        XOR(secret, m_leaderResults[party.first].get()+tableIndex*m_maskSizeInBytes, m_maskSizeInBytes);

        NTL::GF2E element = PolynomialUtils::convertBytesToGF2E(m_elements.get()+index*m_elementSize,m_elementSize);
        GF2E value = eval(*(m_partiesPolynomials[party.first][hashFuncIndex*m_numOfBins+binIndex].get()),element);
        vector<uint8_t> arr = PolynomialUtils::convertElementToBytes(value);
        XOR(secret, arr.data(), m_maskSizeInBytes);
    }

    return isZero(secret, m_maskSizeInBytes);
}
