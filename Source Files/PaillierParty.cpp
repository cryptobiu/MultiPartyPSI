//
// Created by root on 8/1/16.
//

#include "PaillierParty.h"
#include <NTL/vec_ZZ_p.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <vector>

PaillierParty::PaillierParty(uint32_t partyId, ConfigFile &config, boost::asio::io_service &ioService) : BaseMPSIParty(partyId,config,ioService) {

}

void PaillierParty::initPaiilier() {
    uint32_t symSecurityParameter = stoi(getValFromConfig(m_config, "General", "symsecurityparameter"));
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

    uint32_t leaderId = stoi(getValFromConfig(m_config, "General", "leaderid"));

    if (m_partyId == leaderId) {
        m_pTag = GenGermainPrime_ZZ(bitSize/2-1);
        m_qTag = GenGermainPrime_ZZ(bitSize/2-1);

        m_p = 2 * m_pTag + 1;
        m_q = 2 * m_qTag + 1;
        m_n = m_p * m_q;
        m_m = m_pTag * m_qTag;

        // TODO: there is some bug here when I use different m_a,m_b
        //m_a = getRandomInNStar(m_n);
        //m_b = getRandomInNStar(m_n);
        m_a = 1;
        m_b = 1;

        for (auto &party : m_parties) {
            sendZZTo(m_pTag,party.second);
            sendZZTo(m_qTag,party.second);
            sendZZTo(m_a,party.second);
            sendZZTo(m_b,party.second);
        }
    }
    else {
        receiveZZFrom(m_pTag, m_parties[leaderId]);
        receiveZZFrom(m_qTag, m_parties[leaderId]);
        receiveZZFrom(m_a, m_parties[leaderId]);
        receiveZZFrom(m_b, m_parties[leaderId]);

        m_p = 2 * m_pTag + 1;
        m_q = 2 * m_qTag + 1;
        m_n = m_p * m_q;
        m_m = m_pTag * m_qTag;
    }

    m_field = m_n * m_n;
    m_delta = factorial(m_numOfParties);
    m_g = MulMod(PowerMod(1+m_n, m_a,m_field),PowerMod(m_b,m_n,m_field),m_field);

    for (uint32_t j = 1; j <= m_parties.size()+1; j++) {
        double prod = m_delta;
        for (uint32_t jTag = 1; jTag <= m_parties.size()+1; jTag++) {
            if (j == jTag) {
                continue;
            }
            prod = prod*jTag;
            int divisor = jTag-j;
            prod = prod/divisor;
        }
        m_lagrangeBasis[j] = ZZ(static_cast<int>(prod));
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
    ZZ beta = getRandomInNStar(m_n);

    std::vector<ZZ> coefficients;

    coefficients.push_back(MulMod(beta,m_m,m_n*m_m));

    for (uint32_t i=1; i < m_numOfParties; i++) {
        coefficients.push_back(getRandomInNStar(m_n*m_m));
    }

    ZZ_p::init(m_n*m_m);
    ZZ_pX polynomial;
    for (uint32_t i=0; i < m_numOfParties; i++) {
        SetCoeff(polynomial, i, conv<ZZ_p>(coefficients[i]));
    }

    for (auto &party : m_parties) {
        ZZ result = rep(eval(polynomial,ZZ_p(party.first)));
        sendZZTo(result,party.second);
    }

    ZZ_p s_i = eval(polynomial,ZZ_p(m_partyId));
    for (auto &party : m_parties) {
        ZZ value;
        receiveZZFrom(value,party.second);
        ZZ_p coefficient = conv<ZZ_p>(value);
        s_i = s_i + coefficient;
    }

    m_share = rep(s_i);

    m_pubKey = MulMod(MulMod(m_a,beta,m_n),m_m,m_n);
}

ZZ PaillierParty::encrypt(const ZZ& plain) {
    ZZ x = getRandomInNStar(m_n);

    return MulMod(PowerMod(m_g, plain,m_field),PowerMod(x,m_n,m_field),m_field);
}

ZZ PaillierParty::partialDecrypt(const ZZ& cipher) {
    return PowerMod(cipher, 2*m_delta*m_share,m_field);
}

ZZ PaillierParty::decrypt(const std::map<uint32_t,ZZ> &partialCiphers,
                          const std::vector<ZZ> &pubKeys) {
    ZZ sum(0);
    for (auto &pubKey : pubKeys) {
        sum = AddMod(sum,pubKey,m_n);
    }

    ZZ phi = MulMod(m_a,sum,m_n);

    ZZ prod(1);
    for (auto &partialCipher: partialCiphers) {
        uint32_t partyId = partialCipher.first;
        prod = MulMod(prod,PowerMod(partialCipher.second, 2*m_lagrangeBasis[partyId],m_field),m_field);
    }

    ZZ LResult = L_function(prod);

    ZZ combinedShare = InvMod(MulMod(4*m_delta*m_delta,phi,m_n),m_n);
    return MulMod(LResult, combinedShare, m_n);
}

uint32_t PaillierParty::factorial(uint32_t n) {
    uint32_t ret = 1;
    for(uint32_t i = 1; i <= n; ++i)
        ret *= i;
    return ret;
}


ZZ PaillierParty::L_function(const ZZ& value) {
    if (((value-1)%m_n)!=0) {
        PRINT_PARTY(m_partyId) << "Error value " << value << "is not correct !!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }
    return (value-1)/m_n;
}

ZZ PaillierParty::getRandomInNStar(const ZZ& mod) {
    ZZ value;
    ZZ_p::init(mod);
    do {
        value = conv<ZZ>(random_ZZ_p());
    } while (GCD(value,mod)!=1);

    return value;
}