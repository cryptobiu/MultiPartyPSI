//
// Created by root on 7/21/16.
//

#include "LeaderFactory.h"

#include "NaiveLeader.h"

boost::shared_ptr<Leader> LeaderFactory::getLeader(enum Strategy strategy, uint8_t **leaderResults, uint32_t *binIds, uint32_t *perm, uint32_t numOfBins,
                                                     uint8_t *secretShare, uint32_t maskSizeInBytes, uint32_t setSize, std::map<uint32_t, CSocket*> parties,
                                                     uint32_t numOfHashFunctions) {
    if (strategy == Strategy::NAIVE_METHOD_SMALL_N) {
        return boost::shared_ptr<Leader>(new NaiveLeader(leaderResults, binIds, perm, numOfBins,
                secretShare, maskSizeInBytes, setSize, parties, numOfHashFunctions));
    }
    throw(system_error());
}