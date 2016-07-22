//
// Created by root on 7/22/16.
//

#include "../Header Files/GBFFollower.h"
#include "../PSI_C_0.1/PSI/headers/GarbledBF.h"

GBFFollower::GBFFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader) :
        Follower(followerSet, secretShare, leader), m_bfParam(new BFParameters()), m_filter(new GarbledBF()) {

    m_securityParameter = m_followerSet.m_maskSizeInBytes * 8;

    BF_GenerateParameters(m_followerSet.m_numOfElements * m_followerSet.m_numOfHashFunctions, m_securityParameter);
    GBF_Create(m_bfParam->m, m_bfParam->k);

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

void GBFFollower::BF_GenerateParameters(int32_t n, int32_t p) {
    m_bfParam->n=n;
    m_bfParam->k=p;
    m_bfParam->m=ceil(n*p*log2e);
    if (m_bfParam->m%8!=0)
        m_bfParam->m=ceil(m_bfParam->m/8.0)*8;
}

int GBFFollower::GBF_Create(int32_t m, int32_t k){
    m_filter->m=m;
    m_filter->k=k;
    m_filter->indexes=(int32_t*)calloc(k, sizeof(int32_t));
    //gbf->MT=0;
    //gbf->bitmap=NULL;

    if(!(m_filter->data= (uint8_t** )calloc(m,sizeof(uint8_t*)))){
        return 0;
    }
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
    //sendArrayOfByteStrings(sver->socket, sver->bfParam->k, sver->secLev/8, sver->keys);
    buildGBF();
}