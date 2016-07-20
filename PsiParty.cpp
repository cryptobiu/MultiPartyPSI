//
// Created by root on 6/20/16.
//


#include <boost/thread/thread.hpp>
#include "../include/primitives/AES_PRG.hpp"
#include "../include/primitives/Prg.hpp"
//#include "PRG/PRG.hpp"
#include "common/defs.h"
#include <immintrin.h>
#include "PSI/src/ot-based/ot-psi.h"
#include "PSI/src/hashing/hashing_util.h"
#include "PsiParty.h"
#include <iomanip>

#define KEY_SIZE 16

PsiParty::PsiParty(uint partyId, ConfigFile &config, boost::asio::io_service &ioService) :
    MultiPartyPlayer(partyId, config, ioService)
{

    uint64_t rnd;
    uint32_t symsecbits=128;
    uint8_t* seed = (uint8_t*) malloc(AES_BYTES);

    memcpy(seed, const_seed, AES_BYTES);
    seed[0] = static_cast<uint8_t>(m_partyId);
    m_crypt = new crypto(symsecbits, seed);

    m_crypt->gen_rnd((uint8_t*) &rnd, sizeof(uint64_t));
    srand((unsigned)rnd+time(0));



    m_setSize = stoi(m_config.Value("General", "setSize"));


    m_numOfBins = ceil(EPSILON * m_setSize);

    m_elementSizeInBits = stoi(m_config.Value("General", "elementSizeInBits"));
    m_blockSizeInBits = stoi(m_config.Value("General", "blockSizeInBits"));

    m_elements = new uint8_t[m_setSize*sizeof(uint32_t)];

    m_serverSocket.Receive(reinterpret_cast<byte *>(&m_strategy), 1);


    PRINT_PARTY(m_partyId) << "is executing strategy " << (m_strategy == Strategy::NAIVE_METHOD_SMALL_N) << std::endl;

    PRINT_PARTY(m_partyId) << "is receiving elements" << std::endl;

    m_serverSocket.Receive(reinterpret_cast<byte *>(m_elements), m_setSize*sizeof(uint32_t));

    m_statistics.partyId = partyId;

    m_maskbitlen = pad_to_multiple(m_crypt->get_seclvl().statbits + (m_numOfParties-1)*ceil_log2(m_setSize), 8);

    PRINT_PARTY(m_partyId) << "Mask size in bytes is " << getMaskSizeInBytes() << std::endl;

    syncronize();
}

void PsiParty::syncronize() {
    char c;
    m_serverSocket.Send(reinterpret_cast<const byte*>("1"),1);
    m_serverSocket.Receive(reinterpret_cast<byte*>(&c),1);
}

void PsiParty::run() {
    m_statistics.beginTime = clock();

    PRINT_PARTY(m_partyId) << "additive secret sharing is runnning" << std::endl;

    additiveSecretShare();

    PRINT_PARTY(m_partyId) << "additive secret sharing completed" << std::endl;

    m_statistics.afterSharing = clock();

    uint leaderId = stoi(m_config.Value("General", "leaderId"));

    if (m_partyId == leaderId) {
        PRINT_PARTY(m_partyId) << "run as leader" << std::endl;
        runAsLeader();
    }
    else {
        PRINT_PARTY(m_partyId) << "run as follower"  << std::endl;
        runAsFollower(m_parties[leaderId]);
    }

    finishAndReportStatsToServer();
}

void PsiParty::printHex(const uint8_t *arr, uint32_t size) {
    for(uint32_t k = 0; k < size; k++) {
        std::cout << setw(2) << setfill('0') << (hex) << (unsigned int) arr[k] << (dec);
    }
}

void PsiParty::printShares(const uint8_t *arr, uint32_t numOfShares) {
    for (uint32_t i = 0; i < numOfShares; i++) {
        printHex(arr+i*getMaskSizeInBytes(), getMaskSizeInBytes());
        std::cout << " ";
    }
    std::cout << std::endl;
}

void PsiParty::runLeaderAgainstFollower(std::pair<uint32_t, CSocket*> party, uint8_t **partyResult, uint8_t **leaderResults, uint32_t **bin_ids, uint32_t **perm) {
    PRINT_PARTY(m_partyId) << "run leader against party " << party.first << std::endl;

    otpsi(LEADER, m_setSize, m_setSize, sizeof(uint32_t),
            m_elements, partyResult, leaderResults, m_crypt,party.second, 1, m_maskbitlen, m_secretShare, bin_ids, perm);

    PRINT_PARTY(m_partyId) << "otpsi was successful" << std::endl;

    PRINT_PARTY(m_partyId) << "party " << party.first << " results: ";
    printShares(*partyResult, NUM_HASH_FUNCTIONS * m_setSize);

    PRINT_PARTY(m_partyId) << "leader results: ";
    printShares(*leaderResults, m_setSize);

    PRINT_PARTY(m_partyId) << "done running leader against party " << party.first << std::endl;
}

void PsiParty::finishAndReportStatsToServer() {
    PRINT_PARTY(m_partyId) << "is reporting stats to server" << std::endl;
    m_serverSocket.Send(reinterpret_cast<byte *>(&m_statistics), sizeof(struct statistics));
}

void PsiParty::runAsLeader() {

    uint8_t **partiesResults;
    uint8_t **leaderResults;

    partiesResults = new uint8_t*[m_numOfParties];
    leaderResults = new uint8_t*[m_numOfParties];

    /*
    boost::thread_group threadpool;

    for (auto &party : m_parties) {
        threadpool.create_thread(boost::bind(&PsiParty::runLeaderAgainstFollower, this,
                                             party, &partiesResults[party.first-1]));
    }
    threadpool.join_all();
    */

    // it would written over each time but every run gives the same thing
    uint32_t *bin_ids;
    uint32_t *perm;


    for (auto &party : m_parties) {
        runLeaderAgainstFollower(party, &partiesResults[party.first - 1], &leaderResults[party.first - 1], &bin_ids, &perm);
    }

    PRINT_PARTY(m_partyId) << "otpsi was successful" << std::endl;

    m_statistics.afterOTs = clock();


    vector<uint32_t> intersection;
    for (uint32_t i = 0; i < m_setSize; i = i + 4) {
        if (isElementInAllSets(i, partiesResults, leaderResults, bin_ids, perm)) {
            std::cout << "Input " << *(uint32_t*)(&m_elements[i]) << " is in the intersection" << std::endl;
            intersection.push_back(*(uint32_t*)(&m_elements[i]));
        }
    }

    PRINT_PARTY(m_partyId) << "found that intersection size is " << intersection.size() << std::endl;
    m_statistics.intersectionSize = intersection.size();

    m_statistics.specificStats.aftetComputing = clock();
}

void PsiParty::XOR(byte *xoree1, byte *xoree2, uint32_t size) {
    for (uint32_t i = 0; i < size; i++) {
        xoree1[i] = xoree1[i] ^ xoree2[i];
    }
};

bool PsiParty::isZeroXOR(byte *formerShare, uint32_t partyNum, uint8_t **partiesResults) {
    if (partyNum < m_numOfParties) {
        uint8_t *partyResult = partiesResults[partyNum];
        for (uint32_t i = 0; i < m_setSize *NUM_HASH_FUNCTIONS; i++) {
            XOR(formerShare,partyResult+i*getMaskSizeInBytes(), getMaskSizeInBytes());
            if (isZeroXOR(formerShare,partyNum+1, partiesResults)) {
                return true;
            }
            XOR(formerShare,partyResult+i*getMaskSizeInBytes(), getMaskSizeInBytes());
        }
        return false;
    }

    for (uint32_t i = 0; i < getMaskSizeInBytes(); i++) {
        if (formerShare[i] != 0) {
            return false;
        }
    }
    return true;
};

bool PsiParty::isElementInAllSets(uint32_t index, uint8_t **partiesResults, uint8_t **leaderResults, uint32_t *bin_ids, uint32_t *perm) {

    uint32_t binIndex = 0;
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        if (bin_ids[i] == index + 1) {
            std::cout << "Element number " << index << " was found at " << i << std::endl;
            binIndex = i;
            break;
        }
    }

    uint32_t newIndex = 0;
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        if (perm[i] == index) {
            newIndex = i;
            break;
        }
    }

    byte* secret = &m_secretShare[binIndex*getMaskSizeInBytes()];

    for (auto &party : m_parties) {
        XOR(secret, leaderResults[party.first-1]+newIndex*getMaskSizeInBytes(), getMaskSizeInBytes());
    }

    // 1 is always the leader Id
    return isZeroXOR(secret,1, partiesResults);
}

void PsiParty::runAsFollower(CSocket *leader) {
    otpsi(FOLLOWER, m_setSize, m_setSize, sizeof(uint32_t),
          m_elements, NULL, NULL, m_crypt, leader, 1, m_maskbitlen, m_secretShare, NULL, NULL);
    PRINT_PARTY(m_partyId) << "otpsi was successful" << std::endl;
    m_statistics.afterOTs = clock();
    m_statistics.specificStats.afterSend = clock();
}

void PsiParty::additiveSecretShare() {
    /*
    if (getElementSize() % SIZE_OF_BLOCK != 0) {
        throw std::system_error();
    }

    uint numOfBlocks = 2 * m_elementSizeInBits / m_blockSizeInBits;

    uint elementSize = SIZE_OF_BLOCK * numOfBlocks;
    */
    uint32_t shareSize = getMaskSizeInBytes() * m_numOfBins;
    vector<boost::shared_ptr<byte>> shares;

    for (uint i = m_partyId+1; i <= m_numOfParties; i++) {
        byte *share = new byte[KEY_SIZE];
        RAND_bytes(share, KEY_SIZE);
        m_parties[i]->Send(share, KEY_SIZE);
        shares.push_back(boost::shared_ptr<byte>(share));
        //printHex(share,KEY_SIZE);
        //std::cout << std::endl;
    }

    for (uint i = 1; i <= m_partyId-1; i++) {
        byte *share = new byte[KEY_SIZE];
        m_parties[i]->Receive(share, KEY_SIZE);
        shares.push_back(boost::shared_ptr<byte>(share));
        //printHex(share,KEY_SIZE);
        //std::cout << std::endl;
    }

    m_secretShare = new byte[shareSize];
    memset(m_secretShare, 0, shareSize);

    for (auto &share : shares) {
        /*
        std::cout << "key: ";
        printHex(share.get(),KEY_SIZE);
        std::cout << std::endl;

        std::shared_ptr<vector<byte>> key;
        key.reset(new vector<byte>(share.get(), share.get()+KEY_SIZE));
        AES_PRG prg(key, shareSize);
        byte *prgResult = prg.getRandomBytes();
        std::cout << "PRG result: ";
        printHex(prgResult,shareSize);
        std::cout << std::endl;
        */


        SecretKey key(share.get(), KEY_SIZE, "RC4");

        OpenSSLRC4 prg;
        prg.setKey(key);
        vector<byte> result;
        prg.getPRGBytes(result, 0,shareSize);

        //string res(result.begin(),result.end());
        //PRINT_PARTY(m_partyId);
        //printShares(reinterpret_cast<const uint8_t*>(res.data()), 2);

        XOR(m_secretShare, result.data(), shareSize);

        /*
        for (uint j = 0; j < shareSize; j += SIZE_OF_BLOCK) {
            __m128i v = _mm_load_si128((__m128i*)(expShare + j));
            _mm_xor_si128(v, *(totalShare + j / SIZE_OF_BLOCK));
        }
        */
    }

    PRINT_PARTY(m_partyId) << "my secret shares are: ";
    printShares(m_secretShare, m_numOfBins);
}