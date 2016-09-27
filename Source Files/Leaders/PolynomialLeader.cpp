//
// Created by root on 7/25/16.
//

#include "Leaders/PolynomialLeader.h"
#include "PolynomialUtils.h"
#include <vector>
#include <NTL/GF2EX.h>
#include <NTL/GF2XFactoring.h>

PolynomialLeader::PolynomialLeader(const map <uint32_t, boost::shared_ptr<uint8_t>> &leaderResults,
                                   const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
                 const boost::shared_ptr <uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                 boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
                 const std::map <uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions, const secParameters &parameters, uint32_t numCores) :
        Leader(leaderResults,hashInfo,numOfBins,secretShare,maskSizeInBytes,setSize,elements,
               elementSize,parties,numOfHashFunctions, parameters, numCores) {

    m_irreduciblePolynomial = BuildSparseIrred_GF2X(m_parameters.m_statSecParameter);
    //std::cout << "Leader irreducible Polynomial is " << irreduciblePolynomial << std::endl;
    GF2E::init(m_irreduciblePolynomial);

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
        (rcv_ctxs.get())[party.first - 1].irreduciblePolynomial = &m_irreduciblePolynomial;
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

bool PolynomialLeader::isElementInAllSets(uint32_t index, uint32_t binIndex, uint32_t tableIndex, uint32_t hashFuncIndex, uint8_t *secret, bf_info *specInfo) {

    GF2E::init(m_irreduciblePolynomial);

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

    GF2E::init(*(ctx->irreduciblePolynomial));

    uint32_t coefficientSize = ctx->maskbytelen;

    uint8_t *masks = (uint8_t* )calloc(ctx->maskbytelen*ctx->numOfHashFunction*ctx->numPolynomPerHashFunc*ctx->polynomSize,sizeof(uint8_t));

    ctx->sock->Receive(masks, ctx->maskbytelen*ctx->numOfHashFunction*ctx->numPolynomPerHashFunc*ctx->polynomSize);

    for (uint32_t i = 0; i < ctx->numOfHashFunction; i++) {
        PRINT() << "receive polynomial " << i << std::endl;
        for (uint32_t k =0; k < ctx->numPolynomPerHashFunc; k++) {
            ctx->polynoms[i*ctx->numPolynomPerHashFunc+k] = new NTL::GF2EX();
            for (uint32_t j=0; j < ctx->polynomSize; j++) {

                NTL::GF2E coeffElement = PolynomialUtils::convertBytesToGF2E(
                        masks+((i*ctx->numPolynomPerHashFunc+k)*ctx->polynomSize+j)*coefficientSize, coefficientSize);
                SetCoeff(*ctx->polynoms[i*ctx->numPolynomPerHashFunc+k], j, coeffElement);
            }
        }
    }

    free(masks);
}