//
// Created by naor on 9/15/16.
//

#include <ot-psi.h>
#include "Leaders/TwoPartyLeader.h"

void *TwoPartyLeader::receiveMasks(void *ctx_tmp) {
    mask_rcv_ctx* ctx = (mask_rcv_ctx*) ctx_tmp;
    ctx->sock->Receive(ctx->rcv_buf, ctx->maskbytelen * ctx->nmasks);
}

void TwoPartyLeader::receiveServerData() {
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

    receiveServerDataInThreads<mask_rcv_ctx>(rcv_ctxs, &TwoPartyLeader::receiveMasks);
}

uint32_t TwoPartyLeader::run() {
    receiveServerData();

    //compute intersection
    uint32_t *res_pos;
    return otpsi_find_intersection(&res_pos, m_leaderResults[2].get(), m_setSize, m_partiesResults[2].get(),
                                   m_setSize * NUM_HASH_FUNCTIONS, m_maskSizeInBytes, m_hashInfo.get());
}