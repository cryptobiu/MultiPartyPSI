//
// Created by root on 7/25/16.
//

#include "PolynomialFollower.h"
#include "PolynomialUtils.h"
#include "ot-psi.h"

PolynomialFollower::PolynomialFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader, const secParameters &parameters) :
        Follower(followerSet, secretShare, leader, parameters) {

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

        m_polynomials.push_back(polynomial);
    }
}

void PolynomialFollower::generateIrreduciblePolynomial() {

    m_irreduciblePolynomial = BuildSparseIrred_GF2X(m_parameters.m_statSecParameter);
    GF2E::init(m_irreduciblePolynomial);

    // std::cout << "Follower irreducible Polynomial is " << m_irreduciblePolynomial << std::endl;
}

vector<vector<uint8_t>> PolynomialFollower::getPolynomialCoffBytes(NTL::GF2EX & polynomial) {
    vector<vector<uint8_t>> polynomBytes;

    uint32_t polyDeg = deg(polynomial);

    //convert each coefficient polynomial to byte array and put it in the output array.
    for (int i = 0; i <= polyDeg; i++) {
        //get the coefficient polynomial
        NTL::GF2E coefficient = coeff(polynomial, i);

        //get the bytes of the coefficient.
        vector<uint8_t> bytes = PolynomialUtils::convertElementToBytes(coefficient);
        uint32_t numBytes = bytes.size();
        if (numBytes < m_followerSet.m_maskSizeInBytes) {
            for (uint32_t j = 0; j < (m_followerSet.m_maskSizeInBytes-numBytes);j++) {
                bytes.push_back(0);
            }
        }
        polynomBytes.push_back(bytes);
    }

    return polynomBytes;
}

void PolynomialFollower::run() {
    xor_masks(m_followerSet.m_masks.get(), m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    buildPolynomials();

    //vector<uint8_t> irreduciblePolynomialBytes = PolynomialUtils::convertGF2XToBytes(m_irreduciblePolynomial);
    //send_masks(irreduciblePolynomialBytes.data(),1,m_followerSet.m_maskSizeInBytes,m_leader);

    //send the masks to the receiver
    for (auto &polynomial : m_polynomials) {
        vector<vector<uint8_t>> polynomialCoffBytes = getPolynomialCoffBytes(polynomial);
        for (auto &polynomialCoff : polynomialCoffBytes) {
            if (polynomialCoff.size() != m_followerSet.m_maskSizeInBytes) {
                std::cout << "coefficient size is not correct !!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                std::cout << polynomialCoff.size() << std::endl;
            }
            send_masks(polynomialCoff.data(), 1,
                       polynomialCoff.size(), m_leader);
        }
    }
}