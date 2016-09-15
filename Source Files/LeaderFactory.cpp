//
// Created by root on 7/21/16.
//

#include "LeaderFactory.h"
#include "Leaders/NaiveLeader.h"
#include "Leaders/NaiveInverseLeader.h"
#include "Leaders/GBFLeader.h"
#include "Leaders/PolynomialLeader.h"
#include "Leaders/SimpleHashingPolynomialLeader.h"
#include "Leaders/SimpleHashingNaiveLeader.h"
#include "Leaders/SimpleHashingNaiveInverseLeader.h"
#include <system_error>
#include <Leaders/TwoPartyLeader.h>

boost::shared_ptr<Leader> LeaderFactory::getLeader(enum Strategy strategy, const map<uint32_t , boost::shared_ptr<uint8_t>>& leaderResults,
                                                   const boost::shared_ptr<CuckooHashInfo> &hashInfo, uint32_t numOfBins, const boost::shared_ptr<uint8_t> &secretShare, uint32_t maskSizeInBytes,
                                                   uint32_t setSize, boost::shared_ptr<uint8_t> elements, uint32_t elementSize,
                                                   const std::map<uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions, uint32_t maxBinSize,
                                                   const secParameters &parameters, uint32_t numCores) {
    switch(strategy) {
        case Strategy::NAIVE_METHOD_SMALL_N:
            return boost::shared_ptr<Leader>(new NaiveLeader(leaderResults, hashInfo, numOfBins,
                                                             secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters, numCores));
        case Strategy::NAIVE_METHOD_LARGE_N:
            return boost::shared_ptr<Leader>(new NaiveInverseLeader(leaderResults, hashInfo, numOfBins,
                                                             secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters, numCores));
        case Strategy::BLOOM_FILTER:
            return boost::shared_ptr<Leader>(new GBFLeader(leaderResults, hashInfo, numOfBins,
                                                           secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters, numCores));
        case Strategy::POLYNOMIALS:
            return boost::shared_ptr<Leader>(new PolynomialLeader(leaderResults, hashInfo, numOfBins,
                                                           secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters, numCores));
        case Strategy::POLYNOMIALS_SIMPLE_HASH:
            return boost::shared_ptr<Leader>(new SimpleHashingPolynomialLeader(leaderResults, hashInfo, numOfBins,
                                                                  secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters,
                                                                               numCores, maxBinSize));
        case Strategy::SIMPLE_HASH:
            return boost::shared_ptr<Leader>(new SimpleHashingNaiveLeader(leaderResults, hashInfo, numOfBins,
                                                                   secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters,
                                                                          numCores, maxBinSize));
        case Strategy::GAUSS_SIMPLE_HASH:
            return boost::shared_ptr<Leader>(new SimpleHashingNaiveInverseLeader(leaderResults, hashInfo, numOfBins,
                                                                          secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters,
                                                                                 numCores, maxBinSize));
        case Strategy::TWO_PARTY:
            return boost::shared_ptr<Leader>(new TwoPartyLeader(leaderResults, hashInfo, numOfBins,
                                                             secretShare, maskSizeInBytes, setSize, elements, elementSize, parties, numOfHashFunctions, parameters, numCores));
        default:
            break;
    }
    throw(std::system_error());
}
