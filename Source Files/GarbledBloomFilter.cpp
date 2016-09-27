//
// Created by root on 7/24/16.
//

#include "GarbledBloomFilter.h"
#include <iostream>
#include <CryptoInfra/Key.hpp>
#include <primitives/Prg.hpp>
#include <typedefs.h>

#define KEY_SIZE 16

GarbledBloomFilter::GarbledBloomFilter(uint32_t maskSizeInBytes, uint32_t statSecurityParameter, uint32_t setSize) :
        m_maskSizeInBytesE(maskSizeInBytes), m_statSecurityParameter(statSecurityParameter), m_setSizeE(setSize), m_bfParam(new BFParameters()) {
    BF_GenerateParameters();
}

void GarbledBloomFilter::BF_GenerateParameters() {
    m_bfParam->n=m_setSizeE;
    m_bfParam->k=m_statSecurityParameter;
    m_bfParam->m=ceil(m_setSizeE*m_statSecurityParameter*log2e);
    if (m_bfParam->m%8!=0)
        m_bfParam->m=ceil(m_bfParam->m/8.0)*8;
}

boost::shared_ptr<GarbledBF> GarbledBloomFilter::GBF_Create(){
    boost::shared_ptr<GarbledBF> filter(new GarbledBF());
    filter->m=m_bfParam->m;
    filter->k=m_bfParam->k;
    filter->indexes=(int32_t*)calloc(m_bfParam->k, sizeof(int32_t));
    //gbf->MT=0;
    //gbf->bitmap=NULL;

    filter->m_GBFSigmaByteLen = m_maskSizeInBytesE;

    uint8_t *seed = new uint8_t[KEY_SIZE];
    RAND_bytes(seed,KEY_SIZE);
    SecretKey key(seed, KEY_SIZE, "PrgFromOpenSSLAES");
    PrgFromOpenSSLAES prg(ceil_divide(filter->m*filter->m_GBFSigmaByteLen,16));
    prg.setKey(key);

    vector<uint8_t> random;
    prg.getPRGBytes(random, 0, filter->m*filter->m_GBFSigmaByteLen);

    if(!(filter->data= (uint8_t* )calloc(m_bfParam->m*filter->m_GBFSigmaByteLen,sizeof(uint8_t)))){
        return 0;
    }

    memcpy(filter->data,random.data(),m_bfParam->m*filter->m_GBFSigmaByteLen);

    if(!(filter->bf= (uint8_t* )calloc(m_bfParam->m,sizeof(uint8_t)))){
        return 0;
    }

    return filter;
}