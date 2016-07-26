//
// Created by root on 7/26/16.
//

#include <ot-psi.h>
#include "SimpleHashingNaiveLeader.h"

void SimpleHashingNaiveLeader::receiveServerData() {
    vector<pthread_t> rcv_masks_threads;

    boost::shared_ptr<mask_rcv_ctx> rcv_ctxs(new mask_rcv_ctx[m_parties.size()+1]);
    for (auto &party : m_parties) {
        //receive server masks
        m_partiesResults[party.first] = boost::shared_ptr<uint8_t>(new uint8_t[NUM_HASH_FUNCTIONS * m_numOfBins * m_maxBinSize]);

        //receive_masks(server_masks, NUM_HASH_FUNCTIONS * neles, maskbytelen, sock[0]);
        //use a separate thread to receive the server's masks
        (rcv_ctxs.get())[party.first - 1].rcv_buf = m_partiesResults[party.first].get();
        (rcv_ctxs.get())[party.first - 1].nmasks = NUM_HASH_FUNCTIONS * m_numOfBins * m_maxBinSize;
        (rcv_ctxs.get())[party.first - 1].maskbytelen = m_maskSizeInBytes;
        (rcv_ctxs.get())[party.first - 1].sock = party.second.get();
    }

    receiveServerDataInThreads<mask_rcv_ctx>(rcv_ctxs, &NaiveLeader::receiveMasks);
}

bool SimpleHashingNaiveLeader::isZeroXOR(uint8_t *formerShare, uint32_t partyNum, uint32_t binIndex) {
    if (partyNum <= m_parties.size()+1) {
        uint8_t *partyResult = m_partiesResults[partyNum].get();
        uint32_t minIndex = binIndex*m_maxBinSize;
        uint32_t maxIndex = (binIndex+1)*m_maxBinSize;
        for (uint32_t i = minIndex; i < maxIndex; i++) {
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