//
// Created by root on 6/20/16.
//

#ifndef MULTIPARTYPSI_DEFS_H
#define MULTIPARTYPSI_DEFS_H

typedef unsigned int uint;
const char *LOOPBACK_ADDRESS = "127.0.0.1";

#define COPY_CTR(A) A(const A&)
#define ASSIGN_OP(A) operator=(const A&)
#endif //MULTIPARTYPSI_DEFS_H
