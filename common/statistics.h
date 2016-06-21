//
// Created by Naor on 21/06/2016.
//

#ifndef MULTIPARTYPSI_STATISTICS_H
#define MULTIPARTYPSI_STATISTICS_H

union specificStats {
    clock_t afterSend;
    clock_t aftetComputing;
};

struct statistics {
    uint partyId;
    clock_t beginTime;
    clock_t afterSharing;
    clock_t afterOTs;
    union specificStats specificStats;
};
#endif //MULTIPARTYPSI_STATISTICS_H
