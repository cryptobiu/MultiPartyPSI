//
// Created by root on 7/25/16.
//

#include "../Header Files/PolynomialFollower.h"
#include "PolynomialUtils.h"
#include "../PSI/src/ot-based/ot-psi.h"

PolynomialFollower::PolynomialFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader) :
        Follower(followerSet, secretShare, leader) {

    m_securityParameter = m_followerSet.m_maskSizeInBytes*8;

    generateIrreduciblePolynomial();

};

void PolynomialFollower::buildPolynomials(){

    for (uint32_t i = 0; i < m_followerSet.m_numOfHashFunctions; i++) {
        vec_GF2E inputs;
        vec_GF2E masks;
        for (uint32_t j=0; j < m_followerSet.m_numOfElements; j++) {
            uint32_t index = m_followerSet.m_elements_to_hash_table.get()[j*m_followerSet.m_numOfHashFunctions+i];

            NTL::GF2E input = PolynomialUtils::convertBytesToGF2E(m_followerSet.m_realElements.get()+j*m_followerSet.m_elementSizeInBytes, m_followerSet.m_elementSizeInBytes);
            inputs.append(input);

            NTL::GF2E mask = PolynomialUtils::convertBytesToGF2E(m_followerSet.m_masks.get()+index*m_followerSet.m_maskSizeInBytes, m_followerSet.m_maskSizeInBytes);
            masks.append(mask);
        }
        GF2EX polynomial = interpolate(inputs, masks);

        //std::cout << "Polynomial " << i << " is " << polynomial << std::endl;
        m_polynomials.push_back(polynomial);
    }
}

void PolynomialFollower::generateIrreduciblePolynomial() {

    m_irreduciblePolynomial = BuildSparseIrred_GF2X(m_securityParameter);
    GF2E::init(m_irreduciblePolynomial);
}

vector<vector<uint8_t>> PolynomialFollower::getPolynomialCoffBytes(NTL::GF2EX & polynomial) {
    vector<vector<uint8_t>> polynomBytes;

    //convert each coefficient polynomial to byte array and put it in the output array.
    for (int i = 0; i < m_followerSet.m_numOfElements; i++) {
        //get the coefficient polynomial
        NTL::GF2E coefficient = coeff(polynomial, i);

        //get the bytes of the coefficient.
        polynomBytes.push_back(PolynomialUtils::convertElementToBytes(coefficient));
    }

    return polynomBytes;
}

void PolynomialFollower::run() {
    xor_masks(m_followerSet.m_hashTable.get(), m_followerSet.m_elements.get(), m_followerSet.m_numOfElements, m_followerSet.m_masks.get(), m_followerSet.m_elementSizeInBytes,
              m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    buildPolynomials();

    //send the masks to the receiver
    for (auto &polynomial : m_polynomials) {
        vector<vector<uint8_t>> polynomialCoffBytes = getPolynomialCoffBytes(polynomial);
        for (auto &polynomialCoff : polynomialCoffBytes) {
            send_masks(polynomialCoff.data(), 1,
                       m_followerSet.m_maskSizeInBytes, m_leader);
        }
    }
}