//
// Created by root on 8/2/16.
//

#ifndef MULTIPARTYPSI_BASEMPSIPARTY_H
#define MULTIPARTYPSI_BASEMPSIPARTY_H

#include "MultiPartyPlayer.h"
#include <crypto/crypto.h>


class BaseMPSIParty : public MultiPartyPlayer {
public:
    BaseMPSIParty(uint32_t partyId, ConfigFile &config, boost::asio::io_service &ioService);
    virtual ~BaseMPSIParty() {};
    virtual void run()=0;
    void runAndLog();
    void syncronize();
protected:
    uint32_t m_setSize;
    boost::shared_ptr<uint8_t> m_elements;
    secParameters m_parameters;
    uint32_t m_elementSizeInBits;
    uint32_t m_seedSize;
    boost::shared_ptr<uint8_t> m_seedBuf;
    boost::shared_ptr<crypto> m_crypt;
    prf_state_ctx m_prfState;

    crypto *initializeCrypto();
private:
    COPY_CTR(BaseMPSIParty);
    ASSIGN_OP(BaseMPSIParty);

    void LoadConfiguration();
};


#endif //MULTIPARTYPSI_BASEMPSIPARTY_H
