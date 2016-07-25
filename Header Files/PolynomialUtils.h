//
// Created by root on 7/25/16.
//

#ifndef MULTIPARTYPSI_POLYNOMIALUTILS_H
#define MULTIPARTYPSI_POLYNOMIALUTILS_H

#include <NTL/GF2E.h>
#include <vector>

NTL_CLIENT

class PolynomialUtils {
public:
    static NTL::GF2E convertBytesToGF2E(uint8_t *elementByts, uint32_t size);
    static vector<uint8_t> convertElementToBytes(NTL::GF2E & element);
};

#endif //MULTIPARTYPSI_POLYNOMIALUTILS_H
