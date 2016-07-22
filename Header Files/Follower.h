//
// Created by root on 7/21/16.
//

#ifndef MULTIPARTYPSI_FOLLOWER_H
#define MULTIPARTYPSI_FOLLOWER_H

#include <stdlib.h>
#include "defs.h"
#include "../PSI/src/util/socket.h"
#include "boost/shared_ptr.hpp"

struct FollowerSet {
    uint8_t *m_elements;
    uint32_t m_numOfElements;
    uint32_t m_elementSizeInBytes;
    uint8_t *m_hashTable;
    uint32_t *m_numOfElementsInBin;
    uint32_t m_numOfBins;
    uint32_t m_numOfHashFunctions;
    uint8_t *m_masks;
    uint32_t m_maskSizeInBytes;
};

class Follower {
public:
    Follower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader) :
            m_followerSet(followerSet), m_secretShare(secretShare), m_leader(leader) {};
    virtual ~Follower() {};

    virtual void run()=0;
protected:
    const FollowerSet &m_followerSet;
    boost::shared_ptr<uint8_t> m_secretShare;
    CSocket &m_leader;
private:
    COPY_CTR(Follower);
    ASSIGN_OP(Follower);
};

#endif //MULTIPARTYPSI_FOLLOWER_H
