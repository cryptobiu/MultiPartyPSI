//
// Created by root on 7/22/16.
//

#include "Leaders/GBFLeader.h"

GBFLeader::GBFLeader(const map <uint32_t, boost::shared_ptr<uint8_t>> &leaderResults,
                     const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
          const boost::shared_ptr <uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                     boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
          const std::map <uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions, const secParameters &parameters) :
        Leader(leaderResults, hashInfo, numOfBins, secretShare, maskSizeInBytes, setSize, elements, elementSize, parties,
               numOfHashFunctions, parameters), GarbledBloomFilter(maskSizeInBytes, m_parameters.m_statSecParameter, setSize) {

    uint32_t keySize = m_parameters.m_symSecParameter/8;

    for (auto &party : m_parties) {
        m_partiesFilters[party.first] = std::vector<boost::shared_ptr<GarbledBF>>();
        for (uint32_t i = 0; i < m_numOfHashFunctions; i++) {
            m_partiesFilters[party.first].push_back(GBF_Create());
        }
        m_hashFuncs[party.first] = std::vector<boost::shared_ptr<RangeHash>>();
        for (uint32_t i = 0; i < m_bfParam->k; i++) {
            m_hashFuncs[party.first].push_back(boost::shared_ptr<RangeHash>(new RangeHash()));
            m_partiesKeys[party.first].push_back(boost::shared_ptr<uint8_t >(new uint8_t[keySize]));
        }
    }

};

bool GBFLeader::isElementInAllSets(uint32_t index, uint32_t binIndex, uint32_t tableIndex, uint32_t hashFuncIndex, uint8_t *secret) {

    for (auto &party : m_parties) {
        XOR(secret, m_leaderResults[party.first].get()+tableIndex*m_maskSizeInBytes, m_maskSizeInBytes);
        auto value = GBF_query(m_partiesFilters[party.first][hashFuncIndex],m_hashFuncs[party.first],
                  m_elements.get()+index*m_elementSize,m_elementSize,m_partiesKeys[party.first]);
        XOR(secret, value.get(), m_maskSizeInBytes);
    }

    return isZero(secret, m_maskSizeInBytes);
}

boost::shared_ptr<uint8_t> GBFLeader::GBF_query(const boost::shared_ptr<GarbledBF> &filter, vector<boost::shared_ptr<RangeHash>> hashes,
                                                uint8_t* element, int32_t eLen, vector<boost::shared_ptr<uint8_t>> keys) {

    uint32_t keySize = m_parameters.m_symSecParameter/8;

    vector<boost::shared_ptr<RangeHash>> hashFuncs;
    for (int i = 0; i < hashes.size(); i++) {
        boost::shared_ptr<RangeHash> hashFunc(new RangeHash());
        RangeHash_Create(hashFunc.get(), keys[i].get(), keySize, m_bfParam->m);
        hashFuncs.push_back(hashFunc);
    }


    boost::shared_ptr<uint8_t> recovered(new uint8_t[m_maskSizeInBytes]);
    memset(recovered.get(), 0, m_maskSizeInBytes);
    //int32_t* indexes = filter->indexes;
    int32_t* indexes = (int32_t*)calloc(m_bfParam->k, sizeof(int32_t));
    //memset(indexes,0,hashNum);

    for (int i = 0; i < hashFuncs.size(); i++) {
        int32_t index=RangeHash_Digest(hashFuncs[i].get(), element, eLen);
        indexes[i]=index;

        if (!exists(index, indexes, i)) {
            xorByteArray(recovered.get(), &filter->data[index*filter->m_GBFSigmaByteLen], filter->m_GBFSigmaByteLen);
        }
    }

    free(indexes);
    return recovered;
}

void *GBFLeader::receiveKeysAndFilters(void *ctx_tmp) {

    filter_rcv_ctx* ctx = (filter_rcv_ctx*) ctx_tmp;

    uint32_t keySize = ctx->symSecurityParameter/8;
    for (uint32_t i = 0; i < ctx->numHashes; i++) {
        ctx->sock->Receive(ctx->keys[i], keySize);
        RangeHash_Create(ctx->hashes[i], ctx->keys[i], keySize, ctx->filterSize);

    }

    for (uint32_t i = 0; i < ctx->numOfHashFunction; i++) {
        ctx->sock->Receive(ctx->filters[i]->data, ctx->filterSize*ctx->maskbytelen);
    }
}

void GBFLeader::receiveServerData() {

    boost::shared_ptr<filter_rcv_ctx> rcv_ctxs(new filter_rcv_ctx[m_parties.size()+1]);
    for (auto &party : m_parties) {
        //receive_masks(server_masks, NUM_HASH_FUNCTIONS * neles, maskbytelen, sock[0]);
        //use a separate thread to receive the server's masks
        (rcv_ctxs.get())[party.first - 1].filters = new GarbledBF *[m_numOfHashFunctions];
        for (uint32_t i = 0; i < m_numOfHashFunctions; i++) {
            (rcv_ctxs.get())[party.first - 1].filters[i] = m_partiesFilters[party.first][i].get();
        }

        (rcv_ctxs.get())[party.first - 1].hashes = new RangeHash *[m_bfParam->k];
        (rcv_ctxs.get())[party.first - 1].keys = new uint8_t *[m_bfParam->k];
        for (uint32_t i = 0; i < m_bfParam->k; i++) {
            (rcv_ctxs.get())[party.first - 1].hashes[i] = m_hashFuncs[party.first][i].get();
            (rcv_ctxs.get())[party.first - 1].keys[i] = m_partiesKeys[party.first][i].get();
        }

        (rcv_ctxs.get())[party.first - 1].filterSize = m_bfParam->m;
        (rcv_ctxs.get())[party.first - 1].numOfHashFunction = m_numOfHashFunctions;
        (rcv_ctxs.get())[party.first - 1].maskbytelen = m_maskSizeInBytes;
        (rcv_ctxs.get())[party.first - 1].numHashes = m_bfParam->k;

        (rcv_ctxs.get())[party.first - 1].symSecurityParameter = m_parameters.m_symSecParameter;
        (rcv_ctxs.get())[party.first - 1].sock = party.second.get();
    }

    receiveServerDataInThreads<filter_rcv_ctx>(rcv_ctxs, &GBFLeader::receiveKeysAndFilters);
}