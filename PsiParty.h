//
// Created by root on 6/20/16.
//

#ifndef LIBSCAPI_PSIPARTY_H
#define LIBSCAPI_PSIPARTY_H

#include "common/MultiPartyPlayer.h"
#include "common/statistics.h"
#include "PSI/src/util/typedefs.h"
#include "PSI/src/util/crypto/crypto.h"

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
    void runAsFollower(CSocket *leader);
    void finishAndReportStatsToServer();
    void syncronize();
    bool isElementInAllSets(uint32_t index, uint8_t **partiesResults);
    void runLeaderAgainstFollower(std::pair<uint32_t, CSocket*> party, uint8_t **partyResult);

    uint getElementSize() {
        return 16;
    }

    COPY_CTR(PsiParty);
    ASSIGN_OP(PsiParty);

    const uint32_t SIZE_OF_BLOCK = 16;
    uint32_t m_setSize;
    uint32_t m_elementSizeInBits;
    uint32_t m_blockSizeInBits;
    std::vector<boost::shared_ptr<block>> m_secretShares;
    uint8_t *m_elements;
    crypto* m_crypt;
    struct statistics m_statistics;
};

#endif //LIBSCAPI_PSIPARTY_H
