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
        //m_pTag = GenGermainPrime_ZZ(bitSize/2);
        //m_qTag = GenGermainPrime_ZZ(bitSize/2);
        m_pTag = ZZ(3);
        m_qTag = ZZ(2);

        m_p = 2 * m_pTag + 1;
        m_q = 2 * m_qTag + 1;
        m_n = m_p * m_q;
        m_m = m_pTag * m_qTag;

        ZZ_p::init(m_n);
        //m_a = rep(getRandomZp(m_p,m_q));
        //m_b = rep(getRandomZp(m_p,m_q));
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

    PRINT_PARTY(m_partyId) << "pTag is " << m_pTag << std::endl;
    PRINT_PARTY(m_partyId) << "qTag is " << m_pTag << std::endl;
    PRINT_PARTY(m_partyId) << "p is " << m_p << std::endl;
    PRINT_PARTY(m_partyId) << "q is " << m_q << std::endl;
    PRINT_PARTY(m_partyId) << "n is " << m_n << std::endl;
    PRINT_PARTY(m_partyId) << "m is " << m_m << std::endl;
    PRINT_PARTY(m_partyId) << "a is " << m_a << std::endl;
    PRINT_PARTY(m_partyId) << "b is " << m_b << std::endl;
    PRINT_PARTY(m_partyId) << "field is " << m_field << std::endl;
    PRINT_PARTY(m_partyId) << "delta is " << m_delta << std::endl;

    ZZ_p::init(m_field);

    m_g = rep(powerZZ(1+m_n, m_a)*powerZZ(m_b,m_n));

    PRINT_PARTY(m_partyId) << "g is " << m_g << std::endl;

    for (uint32_t j = 1; j <= m_parties.size()+1; j++) {
        PRINT_PARTY(m_partyId) << "j is " << j << std::endl;
        double prod = m_delta;
        PRINT_PARTY(m_partyId) << "prod is " << prod << std::endl;
        PRINT_PARTY(m_partyId) << "j is " << j << std::endl;
        for (uint32_t jTag = 1; jTag <= m_parties.size()+1; jTag++) {
            if (j == jTag) {
                continue;
            }
            PRINT_PARTY(m_partyId) << "jTag is " << jTag << std::endl;
            prod = prod*jTag;
            PRINT_PARTY(m_partyId) << "inter prod is " << prod << std::endl;
            int divisor = jTag-j;
            PRINT_PARTY(m_partyId) << "divisor is " << divisor << std::endl;
            prod = prod/divisor;
            PRINT_PARTY(m_partyId) << "After prod is " << prod << std::endl;
        }
        m_lagrangeBasis[j] = ZZ(static_cast<int>(prod));
        PRINT_PARTY(m_partyId) << "lagrange basis " << j << " is " << m_lagrangeBasis[j] << std::endl;
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
    ZZ beta = rep(getRandomZp(m_p,m_q));
    PRINT_PARTY(m_partyId) << "beta is " << beta << std::endl;
    ZZ_p::init(m_n*m_m);
    ZZ_p freeCoefficient = conv<ZZ_p>(beta*m_m);
    PRINT_PARTY(m_partyId) << "freeCoefficient is " << freeCoefficient << std::endl;

    ZZ_pX polynomial;
    SetCoeff(polynomial, 0, freeCoefficient);

    for (uint32_t i=1; i < m_numOfParties; i++) {
        ZZ_p coff = getRandomZp(m_p,m_q,m_pTag,m_qTag);
        SetCoeff(polynomial, i, coff);
        PRINT_PARTY(m_partyId) << "coefficent " << i << " is " << coff << std::endl;
    }

    PRINT_PARTY(m_partyId) << "The polynomial is " << polynomial << std::endl;

    for (auto &party : m_parties) {
        ZZ result = rep(eval(polynomial,ZZ_p(party.first)));
        PRINT_PARTY(m_partyId) << "Result on eval on " << party.first << " is " << result << std::endl;
        sendZZTo(result,party.second);
    }

    ZZ_p s_i = eval(polynomial,ZZ_p(m_partyId));
    PRINT_PARTY(m_partyId) << "s_i starts with " << s_i << std::endl;
    for (auto &party : m_parties) {
        ZZ value;
        receiveZZFrom(value,party.second);
        ZZ_p coefficient = conv<ZZ_p>(value);
        PRINT_PARTY(m_partyId) << "add from party " << party.first << " the value " << coefficient << std::endl;
        s_i = s_i + coefficient;
    }

    m_share = rep(s_i);
    PRINT_PARTY(m_partyId) << "the share is " << m_share << std::endl;

    ZZ_p::init(m_n);
    ZZ_p pubKey = conv<ZZ_p>(m_a*beta*m_m);
    PRINT_PARTY(m_partyId) << "The public key is " << pubKey << std::endl;
    m_pubKey = rep(pubKey);
}

ZZ PaillierParty::encrypt(const ZZ& plain) {
    ZZ_p::init(m_n);
    ZZ x = rep(getRandomZp(m_p,m_q));
    ZZ_p::init(m_field);

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
        sum = sum + conv<ZZ_p>(pubKey);
    }

    ZZ phi = rep(conv<ZZ_p>(m_a)*sum);

    ZZ_p::init(m_field);
    ZZ_p prod(1);
    for (auto &partialCipher: partialCiphers) {
        uint32_t partyId = partialCipher.first;
        PRINT_PARTY(m_partyId) << "lagrange basis for party " << partyId << " is " << 2*m_lagrangeBasis[partyId] << std::endl;
        prod = prod * powerZZ(partialCipher.second, 2*m_lagrangeBasis[partyId]);
    }

    ZZ LResult = L_function(rep(prod));

    PRINT_PARTY(m_partyId) << "result is " << LResult << std::endl;
    ZZ_p::init(m_n);

    ZZ_p divisor = conv<ZZ_p>(4*m_delta*m_delta*phi);
    return rep(conv<ZZ_p>(LResult)/divisor);
}

uint32_t PaillierParty::factorial(uint32_t n) {
    uint32_t ret = 1;
    for(uint32_t i = 1; i <= n; ++i)
        ret *= i;
    return ret;
}

ZZ_p PaillierParty::powerZZ(const ZZ &base, const ZZ &exponent) {
    ZZ_p baseP = conv<ZZ_p>(base);
    return power(baseP, exponent);
}

ZZ PaillierParty::L_function(const ZZ& value) {
    if (((value-1)%m_n)!=0) {
        PRINT_PARTY(m_partyId) << "Error value " << value << "is not correct !!!!!!!!!!!!!!!!!!!!!" << std::endl;
    }
    return (value-1)/m_n;
}

ZZ_p PaillierParty::getRandomZp(const ZZ& divisor1) {
    ZZ_p value;
    do {
        value = random_ZZ_p();
    } while ((0 !=(conv<ZZ>(value)%divisor1)));

    return value;
}

ZZ_p PaillierParty::getRandomZp(const ZZ& divisor1, const ZZ& divisor2) {
    ZZ_p value;
    ZZ mod1, mod2;
    do {
        value = random_ZZ_p();
        mod1 = conv<ZZ>(value)%divisor1;
        mod2 = conv<ZZ>(value)%divisor2;
    } while ((mod1 == ZZ(0)) || (mod2 == ZZ(0)));

    return value;
}

ZZ_p PaillierParty::getRandomZp(const ZZ& divisor1, const ZZ& divisor2,const ZZ& divisor3,const ZZ& divisor4) {
    ZZ_p value;
    ZZ mod1, mod2, mod3, mod4;
    do {
        value = random_ZZ_p();
        mod1 = conv<ZZ>(value)%divisor1;
        mod2 = conv<ZZ>(value)%divisor2;
        mod3 = conv<ZZ>(value)%divisor3;
        mod4 = conv<ZZ>(value)%divisor4;
    } while ((mod1 == ZZ(0)) || (mod2 == ZZ(0))
             || (mod3 == ZZ(0)) || (mod4 == ZZ(0)));

    return value;
}