//
// Created by root on 7/24/16.
//

#include "GarbledBloomFilter.h"
#include <iostream>

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

    if(!(filter->data= (uint8_t* )calloc(m_bfParam->m*filter->m_GBFSigmaByteLen,sizeof(uint8_t*)))){
        return 0;
    }

    if(!(filter->bf= (uint8_t* )calloc(m_bfParam->m,sizeof(uint8_t*)))){
        return 0;
    }

    return filter;
}