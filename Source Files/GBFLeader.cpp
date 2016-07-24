//
// Created by root on 7/22/16.
//

#include "../Header Files/GBFLeader.h"

GBFLeader::GBFLeader(const map <uint32_t, boost::shared_ptr<uint8_t>> &leaderResults,
          const boost::shared_ptr <uint32_t> &bin_ids, const boost::shared_ptr <uint32_t> &perm, uint32_t numOfBins,
          const boost::shared_ptr <uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
          const std::map <uint32_t, boost::shared_ptr<CSocket>> &parties,
          uint32_t numOfHashFunctions) :
        Leader(leaderResults, bin_ids, perm, numOfBins, secretShare, maskSizeInBytes, setSize, parties,
               numOfHashFunctions), GarbledBloomFilter(maskSizeInBytes, setSize) {

    for (auto &party : m_parties) {
        m_partiesFilters[party.first] = std::vector<boost::shared_ptr<GarbledBF>>();
        for (uint32_t i = 0; i < m_numOfHashFunctions; i++) {
            m_partiesFilters[party.first].push_back(GBF_Create());
        }
    }

};

vector<uint32_t> GBFLeader::run() {

}

void GBFLeader::receiveGBFKeysAndFilters() {

    vector<pthread_t> rcv_filters_threads;

    boost::shared_ptr<filter_rcv_ctx> rcv_ctxs(new filter_rcv_ctx[m_parties.size()+1]);
    for (auto &party : m_parties) {
        pthread_t rcv_masks_thread;

        //receive_masks(server_masks, NUM_HASH_FUNCTIONS * neles, maskbytelen, sock[0]);
        //use a separate thread to receive the server's masks
        (rcv_ctxs.get())[party.first - 1].filters = m_partiesFilters[party.first];
        (rcv_ctxs.get())[party.first - 1].filterSizeInBytes = m_setSize;
        (rcv_ctxs.get())[party.first - 1].numOfHashFunction = m_numOfHashFunctions;
        (rcv_ctxs.get())[party.first - 1].maskbytelen = m_maskSizeInBytes;
        (rcv_ctxs.get())[party.first - 1].sock = party.second.get();
        /*
        if(pthread_create(&rcv_masks_thread, NULL, receive_masks, (void*) (&(rcv_ctxs.get())[party.first - 1]))) {
            cerr << "Error in creating new pthread at cuckoo hashing!" << endl;
            exit(0);
        }

        rcv_filters_threads.push_back(rcv_masks_thread);
         */
    }

    for (auto rcv_filters_thread : rcv_filters_threads) {
        //meanwhile generate the hash table
        //GHashTable* map = otpsi_create_hash_table(ceil_divide(inbitlen,8), masks, neles, maskbytelen, perm);
        //intersect_size = otpsi_find_intersection(eleptr, result, ceil_divide(inbitlen,8), masks, neles, server_masks,
        //		neles * NUM_HASH_FUNCTIONS, maskbytelen, perm);
        //wait for receiving thread
        if(pthread_join(rcv_filters_thread, NULL)) {
            cerr << "Error in joining pthread at cuckoo hashing!" << endl;
            exit(0);
        }
    }
}