//
// Created by root on 7/21/16.
//

#ifndef MULTIPARTYPSI_LEADER_H
#define MULTIPARTYPSI_LEADER_H

#include "defs.h"
#include <map>
#include "socket.h"
#include "boost/shared_ptr.hpp"

typedef void *receiveDataFromFollower(void *);

class Leader {
public:
    Leader(const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults, const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins,
           const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
           boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
           const std::map<uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions, const secParameters &parameters):
            m_leaderResults(leaderResults), m_hashInfo(hashInfo),
            m_numOfBins(numOfBins), m_secretShare(secretShare), m_maskSizeInBytes(maskSizeInBytes), m_setSize(setSize),
            m_parties(parties), m_numOfHashFunctions(numOfHashFunctions), m_elements(elements), m_elementSize(elementSize),
            m_parameters(parameters) {}
    virtual ~Leader() {};

    virtual vector<uint32_t> run();

protected:

    template<class T>
    void receiveServerDataInThreads(const boost::shared_ptr<T>&, receiveDataFromFollower *func);

    virtual void receiveServerData()=0;
    virtual bool isElementInAllSets(uint32_t index, uint32_t binIndex, uint32_t tableIndex, uint32_t hashFuncIndex, uint8_t *secret)=0;

    map<uint32_t , boost::shared_ptr<uint8_t>> m_leaderResults;
    boost::shared_ptr<CuckooHashInfo> m_hashInfo;
    uint32_t m_numOfBins;
    boost::shared_ptr<uint8_t> m_secretShare;
    uint32_t m_maskSizeInBytes;
    uint32_t m_setSize;
    std::map<uint32_t, boost::shared_ptr<CSocket>> m_parties;
    uint32_t m_numOfHashFunctions;
    boost::shared_ptr<uint8_t> m_elements;
    uint32_t m_elementSize;
    secParameters m_parameters;

private:
    COPY_CTR(Leader);
    ASSIGN_OP(Leader);
};

template<class T>
void Leader::receiveServerDataInThreads(const boost::shared_ptr<T>& rcv_ctxs, receiveDataFromFollower *func) {
    vector<pthread_t> rcv_threads;

    for (auto &party : m_parties) {
        pthread_t rcv_thread;

        if(pthread_create(&rcv_thread, NULL, func, (void*) (&(rcv_ctxs.get()[party.first - 1])))) {
            cerr << "Error in creating new pthread at cuckoo hashing!" << endl;
            exit(0);
        }

        rcv_threads.push_back(rcv_thread);
    }

    for (auto &rcv_thread : rcv_threads) {
        //meanwhile generate the hash table
        //GHashTable* map = otpsi_create_hash_table(ceil_divide(inbitlen,8), masks, neles, maskbytelen, perm);
        //intersect_size = otpsi_find_intersection(eleptr, result, ceil_divide(inbitlen,8), masks, neles, server_masks,
        //		neles * NUM_HASH_FUNCTIONS, maskbytelen, perm);
        //wait for receiving thread
        if(pthread_join(rcv_thread, NULL)) {
            cerr << "Error in joining pthread at cuckoo hashing!" << endl;
            exit(0);
        }
    }
}


#endif //MULTIPARTYPSI_LEADER_H
