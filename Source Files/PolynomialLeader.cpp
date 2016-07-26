//
// Created by root on 7/25/16.
//

#include "PolynomialLeader.h"
#include "PolynomialUtils.h"
#include <vector>
#include <NTL/GF2EX.h>
#include <NTL/GF2XFactoring.h>

PolynomialLeader::PolynomialLeader(const map <uint32_t, boost::shared_ptr<uint8_t>> &leaderResults,
                                   const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
                 const boost::shared_ptr <uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                 boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
                 const std::map <uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions) :
        Leader(leaderResults,hashInfo,numOfBins,secretShare,maskSizeInBytes,setSize,elements,
               elementSize,parties,numOfHashFunctions) {

    uint32_t securityParameter = maskSizeInBytes*8;
    GF2X irreduciblePolynomial = BuildSparseIrred_GF2X(securityParameter);
    //std::cout << "Leader irreducible Polynomial is " << irreduciblePolynomial << std::endl;
    GF2E::init(irreduciblePolynomial);

}

void PolynomialLeader::receiveServerData() {

    boost::shared_ptr<polynomial_rcv_ctx> rcv_ctxs(new polynomial_rcv_ctx[m_parties.size()+1]);
    for (auto &party : m_parties) {

        //receive_masks(server_masks, NUM_HASH_FUNCTIONS * neles, maskbytelen, sock[0]);
        //use a separate thread to receive the server's masks
        (rcv_ctxs.get())[party.first - 1].polynoms = new NTL::GF2EX*[m_numOfHashFunctions];

        (rcv_ctxs.get())[party.first - 1].numPolynomPerHashFunc = 1;
        (rcv_ctxs.get())[party.first - 1].polynomSize = m_setSize;
        (rcv_ctxs.get())[party.first - 1].numOfHashFunction = m_numOfHashFunctions;
        (rcv_ctxs.get())[party.first - 1].maskbytelen = m_maskSizeInBytes;
        (rcv_ctxs.get())[party.first - 1].sock = party.second.get();
    }

    receiveServerDataInThreads<polynomial_rcv_ctx>(rcv_ctxs, &PolynomialLeader::receivePolynomials);

    for (auto &party : m_parties) {
        m_partiesPolynomials[party.first] = std::vector<boost::shared_ptr<GF2EX>>();
        NTL::GF2EX** polynoms = (rcv_ctxs.get())[party.first - 1].polynoms;
        for (uint32_t i=0; i < m_numOfHashFunctions; i++) {
            m_partiesPolynomials[party.first].push_back(boost::shared_ptr<GF2EX>(polynoms[i]));
        }
    }
}

bool PolynomialLeader::isElementInAllSets(uint32_t index, uint32_t binIndex, uint32_t tableIndex, uint32_t hashFuncIndex, uint8_t *secret) {

    for (auto &party : m_parties) {
        XOR(secret, m_leaderResults[party.first].get()+tableIndex*m_maskSizeInBytes, m_maskSizeInBytes);

        NTL::GF2E element = PolynomialUtils::convertBytesToGF2E(m_elements.get()+index*m_elementSize,m_elementSize);
        GF2E value = eval(*(m_partiesPolynomials[party.first][hashFuncIndex].get()),element);
        vector<uint8_t> arr = PolynomialUtils::convertElementToBytes(value);

        XOR(secret, arr.data(), m_maskSizeInBytes);
    }

    return isZero(secret, m_maskSizeInBytes);
}

void *PolynomialLeader::receivePolynomials(void *ctx_tmp) {

    polynomial_rcv_ctx* ctx = (polynomial_rcv_ctx*) ctx_tmp;

    uint32_t coefficientSize = ctx->maskbytelen;

    for (uint32_t i = 0; i < ctx->numOfHashFunction; i++) {
        for (uint32_t k =0; k < ctx->numPolynomPerHashFunc; k++) {
            ctx->polynoms[i*ctx->numPolynomPerHashFunc+k] = new NTL::GF2EX();
            for (uint32_t j=0; j < ctx->polynomSize; j++) {
                vector<uint8_t> coefficient(coefficientSize);
                ctx->sock->Receive(coefficient.data(), coefficientSize);
                NTL::GF2E coeffElement = PolynomialUtils::convertBytesToGF2E(coefficient.data(), coefficientSize);
                SetCoeff(*ctx->polynoms[i*ctx->numPolynomPerHashFunc+k], j, coeffElement);
            }
        }
    }
}