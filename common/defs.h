//
// Created by root on 6/20/16.
//

#ifndef MULTIPARTYPSI_DEFS_H
#define MULTIPARTYPSI_DEFS_H

#include <openssl/rand.h>
#include <emmintrin.h>

typedef __m128i block;


#define PRINT_PARTY(partyId) std::cout << "Party " << partyId << " "

#define COPY_CTR(A) A(const A&)
#define ASSIGN_OP(A) A &operator=(const A&)
#endif //MULTIPARTYPSI_DEFS_H


