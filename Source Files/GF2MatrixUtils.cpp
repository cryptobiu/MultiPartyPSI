//
// Created by naor on 8/18/16.
//

#include <NTL/GF2X.h>
#include "GF2MatrixUtils.h"
#include <system_error>

vec_GF2 GF2MatrixUtils::solve(mat_GF2 A, vec_GF2 b) {

    uint32_t nextRow = 0;
    for (uint32_t j =0; j < A.NumCols(); j++) {
        uint32_t i;
        for (i=nextRow; i < A.NumRows(); i++) {
            if (A[i][j] == 1) {
                break;
            }
        }
        if (i==A.NumRows()) {
            continue;
        }
        if (A[i][j] == 1) {
            if (nextRow !=i) {
                swap(A[nextRow],A[i]);
                swap(b[nextRow],b[i]);
            }
            for (uint32_t k=nextRow+1; k < A.NumRows(); k++) {
                if (A[k][j] == 1) {
                    A[k] = A[k] + A[nextRow];
                    b[k] = b[k] + b[nextRow];
                }
            }
            nextRow++;
        }
    }

    vec_GF2 sol;
    sol.SetLength(A.NumCols(), GF2(0));

    int32_t lastRow=A.NumRows()-1;

    while (A[lastRow][A.NumCols()-1]==0) {
        if ((A[lastRow]*sol) != b[lastRow]) {
            throw(system_error());
        }
        lastRow--;
        if (lastRow == -1) {
            throw(system_error());
        }
    }

    for (int32_t j=A.NumCols()-1; j >= 0; j--) {
        if (lastRow == -1) {
            throw(system_error());
        }

        if (A[lastRow][j]==0) {
            continue;
        }

        if ((A[lastRow]*sol) != b[lastRow]) {
            sol[j]=1;
        }
        lastRow--;
    }

    return sol;
}

vec_GF2 GF2MatrixUtils::vec_GF2FromBytes(uint8_t *arr, uint32_t sizeInBytes) {
    NTL::GF2X e;
    NTL::GF2XFromBytes(e, arr, sizeInBytes);
    return VectorCopy(e, sizeInBytes*8);
    //return VectorCopy(e, m_setSize*m_partiesResults.size());
}