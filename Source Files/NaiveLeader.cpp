//
// Created by root on 7/21/16.
//

#include "../Header Files/NaiveLeader.h"
#include "../Header Files/defs.h"
#include "../PSI/src/ot-based/ot-psi.h"

vector<uint32_t> NaiveLeader::run() {

    receiveServerMasks();

    vector<uint32_t> intersection;
    for (uint32_t i = 0; i < m_setSize; i++) {
        if (isElementInAllSets(i)) {
            // std::cout << "Input " << *(uint32_t*)(&m_elements[i]) << " is in the intersection" << std::endl;
            intersection.push_back(i);
            //intersection.push_back(*(uint32_t*)(&m_elements[i]));
        }
    }

    return intersection;
}

void NaiveLeader::receiveServerMasks() {
    vector<pthread_t> rcv_masks_threads;

    boost::shared_ptr<mask_rcv_ctx> rcv_ctxs(new mask_rcv_ctx[m_parties.size()+1]);
    for (auto &party : m_parties) {
        pthread_t rcv_masks_thread;

        //receive server masks
        m_partiesResults[party.first] = (uint8_t*) malloc(NUM_HASH_FUNCTIONS * m_setSize * m_maskSizeInBytes);

        //receive_masks(server_masks, NUM_HASH_FUNCTIONS * neles, maskbytelen, sock[0]);
        //use a separate thread to receive the server's masks
        (rcv_ctxs.get())[party.first - 1].rcv_buf = m_partiesResults[party.first];
        (rcv_ctxs.get())[party.first - 1].nmasks = NUM_HASH_FUNCTIONS * m_setSize;
        (rcv_ctxs.get())[party.first - 1].maskbytelen = m_maskSizeInBytes;
        (rcv_ctxs.get())[party.first - 1].sock = party.second;
        if(pthread_create(&rcv_masks_thread, NULL, receive_masks, (void*) (&(rcv_ctxs.get())[party.first - 1]))) {
            cerr << "Error in creating new pthread at cuckoo hashing!" << endl;
            exit(0);
        }

        rcv_masks_threads.push_back(rcv_masks_thread);
    }

    for (auto rcv_masks_thread : rcv_masks_threads) {
        //meanwhile generate the hash table
        //GHashTable* map = otpsi_create_hash_table(ceil_divide(inbitlen,8), masks, neles, maskbytelen, perm);
        //intersect_size = otpsi_find_intersection(eleptr, result, ceil_divide(inbitlen,8), masks, neles, server_masks,
        //		neles * NUM_HASH_FUNCTIONS, maskbytelen, perm);
        //wait for receiving thread
        if(pthread_join(rcv_masks_thread, NULL)) {
            cerr << "Error in joining pthread at cuckoo hashing!" << endl;
            exit(0);
        }
    }
}

bool NaiveLeader::isElementInAllSets(uint32_t index) {

    uint32_t binIndex = 0;
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        if ((m_binIds.get())[i] == index + 1) {
            // std::cout << "Element number " << index << " was found at " << i << std::endl;
            binIndex = i;
            break;
        }
    }

    uint32_t newIndex = 0;
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        if ((m_perm.get())[i] == index) {
            newIndex = i;
            break;
        }
    }

    uint8_t* secret = &(m_secretShare.get()[binIndex*m_maskSizeInBytes]);

    for (auto &party : m_parties) {
        XOR(secret, m_leaderResults[party.first].get()+newIndex*m_maskSizeInBytes, m_maskSizeInBytes);
    }

    // 1 is always the leader Id
    return isZeroXOR(secret,2);
}

bool NaiveLeader::isZeroXOR(uint8_t *formerShare, uint32_t partyNum) {
    if (partyNum <= m_parties.size()+1) {
        uint8_t *partyResult = m_partiesResults[partyNum];
        for (uint32_t i = 0; i < m_setSize *m_numOfHashFunctions; i++) {
            XOR(formerShare,partyResult+i*m_maskSizeInBytes, m_maskSizeInBytes);
            if (isZeroXOR(formerShare,partyNum+1)) {
                return true;
            }
            XOR(formerShare,partyResult+i*m_maskSizeInBytes, m_maskSizeInBytes);
        }
        return false;
    }

    for (uint32_t i = 0; i < m_maskSizeInBytes; i++) {
        if (formerShare[i] != 0) {
            return false;
        }
    }
    return true;
};