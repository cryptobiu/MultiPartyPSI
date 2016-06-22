//
// Created by liork on 30/05/16.
//

#ifndef SCAPIPRG_PRG_HPP
#define SCAPIPRG_PRG_HPP


#include <iostream>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <emmintrin.h>
#include <malloc.h>
#include "aes_locl.h"

extern unsigned int OPENSSL_ia32cap_P[];
# define AESNI_CAPABLE (OPENSSL_ia32cap_P[1]&(1<<(57-32)))

using namespace std;

typedef unsigned char byte;

#define DEFAULT_CACHE_SIZE 60000

class PRG_CTR128
{
public:
    PRG_CTR128(int max_size);
    ~PRG_CTR128();
    byte *inc(int size);

private:
    void spillCounter();
    void recordCounter(int size);
    void doInc(int size);
    void AES_ctr128_inc(byte *counter);


    byte *m_buf;
    int m_max_size;
    byte m_ctr[16] __attribute__((aligned(16))) = {0};
    __m128i m_CONST_ONE;
};


class PRG
{
// TODO: it can't be private !!
// private:
public:
    PRG(int cahchedSize=DEFAULT_CACHE_SIZE);
    PRG(byte *key,int cahchedSize=DEFAULT_CACHE_SIZE);
    PRG(byte *key, byte *iv,int cahchedSize=DEFAULT_CACHE_SIZE);

public:
    static PRG& instance();
    ~PRG();
    byte *getRandomBytes();
    uint32_t getRandom();
    void prepare(int isPlanned = 1);


private:
    void checkAESNI();

    static unsigned char m_defualtkey[16];
    static unsigned char m_defaultiv[16];

    PRG_CTR128 m_ctr128;
    EVP_CIPHER_CTX m_enc;
    const byte* m_key;
    int m_cahchedSize;
    byte *m_cachedRandoms;
    const byte *m_iv;
    byte* m_ctr;
    int m_cachedRandomsIdx;
    int m_idx;
    uint32_t *m_pIdx;
    uint32_t m_u1;
    uint32_t m_u2;
    uint32_t m_u3;
    uint32_t m_u4;



    static PRG *_prg;
};

class UnsupportAESNIException : public exception
{

public:
    virtual const char* what() const throw();
};

#endif //SCAPIPRG_PRG_HPP
