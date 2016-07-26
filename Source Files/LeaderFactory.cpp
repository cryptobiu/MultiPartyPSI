//
// Created by root on 7/21/16.
//

#include "LeaderFactory.h"
#include "NaiveLeader.h"
#include "GBFLeader.h"
#include "PolynomialLeader.h"

boost::shared_ptr<Leader> LeaderFactory::getLeader(enum Strategy strategy, const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults,
                                                   const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins, const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes,
                                                   uint32_t setSize, boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
                                                   const std::map<uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions) {
    switch(strategy) {
        case Strategy::NAIVE_METHOD_SMALL_N:
            return boost::shared_ptr<Leader>(new NaiveLeader(leaderResults, hashInfo, numOfBins,
                                                             secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions));
        case Strategy::BLOOM_FILTER:
            return boost::shared_ptr<Leader>(new GBFLeader(leaderResults, hashInfo, numOfBins,
                                                           secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions));
        case Strategy::POLYNOMIALS:
            return boost::shared_ptr<Leader>(new PolynomialLeader(leaderResults, hashInfo, numOfBins,
                                                           secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions));
        default:
            break;
    }
    throw(system_error());
}