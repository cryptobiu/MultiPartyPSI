//
// Created by root on 7/24/16.
//

#ifndef MULTIPARTYPSI_GARBLEDBLOOMFILTER_H
#define MULTIPARTYPSI_GARBLEDBLOOMFILTER_H

#include "defs.h"
#include <boost/shared_ptr.hpp>

#include "RangeHash.h"
#include "BF.h"
#include "GarbledBF.h"
#include "GarbledBloomFilter.h"

class GarbledBloomFilter {
public:
    static GarbledBF *GBF_Create(BFParameters *bfParam, uint8_t *random);
protected:
    GarbledBloomFilter(uint32_t maskSizeInBytes, uint32_t statSecurityParameter, uint32_t setSize);

    void BF_GenerateParameters();

    uint32_t m_setSizeE;
    uint32_t m_maskSizeInBytesE;
    uint32_t m_statSecurityParameter;

    boost::shared_ptr<BFParameters> m_bfParam;
};


#endif //MULTIPARTYPSI_GARBLEDBLOOMFILTER_H
