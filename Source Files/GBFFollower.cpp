//
// Created by root on 7/22/16.
//

#include "../Header Files/GBFFollower.h"
#include "../PSI_C_0.1/PSI/headers/GarbledBF.h"
#include "../PSI/src/ot-based/ot-psi.h"

GBFFollower::GBFFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader) :
        Follower(followerSet, secretShare, leader), GarbledBloomFilter(m_followerSet.m_maskSizeInBytes, m_followerSet.m_numOfElements) {

    m_filter = GBF_Create();
    generateHashKeys();

    for(int i=0;i<m_bfParam->k;i++){
        RangeHash* hashFunc;
        RangeHash_Create(&hashFunc, m_keys[i].get(), m_securityParameter/8, m_bfParam->m);
        m_hashFuncs.push_back(boost::shared_ptr<RangeHash>(hashFunc));
    }
};

void GBFFollower::buildGBF(){

    AESRandom* rnd;

    AESRandom_Create(&rnd, m_securityParameter/8);
    for(int i=0;i<m_followerSet.m_numOfElements * m_followerSet.m_numOfHashFunctions;i++){
        GBF_add(m_filter.get(), m_hashFuncs, m_bfParam->k,
                m_followerSet.m_elements.get()+i*m_followerSet.m_elementSizeInBytes,
                m_followerSet.m_elementSizeInBytes,
                m_followerSet.m_masks.get()+i*m_followerSet.m_maskSizeInBytes, rnd);

    }
    //GBF_doFinal(sver->filter, rnd);
    AESRandom_Destroy(rnd);
}



void GBFFollower::generateHashKeys() {
    AESRandom* rnd;
    AESRandom_Create(&rnd, 16);
    int32_t keyLen=m_securityParameter/8;

    for(int i=0;i<m_bfParam->k;i++){
        boost::shared_ptr<uint8_t> e(new uint8_t[keyLen]);
        AESRandom_NextBytes(rnd, e.get(), keyLen);
        m_keys.push_back(e);
    }
    AESRandom_Destroy(rnd);
}

void GBFFollower::run() {
    xor_masks(m_followerSet.m_hashTable.get(), m_followerSet.m_elements.get(), m_followerSet.m_numOfElements, m_followerSet.m_masks.get(), m_followerSet.m_elementSizeInBytes,
              m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    //sendArrayOfByteStrings(sver->socket, sver->bfParam->k, sver->secLev/8, sver->keys);
    buildGBF();

    //send the masks to the receiver
    send_masks(m_filter->data, m_bfParam->m,
               m_followerSet.m_maskSizeInBytes, m_leader);
}