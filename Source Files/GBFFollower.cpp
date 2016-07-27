//
// Created by root on 7/22/16.
//

#include <GarbledBF.h>
#include "GBFFollower.h"
#include "ot-psi.h"

GBFFollower::GBFFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader, const secParameters &parameters) :
        Follower(followerSet, secretShare, leader, parameters),
        GarbledBloomFilter(m_followerSet.m_maskSizeInBytes, m_parameters.m_statSecParameter, m_followerSet.m_numOfElements) {

    generateHashKeys();

    for(int i=0;i<m_bfParam->k;i++){
        boost::shared_ptr<RangeHash> hashFunc(new RangeHash());
        RangeHash_Create(hashFunc.get(), m_keys[i].get(), m_parameters.m_symSecParameter/8, m_bfParam->m);
        m_hashFuncs.push_back(boost::shared_ptr<RangeHash>(hashFunc));
    }
};

void GBFFollower::buildGBF(){

    AESRandom* rnd;

    AESRandom_Create(&rnd, m_parameters.m_symSecParameter/8);

    for (uint32_t i = 0; i < m_followerSet.m_numOfHashFunctions; i++) {
        auto filter = GBF_Create();
        for (uint32_t j=0; j < m_followerSet.m_numOfElements; j++) {
            uint32_t index = m_followerSet.m_elements_to_hash_table.get()[j*m_followerSet.m_numOfHashFunctions+i];

            GBF_add(filter.get(), m_hashFuncs, m_bfParam->k,
                    m_followerSet.m_realElements.get()+j*m_followerSet.m_elementSizeInBytes,
                    m_followerSet.m_elementSizeInBytes,
                    m_followerSet.m_masks.get()+index*m_followerSet.m_maskSizeInBytes, rnd);
        }
        m_filters.push_back(filter);
    }

    AESRandom_Destroy(rnd);

}

void GBFFollower::generateHashKeys() {
    AESRandom* rnd;

    int32_t keyLen=m_parameters.m_symSecParameter/8;

    AESRandom_Create(&rnd, keyLen);

    for(int i=0;i<m_bfParam->k;i++){
        boost::shared_ptr<uint8_t> e(new uint8_t[keyLen]);
        AESRandom_NextBytes(rnd, e.get(), keyLen);
        m_keys.push_back(e);
    }
    AESRandom_Destroy(rnd);
}

void GBFFollower::run() {
    xor_masks(m_followerSet.m_masks.get(), m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());


    for (auto &key : m_keys) {
        m_leader.Send(key.get(),m_parameters.m_symSecParameter/8);
    }

    buildGBF();

    //send the masks to the receiver
    for (auto &filter : m_filters) {
        send_masks(filter->data, m_bfParam->m,
                   m_followerSet.m_maskSizeInBytes, m_leader);
    }
}