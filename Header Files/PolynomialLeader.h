//
// Created by root on 7/25/16.
//

#ifndef MULTIPARTYPSI_POLYNOMIALLEADER_H
#define MULTIPARTYPSI_POLYNOMIALLEADER_H

#include "Leader.h"
#include <boost/shared_ptr.hpp>
#include <NTL/GF2EX.h>

struct polynomial_rcv_ctx {
    NTL::GF2EX **polynoms;
    uint32_t setSize;
    uint32_t numOfHashFunction;
    uint32_t maskbytelen;
    CSocket* sock;
};

class PolynomialLeader : public Leader {
public:
    PolynomialLeader(const map <uint32_t, boost::shared_ptr<uint8_t>> &leaderResults,
              const boost::shared_ptr <uint32_t> &bin_ids, const boost::shared_ptr <uint32_t> &perm, uint32_t numOfBins,
              const boost::shared_ptr <uint8_t> &secretShare, uint32_t maskSizeInBytes, uint32_t setSize,
              boost::shared_ptr<uint8_t> elements, uint32_t elementSize, const boost::shared_ptr<uint32_t> &hashed_by,
              const std::map <uint32_t, boost::shared_ptr<CSocket>> &parties, uint32_t numOfHashFunctions);

    virtual ~PolynomialLeader() { };

private:

    COPY_CTR(PolynomialLeader);
    ASSIGN_OP(PolynomialLeader);

    bool isElementInAllSets(uint32_t index);
    void receiveServerData();

    static void *receivePolynomials(void *ctx_tmp);

    std::map<uint32_t , std::vector<boost::shared_ptr<NTL::GF2EX>>> m_partiesPolynomials;
};


#endif //MULTIPARTYPSI_POLYNOMIALLEADER_H
