//
// Created by root on 8/1/16.
//

#ifndef MULTIPARTYPSI_KISSNERPARTY_H
#define MULTIPARTYPSI_KISSNERPARTY_H

#include "Paillier.h"

class KissnerParty : public PaillierParty {
public:
    KissnerParty(uint32_t partyId, ConfigFile &config, boost::asio::io_service &ioService);
    virtual ~KissnerParty() {};

    void run();
private:
    COPY_CTR(KissnerParty);
    ASSIGN_OP(KissnerParty);
};


#endif //MULTIPARTYPSI_KISSNERPARTY_H
