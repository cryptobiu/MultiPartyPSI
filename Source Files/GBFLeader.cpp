//
// Created by root on 7/22/16.
//

#include "GBFLeader.h"

GBFLeader::GBFLeader(const map <uint32_t, boost::shared_ptr<uint8_t>> &leaderResults,
                     const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
          const boost::shared_ptr <uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                     boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
          const std::map <uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions) :
        Leader(leaderResults, hashInfo, numOfBins, secretShare, maskSizeInBytes, setSize, elements, elementSize, parties,
               numOfHashFunctions), GarbledBloomFilter(maskSizeInBytes, setSize) {

    for (auto &party : m_parties) {
        m_partiesFilters[party.first] = std::vector<boost::shared_ptr<GarbledBF>>();
        for (uint32_t i = 0; i < m_numOfHashFunctions; i++) {
            m_partiesFilters[party.first].push_back(GBF_Create());
        }
        m_hashFuncs[party.first] = std::vector<boost::shared_ptr<RangeHash>>();
        for (uint32_t i = 0; i < m_bfParam->k; i++) {
            m_hashFuncs[party.first].push_back(boost::shared_ptr<RangeHash>(new RangeHash()));
        }
    }

};

bool GBFLeader::isElementInAllSets(uint32_t index) {

    uint32_t binIndex = m_hashInfo.get()[index].binIndex;
    uint32_t newIndex = m_hashInfo.get()[index].tableIndex;
    uint32_t hash_index = m_hashInfo.get()[index].hashedBy;

    uint8_t* secret = &(m_secretShare.get()[binIndex*m_maskSizeInBytes]);

    for (auto &party : m_parties) {
        XOR(secret, m_leaderResults[party.first].get()+newIndex*m_maskSizeInBytes, m_maskSizeInBytes);
        auto value = GBF_query(m_partiesFilters[party.first][hash_index],m_hashFuncs[party.first],
                  m_elements.get()+index*m_elementSize,m_elementSize);
        XOR(secret, value.get(), m_maskSizeInBytes);
    }

    // 1 is always the leader Id
    for (uint32_t i = 0; i < m_maskSizeInBytes; i++) {
        if (secret[i] != 0) {
            return false;
        }
    }
    return true;
}

boost::shared_ptr<uint8_t> GBFLeader::GBF_query(const boost::shared_ptr<GarbledBF> &filter, vector<boost::shared_ptr<RangeHash>> hashes,
                                                uint8_t* element, int32_t eLen) {

    boost::shared_ptr<uint8_t> recovered(new uint8_t[m_maskSizeInBytes]);
    memset(recovered.get(), 0, m_maskSizeInBytes);
    int32_t* indexes = filter->indexes;
    //memset(indexes,0,hashNum);

    for (int i = 0; i < hashes.size(); i++) {
        int32_t index=RangeHash_Digest(hashes[i].get(), element, eLen);
        indexes[i]=index;

        if (!exists(index, indexes, i)) {
            xorByteArray(recovered.get(), &filter->data[index*filter->m_GBFSigmaByteLen], filter->m_GBFSigmaByteLen);
        }
    }
    return recovered;
}

void *GBFLeader::receiveKeysAndFilters(void *ctx_tmp) {

    filter_rcv_ctx* ctx = (filter_rcv_ctx*) ctx_tmp;

    uint32_t keySize = ctx->securityParameter/8;
    for (uint32_t i = 0; i < ctx->numHashes; i++) {
        boost::shared_ptr<uint8_t > key(new uint8_t[keySize]);
        ctx->sock->Receive(key.get(), keySize);
        RangeHash_Create(ctx->hashes[i], key.get(), keySize, ctx->filterSize);

    }

    for (uint32_t i = 0; i < ctx->numOfHashFunction; i++) {
        ctx->sock->Receive(ctx->filters[i]->data, ctx->filterSize*ctx->maskbytelen);
    }
}

void GBFLeader::receiveServerData() {

    vector<pthread_t> rcv_filters_threads;

    boost::shared_ptr<filter_rcv_ctx> rcv_ctxs(new filter_rcv_ctx[m_parties.size()+1]);
    for (auto &party : m_parties) {
        pthread_t rcv_filter_thread;

        //receive_masks(server_masks, NUM_HASH_FUNCTIONS * neles, maskbytelen, sock[0]);
        //use a separate thread to receive the server's masks
        (rcv_ctxs.get())[party.first - 1].filters = new GarbledBF*[m_numOfHashFunctions];
        for (uint32_t i =0; i < m_numOfHashFunctions; i++ ) {
            (rcv_ctxs.get())[party.first - 1].filters[i] = m_partiesFilters[party.first][i].get();
        }

        (rcv_ctxs.get())[party.first - 1].hashes = new RangeHash*[m_bfParam->k];
        for (uint32_t i =0; i < m_bfParam->k; i++ ) {
            (rcv_ctxs.get())[party.first - 1].hashes[i] = m_hashFuncs[party.first][i].get();
        }

        (rcv_ctxs.get())[party.first - 1].filterSize = m_bfParam->m;
        (rcv_ctxs.get())[party.first - 1].numOfHashFunction = m_numOfHashFunctions;
        (rcv_ctxs.get())[party.first - 1].maskbytelen = m_maskSizeInBytes;
        (rcv_ctxs.get())[party.first - 1].numHashes = m_bfParam->k;

        std::cout << "security parameter is " << m_securityParameter << std::endl;
        (rcv_ctxs.get())[party.first - 1].securityParameter = m_securityParameter;
        (rcv_ctxs.get())[party.first - 1].sock = party.second.get();


        if(pthread_create(&rcv_filter_thread, NULL, GBFLeader::receiveKeysAndFilters, (void*) (&(rcv_ctxs.get()[party.first - 1])))) {
            cerr << "Error in creating new pthread at cuckoo hashing!" << endl;
            exit(0);
        }

        rcv_filters_threads.push_back(rcv_filter_thread);
    }

    for (auto &rcv_filters_thread : rcv_filters_threads) {
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