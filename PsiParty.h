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
    PsiParty(uint partyId, ConfigFile &config, boost::asio::io_service &ioService);
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

    uint32_t getMaskSizeInBytes() {
        return ceil_divide(m_maskbitlen, 8);
    }

    void XOR(byte *xoree1, byte *xoree2);

    bool isZeroXOR(byte *formerShare, uint32_t partyNum, uint8_t **partiesResults);

    COPY_CTR(PsiParty);
    ASSIGN_OP(PsiParty);

    const uint32_t SIZE_OF_BLOCK = 16;
    uint32_t m_setSize;
    uint32_t m_elementSizeInBits;
    uint32_t m_blockSizeInBits;
    byte *m_secretShare;
    uint8_t *m_elements;
    crypto* m_crypt;
    uint32_t m_maskbitlen;
    struct statistics m_statistics;
};

#endif //LIBSCAPI_PSIPARTY_H
