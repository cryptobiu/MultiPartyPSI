//
// Created by root on 7/21/16.
//

#include "../Header Files/LeaderFactory.h"

#include "../Header Files/NaiveLeader.h"
#include "../Header Files/GBFLeader.h"

boost::shared_ptr<Leader> LeaderFactory::getLeader(enum Strategy strategy, const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults, const boost::shared_ptr<uint32_t> &binIds, const boost::shared_ptr<uint32_t> &perm, uint32_t numOfBins,
                                                     const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
                                                   boost::shared_ptr<uint8_t> elements, uint32_t elementSize, const boost::shared_ptr<uint32_t> &hashed_by,
                                                   const std::map<uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions) {
    if (strategy == Strategy::NAIVE_METHOD_SMALL_N) {
        return boost::shared_ptr<Leader>(new NaiveLeader(leaderResults, binIds, perm, numOfBins,
                secretShare, maskSizeInBytes, setSize, elements, elementSize, hashed_by, parties, numOfHashFunctions));
    }
    else if (strategy == Strategy::BLOOM_FILTER) {
        return boost::shared_ptr<Leader>(new GBFLeader(leaderResults, binIds, perm, numOfBins,
                                                         secretShare, maskSizeInBytes, setSize, elements, elementSize, hashed_by, parties, numOfHashFunctions));
    }
    throw(system_error());
}