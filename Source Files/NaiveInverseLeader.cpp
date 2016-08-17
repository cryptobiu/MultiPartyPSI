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
        m_matPerHash[i] = to_mat_GF2(fullVector);
    }
}

vec_GF2 NaiveInverseLeader::vec_GF2FromBytes(uint8_t *arr, uint32_t size) {
    NTL::GF2X e;
    NTL::GF2XFromBytes(e, arr, size);
    return VectorCopy(e, size * sizeof(uint8_t));
}

bool NaiveInverseLeader::isZeroXOR(uint8_t *formerShare, uint32_t partyNum, uint32_t hashIndex) {

    vec_GF2 b = vec_GF2FromBytes(formerShare, m_maskSizeInBytes);

    GF2 det;
    vec_GF2 res;
    solve(det, res, m_matPerHash[hashIndex], b);

    return (det != 0);
};