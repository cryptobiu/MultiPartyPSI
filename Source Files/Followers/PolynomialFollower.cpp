//
// Created by root on 7/25/16.
//

#include "Followers/PolynomialFollower.h"
#include "PolynomialUtils.h"
#include "ot-psi.h"

PolynomialFollower::PolynomialFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader,
                                       const secParameters &parameters, uint32_t numCores) :
        Follower(followerSet, secretShare, leader, parameters, numCores) {

    generateIrreduciblePolynomial();

};

void *PolynomialFollower::buildPolynomialsInThread(void *poly_struct) {
    polynomial_struct *pol_struct = reinterpret_cast<polynomial_struct*>(poly_struct);

    GF2E::init(*(pol_struct->irreduciblePolynomial));

    vec_GF2E inputs;
    vec_GF2E masks;
    PRINT() << "Get Points" << std::endl;
    for (uint32_t j=0; j < pol_struct->followerSet->m_numOfElements; j++) {
        uint32_t index = pol_struct->followerSet->m_elements_to_hash_table.get()[
                j*pol_struct->followerSet->m_numOfHashFunctions+pol_struct->hashIndex];

        NTL::GF2E input = PolynomialUtils::convertBytesToGF2E(
                pol_struct->followerSet->m_realElements.get()+j*pol_struct->followerSet->m_elementSizeInBytes,
                                                              pol_struct->followerSet->m_elementSizeInBytes);
        inputs.append(input);

        NTL::GF2E mask = PolynomialUtils::convertBytesToGF2E(
                pol_struct->followerSet->m_masks.get()+index*pol_struct->followerSet->m_maskSizeInBytes,
                pol_struct->followerSet->m_maskSizeInBytes);
        masks.append(mask);
    }

    PRINT() << "interpolate" << std::endl;

    GF2EX polynomial = interpolate(inputs, masks); // this is the costly operation

    pol_struct->polynomials->push_back(polynomial);
}

void PolynomialFollower::buildPolynomials(){

    vector<pthread_t> check_threads;
    vector<boost::shared_ptr<polynomial_struct>> polyStructs;

    for (uint32_t i = 0; i < m_followerSet.m_numOfHashFunctions; i++) {
        pthread_t check_thread;
        boost::shared_ptr<polynomial_struct> poly_struct(new polynomial_struct);
        poly_struct->polynomials = new vector<GF2EX>();
        poly_struct->hashIndex = i;
        poly_struct->followerSet = &m_followerSet;
        poly_struct->irreduciblePolynomial = &m_irreduciblePolynomial;

        if(pthread_create(&check_thread, NULL, &PolynomialFollower::buildPolynomialsInThread, (void*)poly_struct.get())) {
            cerr << "Error in creating new pthread at check element!" << endl;
            exit(0);
        }

        check_threads.push_back(check_thread);

        polyStructs.push_back(poly_struct);
    }

    for (auto &check_thread : check_threads) {
        //meanwhile generate the hash table
        //GHashTable* map = otpsi_create_hash_table(ceil_divide(inbitlen,8), masks, neles, maskbytelen, perm);
        //intersect_size = otpsi_find_intersection(eleptr, result, ceil_divide(inbitlen,8), masks, neles, server_masks,
        //		neles * NUM_HASH_FUNCTIONS, maskbytelen, perm);
        //wait for receiving thread
        if(pthread_join(check_thread, NULL)) {
            cerr << "Error in joining pthread at check element!" << endl;
            exit(0);
        }
    }

    for (auto &polyStruct : polyStructs) {
        m_polynomials.insert(m_polynomials.end(), polyStruct->polynomials->begin(), polyStruct->polynomials->end());
        delete polyStruct->polynomials;
    }
}

void PolynomialFollower::generateIrreduciblePolynomial() {

    m_irreduciblePolynomial = BuildSparseIrred_GF2X(m_parameters.m_statSecParameter);
    GF2E::init(m_irreduciblePolynomial);

    // std::cout << "Follower irreducible Polynomial is " << m_irreduciblePolynomial << std::endl;
}

void PolynomialFollower::getPolynomialCoffBytes(NTL::GF2EX & polynomial, uint32_t polyDeg, uint8_t *arr) {
    vector<vector<uint8_t>> polynomBytes;

    //convert each coefficient polynomial to byte array and put it in the output array.
    for (int i = 0; i < polyDeg; i++) {
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
        memcpy(arr+i*m_followerSet.m_maskSizeInBytes,bytes.data(),m_followerSet.m_maskSizeInBytes);
    }
}

void PolynomialFollower::sendPolynomials() {

    uint8_t *masks;
    posix_memalign((void**)&masks, 16, m_followerSet.m_maskSizeInBytes*m_followerSet.m_numOfHashFunctions*m_followerSet.m_numOfElements);

    //send the masks to the receiver
    for (uint32_t i = 0; i < m_followerSet.m_numOfHashFunctions; i++) {
        getPolynomialCoffBytes(m_polynomials[i], m_followerSet.m_numOfElements, masks+m_followerSet.m_maskSizeInBytes*i*m_followerSet.m_numOfElements);
    }

    send_masks(masks, m_followerSet.m_numOfHashFunctions*m_followerSet.m_numOfElements,
               m_followerSet.m_maskSizeInBytes, m_leader);

    free(masks);

}

void PolynomialFollower::run() {
    xor_masks(m_followerSet.m_masks.get(), m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    PRINT() << "buildPolynomials" << std::endl;
    buildPolynomials();

    //vector<uint8_t> irreduciblePolynomialBytes = PolynomialUtils::convertGF2XToBytes(m_irreduciblePolynomial);
    //send_masks(irreduciblePolynomialBytes.data(),1,m_followerSet.m_maskSizeInBytes,m_leader);

    PRINT() << "sendPolynomials" << std::endl;
    sendPolynomials();
}