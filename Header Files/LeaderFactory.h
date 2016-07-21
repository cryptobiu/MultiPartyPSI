//
// Created by root on 7/21/16.
//

#ifndef MULTIPARTYPSI_LEADERFACTORY_H
#define MULTIPARTYPSI_LEADERFACTORY_H


#include <boost/shared_ptr.hpp>
#include "Leader.h"

using boost::shared_ptr;

class LeaderFactory {
public:
    LeaderFactory();
    virtual ~LeaderFactory() {};

    static boost::shared_ptr<Leader> getLeader(enum Strategy strategy, uint8_t **leaderResults, uint32_t *binIds, uint32_t *perm, uint32_t numOfBins,
                                                 uint8_t *secretShare, uint32_t maskSizeInBytes, uint32_t setSize, std::map<uint32_t, CSocket*> parties,
                                                 uint32_t numOfHashFunctions);
private:
    COPY_CTR(LeaderFactory);
    ASSIGN_OP(LeaderFactory);
};


#endif //MULTIPARTYPSI_LEADERFACTORY_H
