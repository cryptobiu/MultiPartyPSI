//
// Created by root on 6/20/16.
//

#ifndef LIBSCAPI_PSIPARTY_H
#define LIBSCAPI_PSIPARTY_H

#include "MultiPartyPlayer.h"
#include "BaseMPSIParty.h"
#include "statistics.h"
#include "typedefs.h"
#include "crypto/crypto.h"
#include "boost/shared_ptr.hpp"

class PsiParty : public BaseMPSIParty {
public:
    PsiParty(uint partyId, ConfigFile &config, boost::asio::io_service &ioService);
    virtual ~PsiParty() { };

    void run();

private:

    /**
     * Works as follows:
     * randomly pick $numberOfParties$-1 random shares with size $ElementSize$ and send them to all the parties
     * receive $numberOfParties$-1 shares from all the parties with size $ElementSize$
     * compute PRG on each of the shares with expansion factor of $SetSize$ and XOR them all.
     */
    void additiveSecretShare();

    void runAsLeader();
    void runAsFollower(CSocket &leader);
    void finishAndReportStatsToServer();

    void initializeMaskSize();
    void setBinsParameters();
    void runLeaderAgainstFollower(const std::pair<uint32_t, boost::shared_ptr<CSocket>> &party, const boost::shared_ptr<uint8_t> &leaderResults,
                                  const boost::shared_ptr<uint32_t> &nelesinbin, uint32_t outbitlen, const boost::shared_ptr<uint8_t> &hash_table);

    uint32_t getMaskSizeInBytes() {
        return ceil_divide(m_maskbitlen, 8);
    }

    const double EPSILON=1.2;
    COPY_CTR(PsiParty);
    ASSIGN_OP(PsiParty);

    uint32_t m_maxBinSize;
    boost::shared_ptr<uint8_t> m_secretShare;
    uint32_t m_maskbitlen;
    struct statistics m_statistics;
    enum Strategy m_strategy;
    uint32_t m_numOfBins;

    boost::shared_ptr<uint8_t> m_eleptr;

    uint32_t m_internal_bitlen;
};

#endif //LIBSCAPI_PSIPARTY_H
