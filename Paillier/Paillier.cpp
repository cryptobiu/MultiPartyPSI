//
// Created by root on 8/1/16.
//

#include "Paillier.h"
#include <NTL/vec_ZZ_p.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <vector>

PaillierParty::PaillierParty(uint32_t partyId, ConfigFile &config, boost::asio::io_service &ioService) : MultiPartyPlayer(partyId,config,ioService) {
    uint32_t symSecurityParameter = stoi(m_config.Value("General", "symSecurityParameter"));
    uint32_t bitSize;
    switch(symSecurityParameter) {
        case 80:
            bitSize = 2048; // some suggest 1024
            break;
        case 128:
            bitSize = 2560; // some suggest 2048(ANSSI) or 3072(NIST)
            break;
        default:
            throw(system_error());
    }

    uint32_t leaderId = stoi(m_config.Value("General", "leaderId"));

    if (m_partyId == leaderId) {
        ZZ pTag = GenGermainPrime_ZZ(bitSize/2);
        ZZ qTag = GenGermainPrime_ZZ(bitSize/2);
        ZZ p = 2 * pTag + 1;
        ZZ q = 2 * qTag + 1;
        m_n = p * q;
        m_m = pTag * qTag;
        ZZ_p::init(m_n);
        m_a = rep(random_ZZ_p());
        m_b = rep(random_ZZ_p());

        for (auto &party : m_parties) {
            sendZZTo(m_n,party.second);
            sendZZTo(m_m,party.second);
            sendZZTo(m_a,party.second);
            sendZZTo(m_b,party.second);
        }
    }
    else {
        receiveZZFrom(m_n, m_parties[leaderId]);
        receiveZZFrom(m_m, m_parties[leaderId]);
        receiveZZFrom(m_a, m_parties[leaderId]);
        receiveZZFrom(m_b, m_parties[leaderId]);
    }

    m_field = m_n * m_n;
    m_delta = factorial(m_numOfParties);

    ZZ_p::init(m_field);
    PRINT_PARTY(m_partyId) << "n is " << m_n << std::endl;
    PRINT_PARTY(m_partyId) << "a is " << m_a << std::endl;
    PRINT_PARTY(m_partyId) << "b is " << m_b << std::endl;

    PRINT_PARTY(m_partyId) << "b^n is " << rep(powerZZ(m_b,m_n)) << std::endl;
    PRINT_PARTY(m_partyId) << "(1+n)^a is " << rep(powerZZ(1+m_n, m_a)) << std::endl;
    m_g = rep(powerZZ(1+m_n, m_a)*powerZZ(m_b,m_n));

    PRINT_PARTY(m_partyId) << "g is " << m_g << std::endl;

    for (auto &party : m_parties) {
        double j = party.first;
        double prod = m_delta;
        for (auto &other : m_parties) {
            double jTag = other.first;
            if (j == jTag) {
                continue;
            }
            prod = prod*jTag/(jTag-j);
        }
        m_lagrangeBasis[party.first] = ZZ(static_cast<uint32_t>(prod));
    }

    secretShare();
}

void PaillierParty::sendZZTo(const ZZ& element, const boost::shared_ptr<CSocket> &socket) {
    int numBytes = NTL::NumBytes(element);
    vector<uint8_t> arr(numBytes);
    BytesFromZZ(arr.data(),element,numBytes);
    socket->Send(&numBytes,sizeof(int));
    socket->Send(arr.data(),numBytes);
}

void PaillierParty::receiveZZFrom(ZZ& element, const boost::shared_ptr<CSocket> &socket) {
    int numBytes;
    socket->Receive(&numBytes,sizeof(int));

    vector<uint8_t> arr(numBytes);
    socket->Receive(arr.data(),numBytes);

    ZZFromBytes(element, arr.data(),numBytes);

}

void PaillierParty::secretShare() {
    ZZ_p::init(m_n);
    ZZ beta = rep(random_ZZ_p());
    ZZ_p::init(m_n*m_m);
    ZZ_p freeCoefficient;
    freeCoefficient.init(beta*m_m);

    ZZ_pX polynomial;
    SetCoeff(polynomial, 0, freeCoefficient);

    for (uint32_t i=1; i < m_numOfParties; i++) {
        SetCoeff(polynomial, i, random_ZZ_p());
    }

    for (auto &party : m_parties) {
        sendZZTo(rep(eval(polynomial,ZZ_p(party.first))),party.second);
    }

    ZZ_p s_i = eval(polynomial,ZZ_p(m_partyId));
    for (auto &party : m_parties) {
        ZZ value;
        receiveZZFrom(value,party.second);
        ZZ_p coefficient;
        coefficient.init(value);
        s_i = s_i + coefficient;
    }

    m_share = rep(s_i);

    ZZ_p::init(m_n);
    ZZ_p pubKey;
    pubKey.init(m_a*beta*m_m);
    m_pubKey = rep(pubKey);
}

ZZ PaillierParty::encrypt(const ZZ& plain) {
    std::cout << "n is " << m_n << std::endl;
    ZZ_p::init(m_n);
    std::cout << "get random" << std::endl;
    ZZ x = rep(random_ZZ_p());
    std::cout << "field is " << m_field << std::endl;
    ZZ_p::init(m_field);
    std::cout << "calculating rep" << std::endl;

    std::cout << "g is " << m_g << std::endl;
    std::cout << "plain is " << plain << std::endl;
    std::cout << "x is " << x << std::endl;
    std::cout << "n is " << m_n << std::endl;

    return rep(powerZZ(m_g, plain)*powerZZ(x,m_n));
}

ZZ PaillierParty::partialDecrypt(const ZZ& cipher) {
    ZZ_p::init(m_field);
    return rep(powerZZ(cipher, 2*m_delta*m_share));
}

ZZ PaillierParty::decrypt(const std::map<uint32_t,ZZ> &partialCiphers,
                          const std::vector<ZZ> &pubKeys) {
    ZZ_p::init(m_n);

    ZZ_p sum(0);
    for (auto &pubKey : pubKeys) {
        ZZ_p value;
        value.init(pubKey);
        sum = sum + value;
    }

    ZZ_p a;
    a.init(m_a);
    ZZ phi = rep(a*sum);

    ZZ_p::init(m_field);
    ZZ_p prod(1);
    for (auto &partialCipher: partialCiphers) {
        uint32_t partyId = partialCipher.first;
        prod = prod * powerZZ(partialCipher.second, 2*m_lagrangeBasis[partyId]);
    }

    ZZ LResult = L_function(rep(prod));
    ZZ_p::init(m_n);

    ZZ_p result;
    result.init(LResult);
    ZZ_p divisor;
    divisor.init(4*m_delta*m_delta*phi);
    return rep(result/divisor);
}

uint32_t PaillierParty::factorial(uint32_t n) {
    uint32_t ret = 1;
    for(uint32_t i = 1; i <= n; ++i)
        ret *= i;
    return ret;
}

ZZ_p PaillierParty::powerZZ(const ZZ &base, const ZZ &exponent) {
    ZZ_p baseP;
    baseP.init(base);
    return power(baseP, exponent);
}

ZZ PaillierParty::L_function(const ZZ& value) {
    return (value-1)/m_n;
}