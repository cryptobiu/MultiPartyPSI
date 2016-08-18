//
// Created by naor on 8/18/16.
//

#ifndef MULTIPARTYPSI_GF2MATRIXUTILS_H
#define MULTIPARTYPSI_GF2MATRIXUTILS_H

#include <NTL/mat_GF2.h>

NTL_CLIENT

class GF2MatrixUtils {
public:
    static vec_GF2 vec_GF2FromBytes(uint8_t *arr, uint32_t size);
    static vec_GF2 solve(mat_GF2 A, vec_GF2 b);
};


#endif //MULTIPARTYPSI_GF2MATRIXUTILS_H
