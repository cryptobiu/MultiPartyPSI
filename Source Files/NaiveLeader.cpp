//
// Created by root on 7/21/16.
//

#include "NaiveLeader.h"
#include "defs.h"
#include "ot-psi.h"

void *NaiveLeader::receiveMasks(void *ctx_tmp) {
    mask_rcv_ctx* ctx = (mask_rcv_ctx*) ctx_tmp;
    ctx->sock->Receive(ctx->rcv_buf, ctx->maskbytelen * ctx->nmasks);
}

void NaiveLeader::receiveServerData() {
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
}

bool NaiveLeader::isElementInAllSets(uint32_t index, uint32_t binIndex, uint32_t tableIndex, uint32_t hashFuncIndex, uint8_t *secret) {

    for (auto &party : m_parties) {
        XOR(secret, m_leaderResults[party.first].get()+tableIndex*m_maskSizeInBytes, m_maskSizeInBytes);
    }

    // 1 is always the leader Id
    return isZeroXOR(secret,2,binIndex);
}

bool NaiveLeader::isZeroXOR(uint8_t *formerShare, uint32_t partyNum, uint32_t binIndex) {
    if (partyNum <= m_parties.size()+1) {
        uint8_t *partyResult = m_partiesResults[partyNum].get();
        for (uint32_t i = 0; i < m_setSize *m_numOfHashFunctions; i++) {
            XOR(formerShare,partyResult+i*m_maskSizeInBytes, m_maskSizeInBytes);
            if (isZeroXOR(formerShare,partyNum+1,binIndex)) {
                return true;
            }
            XOR(formerShare,partyResult+i*m_maskSizeInBytes, m_maskSizeInBytes);
        }
        return false;
    }

    return isZero(formerShare, m_maskSizeInBytes);
};