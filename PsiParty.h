//
// Created by root on 6/20/16.
//

#ifndef LIBSCAPI_PSIPARTY_H
#define LIBSCAPI_PSIPARTY_H

#include "common/MultiPartyPlayer.h"

typedef __m128i block;

class PsiParty : public MultiPartyPlayer {
public:
    PsiParty(uint partyId, ConfigFile config, boost::asio::io_service &ioService) :
        MultiPartyPlayer(partyId, config, ioService), m_secretShare(NULL) {};
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

    uint getElementSize() {
        return 16;
    }

    COPY_CTR(PsiParty);
    ASSIGN_OP(PsiParty);

    block *m_secretShare;
};

#endif //LIBSCAPI_PSIPARTY_H
