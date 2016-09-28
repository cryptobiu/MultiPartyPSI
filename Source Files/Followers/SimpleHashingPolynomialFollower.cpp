//
// Created by root on 7/26/16.
//

#include <ot-psi.h>
#include <Util.h>
#include "Followers/SimpleHashingPolynomialFollower.h"
#include "PolynomialUtils.h"

void *SimpleHashingPolynomialFollower::buildPolynomialsInThread(void *poly_struct) {
    hash_polynomial_struct *pol_struct = reinterpret_cast<hash_polynomial_struct*>(poly_struct);

    GF2E::init(*(pol_struct->irreduciblePolynomial));

    for (uint32_t k=pol_struct->startBin; k < pol_struct->endBin; k++) {
        uint32_t numOfElementsInBin = pol_struct->followerSet->m_numOfElementsInBin.get()[k];

        vec_GF2E inputs;
        vec_GF2E masks;

        uint32_t numOfElementsOfHashInBin = 0;
        for (uint32_t j=pol_struct->elementIndex; j < pol_struct->elementIndex+numOfElementsInBin; j++) {
            if ((pol_struct->followerSet->m_binToElementsToHashTable.get()[j] % pol_struct->followerSet->m_numOfHashFunctions) != pol_struct->hashIndex) {
                continue;
            }
            numOfElementsOfHashInBin++;

            uint32_t elementIndex = pol_struct->followerSet->m_binToElementsToHashTable.get()[j] / pol_struct->followerSet->m_numOfHashFunctions;
            NTL::GF2E input = PolynomialUtils::convertBytesToGF2E(
                    pol_struct->followerSet->m_realElements.get()+elementIndex*pol_struct->followerSet->m_elementSizeInBytes,
                    pol_struct->followerSet->m_elementSizeInBytes);
            inputs.append(input);

            NTL::GF2E mask = PolynomialUtils::convertBytesToGF2E(
                    pol_struct->followerSet->m_masks.get()+j*pol_struct->followerSet->m_maskSizeInBytes,
                    pol_struct->followerSet->m_maskSizeInBytes);
            masks.append(mask);
        }

        if (numOfElementsOfHashInBin > pol_struct->followerSet->m_maxBinSize) {
            std::cout << "ERROR MAX SIZE IN BIN IS NOT BIG ENOUGH !!!!" << std::endl;
        }

        uint32_t numOfRandomElements = pol_struct->followerSet->m_maxBinSize-numOfElementsOfHashInBin;
        for (uint32_t j=0; j < numOfRandomElements; j++) {
            inputs.append(random_GF2E());
            masks.append(random_GF2E());
        }

        GF2EX polynomial = interpolate(inputs, masks);
        pol_struct->polynomials->push_back(polynomial);

        pol_struct->elementIndex = pol_struct->elementIndex + numOfElementsInBin;
    }

}

void SimpleHashingPolynomialFollower::buildPolynomials(){

    uint32_t numThreads = min(m_numCores,m_followerSet.m_numOfBins);

    for (uint32_t i = 0; i < m_followerSet.m_numOfHashFunctions; i++) {
        uint32_t elementIndex = 0;

        vector<pthread_t> check_threads;
        vector<boost::shared_ptr<hash_polynomial_struct>> polyStructs;

        for (uint32_t k=0; k < numThreads; k++) {
            pthread_t check_thread;
            boost::shared_ptr<hash_polynomial_struct> poly_struct(new hash_polynomial_struct);

            poly_struct->polynomials = new vector<GF2EX>();
            poly_struct->hashIndex = i;
            poly_struct->followerSet = &m_followerSet;
            poly_struct->irreduciblePolynomial = &m_irreduciblePolynomial;
            poly_struct->startBin = k*m_followerSet.m_numOfBins/numThreads;
            poly_struct->endBin = (k+1)*m_followerSet.m_numOfBins/numThreads;
            if (k==(numThreads-1)) {
                poly_struct->endBin = m_followerSet.m_numOfBins;
            }
            poly_struct->elementIndex = elementIndex;

            if(pthread_create(&check_thread, NULL, &SimpleHashingPolynomialFollower::buildPolynomialsInThread, (void*)poly_struct.get())) {
                cerr << "Error in creating new pthread at check element!" << endl;
                exit(0);
            }

            check_threads.push_back(check_thread);

            polyStructs.push_back(poly_struct);

            for (uint32_t j = poly_struct->startBin; j < poly_struct->endBin; j++) {
                elementIndex = elementIndex + m_followerSet.m_numOfElementsInBin.get()[j];
            }
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
}

void SimpleHashingPolynomialFollower::sendPolynomials() {
    uint8_t *masks;
    //posix_memalign((void**)&masks, 16, m_followerSet.m_maskSizeInBytes*m_followerSet.m_numOfHashFunctions*m_followerSet.m_numOfBins*m_followerSet.m_maxBinSize);

    masks= (uint8_t* )calloc(m_followerSet.m_maskSizeInBytes*m_followerSet.m_numOfHashFunctions*m_followerSet.m_numOfBins*m_followerSet.m_maxBinSize,sizeof(uint8_t));

    //send the masks to the receiver
    for (uint32_t i = 0; i < m_followerSet.m_numOfHashFunctions*m_followerSet.m_numOfBins; i++) {
        getPolynomialCoffBytes(m_polynomials[i], m_followerSet.m_maxBinSize, masks+m_followerSet.m_maskSizeInBytes*i*m_followerSet.m_maxBinSize);
    }
    m_polynomials.clear();
    /*
    for (uint32_t i = 0; i < m_followerSet.m_numOfHashFunctions; i++) {
        send_masks(masks+i*m_followerSet.m_numOfBins*m_followerSet.m_maxBinSize, m_followerSet.m_numOfBins*m_followerSet.m_maxBinSize,
                   m_followerSet.m_maskSizeInBytes, m_leader);
    }
    */
    send_masks(masks, m_followerSet.m_numOfHashFunctions*m_followerSet.m_numOfBins*m_followerSet.m_maxBinSize,
               m_followerSet.m_maskSizeInBytes, m_leader);


    free(masks);
}