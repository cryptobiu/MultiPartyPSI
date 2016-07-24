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
protected:
    GarbledBloomFilter(uint32_t maskSizeInBytes, uint32_t setSize);

    boost::shared_ptr<GarbledBF> GBF_Create();
    void BF_GenerateParameters();

    uint32_t m_setSizeE;
    uint32_t m_maskSizeInBytesE;
    uint32_t m_securityParameter;
    boost::shared_ptr<BFParameters> m_bfParam;
};


#endif //MULTIPARTYPSI_GARBLEDBLOOMFILTER_H
