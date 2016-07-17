//
// Created by Naor on 21/06/2016.
//

#ifndef MULTIPARTYPSI_STATISTICS_H
#define MULTIPARTYPSI_STATISTICS_H

union specificStats {
    float afterSend;
    float aftetComputing;
};

struct statistics {
    uint32_t partyId;
    float beginTime;
    float afterSharing;
    float afterOTs;
    union specificStats specificStats;
};
#endif //MULTIPARTYPSI_STATISTICS_H
