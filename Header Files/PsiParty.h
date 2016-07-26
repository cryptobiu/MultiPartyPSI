//
// Created by root on 6/20/16.
//

#ifndef LIBSCAPI_PSIPARTY_H
#define LIBSCAPI_PSIPARTY_H

#include "MultiPartyPlayer.h"
#include "statistics.h"
#include "typedefs.h"
#include "crypto/crypto.h"
#include "boost/shared_ptr.hpp"

class PsiParty : public MultiPartyPlayer {
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
    void syncronize();

    void initializeMaskSize();
    void initializeCrypto();
    void setBinsParameters();
    void runLeaderAgainstFollower(const std::pair<uint32_t, boost::shared_ptr<CSocket>> &party, const boost::shared_ptr<uint8_t> &leaderResults,
                                  const boost::shared_ptr<uint32_t> &nelesinbin, uint32_t outbitlen, const boost::shared_ptr<uint8_t> &hash_table);

    uint32_t getMaskSizeInBytes() {
        return ceil_divide(m_maskbitlen, 8);
    }

    void LoadConfiguration();

    const double EPSILON=1.2;
    COPY_CTR(PsiParty);
    ASSIGN_OP(PsiParty);

    uint32_t m_setSize;
    uint32_t m_elementSizeInBits;
    uint32_t m_maxBinSize;
    boost::shared_ptr<uint8_t> m_secretShare;
    boost::shared_ptr<uint8_t> m_elements;
    boost::shared_ptr<crypto> m_crypt;
    uint32_t m_maskbitlen;
    struct statistics m_statistics;
    enum Strategy m_strategy;
    uint32_t m_numOfBins;
    prf_state_ctx m_prfState;

    uint32_t m_symsecbits;

    boost::shared_ptr<uint8_t> m_eleptr;

    uint32_t m_internal_bitlen;
    uint32_t m_seedSize;
};

#endif //LIBSCAPI_PSIPARTY_H
