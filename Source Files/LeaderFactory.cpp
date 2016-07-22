//
// Created by root on 7/21/16.
//

#include "../Header Files/LeaderFactory.h"

#include "../Header Files/NaiveLeader.h"

boost::shared_ptr<Leader> LeaderFactory::getLeader(enum Strategy strategy, const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults, const boost::shared_ptr<uint32_t> &binIds, const boost::shared_ptr<uint32_t> &perm, uint32_t numOfBins,
                                                     const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize, std::map<uint32_t, CSocket*> parties,
                                                     uint32_t numOfHashFunctions) {
    if (strategy == Strategy::NAIVE_METHOD_SMALL_N) {
        return boost::shared_ptr<Leader>(new NaiveLeader(leaderResults, binIds, perm, numOfBins,
                secretShare, maskSizeInBytes, setSize, parties, numOfHashFunctions));
    }
    throw(system_error());
}