//
// Created by naor on 8/17/16.
//

#include <NTL/GF2X.h>
#include "NaiveInverseLeader.h"
#include "defs.h"
#include "ot-psi.h"

void NaiveInverseLeader::receiveServerData() {
    boost::shared_ptr<mask_rcv_ctx> rcv_ctxs(new mask_rcv_ctx[m_parties.size()+1]);
    for (auto &party : m_parties) {
        //receive server masks
        m_partiesResults[party.first] = boost::shared_ptr<uint8_t>(new uint8_t[NUM_HASH_FUNCTIONS * m_setSize * m_maskSizeInBytes]);

        //receive_masks(server_masks, NUM_HASH_FUNCTIONS * neles, maskbytelen, sock[0]);
        //use a separate thread to receive the server's masks
        (rcv_ctxs.get())[party.first - 1].rcv_buf = m_partiesResults[party.first].get();
        (rcv_ctxs.get())[party.first - 1].nmasks = NUM_HASH_FUNCTIONS * m_setSize;
        (rcv_ctxs.get())[party.first - 1].maskbytelen = m_maskSizeInBytes;
        (rcv_ctxs.get())[party.first - 1].sock = party.second.get();
    }

    receiveServerDataInThreads<mask_rcv_ctx>(rcv_ctxs, &NaiveLeader::receiveMasks);

    for (uint32_t i=0; i < m_numOfHashFunctions; i++) {
        vec_vec_GF2 fullVector;
        for (auto &result : m_partiesResults) {
            uint8_t *ptr = result.second.get()+i*m_setSize*m_maskSizeInBytes;
            for (uint32_t j=0; j < m_setSize; j++) {
                fullVector.append(vec_GF2FromBytes(ptr+j*m_maskSizeInBytes, m_maskSizeInBytes));
            }
        }
        m_matPerHash[i] = transpose(to_mat_GF2(fullVector));
    }
}

vec_GF2 NaiveInverseLeader::solve(mat_GF2 A, vec_GF2 b) {

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
        if (A[lastRow][j]==0) {
            continue;
        }
        if (lastRow == -1) {
            throw(system_error());
        }

        if ((A[lastRow]*sol) != b[lastRow]) {
            sol[j]=1;
        }
        lastRow--;
    }

    std::cout << "solution to matrix is " << sol << std::endl;
    return sol;
}

vec_GF2 NaiveInverseLeader::vec_GF2FromBytes(uint8_t *arr, uint32_t sizeInBytes) {
    NTL::GF2X e;
    NTL::GF2XFromBytes(e, arr, sizeInBytes);
    return VectorCopy(e, sizeInBytes*8);
    //return VectorCopy(e, m_setSize*m_partiesResults.size());
}

bool NaiveInverseLeader::isZeroXOR(uint8_t *formerShare, uint32_t partyNum, uint32_t hashIndex) {

    vec_GF2 b = vec_GF2FromBytes(formerShare, m_maskSizeInBytes);

    try {
        vec_GF2 res = solve(m_matPerHash[hashIndex], b);

        for (uint32_t i =0; i < m_parties.size()*m_setSize; i+=m_setSize) {
            uint32_t numOnes = 0;
            for (uint32_t j = 0; j < m_setSize; j++) {
                if (res[i+j]==1) {
                    numOnes++;
                }
            }
            if (numOnes != 1) {
                return false;
            }
        }
    }
    catch(system_error) {
        return false;
    }
    return true;
}