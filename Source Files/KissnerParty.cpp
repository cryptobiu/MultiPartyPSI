//
// Created by root on 8/1/16.
//

#include <NTL/ZZ_pE.h>
#include "KissnerParty.h"

KissnerParty::KissnerParty(uint32_t partyId, ConfigFile &config, boost::asio::io_service &ioService) :
        PaillierParty(partyId,config,ioService){

}

void KissnerParty::run() {

    initPaiilier();

    vector<ZZ> encCoefficients = getEncryptedCoefficients();

    vector<ZZ> sharedPolynomial = makeSharedPolynomial(encCoefficients);

    uint32_t leaderId = stoi(getValFromConfig(m_config, "General", "leaderid"));

    if (m_partyId != leaderId) {
        makeDecryptedPolynomialAsFollower(sharedPolynomial,leaderId);
    }
    else {
        vector<ZZ> decryptedPoly = makeDecryptedPolynomialAsLeader(sharedPolynomial);
        ZZ_p::init(m_n);
        ZZ_pX poly;
        for (uint32_t i=0; i <= 2*m_setSize; i++) {
            SetCoeff(poly,i,conv<ZZ_p>(decryptedPoly[i]));
        }
        vec_ZZ_p values = eval(poly,m_roots);
        vec_ZZ_p intersection;

        for (uint32_t i=0; i < m_roots.length(); i++) {
            if (values[i]==0) {
                intersection.append(m_roots[i]);
            }

        }
        PRINT() << "intersection size is " << intersection.length() << std::endl;
    }
}

void KissnerParty::makeDecryptedPolynomialAsFollower(const vector<ZZ>& sharedPoly, uint32_t leaderId) {
    uint32_t prevParty = m_partyId-1;
    if (prevParty==0) {
        prevParty = m_numOfParties;
    }
    uint32_t nextParty = m_partyId%m_numOfParties+1;

    vector<ZZ> prevPoly;
    for (uint32_t i=0; i<=2*m_setSize; i++) {
        ZZ coeff;
        receiveZZFrom(coeff,m_parties[prevParty]);
        prevPoly.push_back(coeff);
    }

    vector<ZZ> nextPoly = addEncPolys(prevPoly,sharedPoly);

    for (auto &coeff : nextPoly) {
        sendZZTo(coeff,m_parties[nextParty]);
    }

    sendZZTo(getPubKey(),m_parties[leaderId]);

    for (uint32_t i=0; i<=2*m_setSize; i++) {
        ZZ cipher;
        receiveZZFrom(cipher,m_parties[leaderId]);
        ZZ dec = partialDecrypt(cipher);
        sendZZTo(dec,m_parties[leaderId]);
    }
}

vector<ZZ> KissnerParty::makeDecryptedPolynomialAsLeader(const vector<ZZ>& sharedPoly) {

    uint32_t prevParty = m_partyId-1;
    if (prevParty==0) {
        prevParty = m_numOfParties;
    }
    uint32_t nextParty = m_partyId%m_numOfParties+1;

    for (auto &coeff : sharedPoly) {
        sendZZTo(coeff,m_parties[nextParty]);
    }

    vector<ZZ> finalPoly;
    for (uint32_t i=0; i<=2*m_setSize; i++) {
        ZZ coeff;
        receiveZZFrom(coeff,m_parties[prevParty]);
        finalPoly.push_back(coeff);
    }

    vector<ZZ> pubKeys;
    pubKeys.push_back(getPubKey());
    for (auto &party: m_parties) {
        ZZ pubKey;
        receiveZZFrom(pubKey,party.second);
        pubKeys.push_back(pubKey);
    }


    vector<ZZ> decryptedPoly;
    for (auto &cipher : finalPoly) {
        for (auto &party: m_parties) {
            sendZZTo(cipher,party.second);
        }

        map<uint32_t,ZZ> decs;
        decs[m_partyId]=partialDecrypt(cipher);

        for (auto &party: m_parties) {
            ZZ dec;
            receiveZZFrom(dec,party.second);
            decs[party.first]=dec;
        }

        decryptedPoly.push_back(decrypt(decs,pubKeys));
    }

    return decryptedPoly;
}

vector<ZZ> KissnerParty::getEncryptedCoefficients() {
    ZZ_p::init(m_n);
    uint32_t elementSizeInBytes = ceil_divide(m_elementSizeInBits, 8);
    for (uint32_t i=0; i < m_setSize; i++) {
        m_roots.append(conv<ZZ_p>(ZZFromBytes(m_elements.get()+i*elementSizeInBytes,elementSizeInBytes)));
    }
    ZZ_pX polynomial = BuildFromRoots(m_roots);

    vector<ZZ> coefficients;
    for (uint32_t i = 0; i <= m_setSize; i++) {
        ZZ_p coeff;
        GetCoeff(coeff, polynomial, i);
        coefficients.push_back(conv<ZZ>(coeff));
    }

    vector<ZZ> encCoefficients;

    for (auto &coeff : coefficients) {
        encCoefficients.push_back(encrypt(coeff));
    }

    return encCoefficients;
}

vector<ZZ> KissnerParty::addEncPolys(const vector<ZZ> poly1, const vector<ZZ> poly2) {
    vector<ZZ> coefficients;
    for (uint32_t i = 0; i < poly1.size(); i++) {
        coefficients.push_back(MulMod(poly1[i],poly2[i],m_field));
    }
    return coefficients;
}

vector<ZZ> KissnerParty::mulEncPolyByPoly(const vector<ZZ> encPoly, const vector<ZZ> poly) {
    vector<ZZ> coefficients;
    for (uint32_t i = 0; i <= 2*m_setSize; i++) {
        ZZ prod(1);
        for (uint32_t j=0; j<=i; j++) {
            if ((j<=m_setSize) && ((i-j)<=m_setSize)) {
                prod=MulMod(prod,PowerMod(encPoly[j],poly[i-j],m_field),m_field);
            }
        }
        coefficients.push_back(prod);
    }
    return coefficients;
}

vector<ZZ> KissnerParty::makeSharedPolynomial(const vector<ZZ>& encPoly) {

    for (uint32_t i = m_partyId+1; i <= m_numOfParties; i++) {
        for (auto &encCoeff : encPoly) {
            sendZZTo(encCoeff,m_parties[i]);
        }
    }

    boost::shared_ptr<vector<ZZ>[]> receivedPolys(new vector<ZZ>[m_partyId]);
    for (uint32_t i = 1; i <= m_partyId-1; i++) {
        for (uint32_t j=0; j<=m_setSize; j++) {
            ZZ encCoeff;
            receiveZZFrom(encCoeff,m_parties[i]);
            (receivedPolys.get()+i-1)->push_back(encCoeff);
        }
    }

    *(receivedPolys.get()+m_partyId-1)=encPoly;

    vector<ZZ> sumPoly;

    for (uint32_t i = 0; i < m_partyId; i++) {
        vector<ZZ> randomPoly;
        for (uint32_t j=0; j <= m_setSize; j++) {
            randomPoly.push_back(getRandomInNStar(m_field));
        }
        vector<ZZ> prodPoly = mulEncPolyByPoly(*(receivedPolys.get()+i),randomPoly);

        if (sumPoly.empty()) {
            sumPoly = prodPoly;
        }
        else {
            sumPoly = addEncPolys(sumPoly,prodPoly);
        }
    }

    return sumPoly;
}