//
// Created by root on 6/20/16.
//

#ifndef LIBSCAPI_PSIPARTY_H
#define LIBSCAPI_PSIPARTY_H

#include "common/MultiPartyPlayer.h"
#include "common/statistics.h"

typedef __m128i block;

class PsiParty : public MultiPartyPlayer {
public:
    PsiParty(uint partyId, ConfigFile config, boost::asio::io_service &ioService);
    virtual ~PsiParty() {};

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
    void runLeaderAgainstFollower(const boost::shared_ptr<CommPartyTCPSynced> &leader);
    void runAsFollower(const boost::shared_ptr<CommPartyTCPSynced> &leader);
    void finishAndReportStatsToServer();

    uint getElementSize() {
        return 16;
    }

    COPY_CTR(PsiParty);
    ASSIGN_OP(PsiParty);

    const uint SIZE_OF_BLOCK = 16;
    uint m_setSize;
    uint m_elementSizeInBits;
    uint m_blockSizeInBits;
    std::vector<boost::shared_ptr<block>> m_secretShares;
    std::vector<uint32> m_elements;
    struct statistics m_statistics;
};

#endif //LIBSCAPI_PSIPARTY_H
