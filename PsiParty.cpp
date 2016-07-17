//
// Created by root on 6/20/16.
//

#include "PsiParty.h"
#include <boost/thread/thread.hpp>
#include "PRG/PRG.hpp"
#include "common/defs.h"
#include <immintrin.h>
#include "PSI/src/util/typedefs.h"
#include "PSI/src/util/crypto/crypto.h"
#include "PSI/src/ot-based/ot-psi.h"

#define SIZE_OF_BLOCK 16

PsiParty::PsiParty(uint partyId, ConfigFile config, boost::asio::io_service &ioService) :
    MultiPartyPlayer(partyId, config, ioService)
{
    m_setSize = stoi(m_config.Value("General", "setSize"));
    m_elementSizeInBits = stoi(m_config.Value("General", "elementSizeInBits"));
    m_blockSizeInBits = stoi(m_config.Value("General", "blockSizeInBits"));

    m_elements = new uint8_t[m_setSize*sizeof(uint32_t)];
    m_serverSocket.Receive(reinterpret_cast<byte *>(m_elements), m_setSize*sizeof(uint32_t));

    m_statistics.partyId = partyId;

    syncronize();
}

void PsiParty::syncronize() {
    char c;
    m_serverSocket.Send(reinterpret_cast<const byte*>("1"),1);
    m_serverSocket.Receive(reinterpret_cast<byte*>(&c),1);
}

void PsiParty::run() {
    m_statistics.beginTime = clock();

    //std::cout << "additive secret share" << std::endl;
    //additiveSecretShare();

    m_statistics.afterSharing = clock();

    std::cout << m_statistics.afterSharing << std::endl;

    uint leaderId = stoi(m_config.Value("General", "leaderId"));

    if (m_partyId = leaderId) {
        std::cout << "Run as leader" << std::endl;
        runAsLeader();
    }
    else {
        std::cout << "Run as follower" << std::endl;
        runAsFollower(m_otherParties[leaderId]);
    }

    finishAndReportStatsToServer();
}


void PsiParty::runLeaderAgainstFollower(const boost::shared_ptr<CommPartyTCPSynced> &leader) {

}

void PsiParty::finishAndReportStatsToServer() {
    std::cout << "send statistics to server" << std::endl;
    m_serverSocket.Send(reinterpret_cast<byte *>(&m_statistics), sizeof(struct statistics));
}

void PsiParty::runAsLeader() {

    uint8_t *results;
    uint64_t rnd;
    uint32_t symsecbits=128;
    uint8_t* seed = (uint8_t*) malloc(AES_BYTES);

    memcpy(seed, const_seed, AES_BYTES);
    seed[0] = SERVER;
    crypto* crypt = new crypto(symsecbits, seed);


    crypt->gen_rnd((uint8_t*) &rnd, sizeof(uint64_t));
    srand((unsigned)rnd+time(0));



    //boost::thread_group threadpool;

    for (auto &party : m_parties) {

        otpsi(SERVER, m_setSize, m_setSize, sizeof(uint32_t), m_elements, &results, crypt,party.second,1);
        //threadpool.create_thread(boost::bind(&PsiParty::runLeaderAgainstFollower, this, party.second));
    }
    //threadpool.join_all();

    m_statistics.afterOTs = clock();

    vector<uint> intersection;
    /*
    for (auto &element : m_elements) {
        if (isElementInAllSets(element)) {
            intersection.push_back(element);
        }
    }
    */
    m_statistics.specificStats.aftetComputing = clock();
}

bool PsiParty::isElementInAllSets(uint element) {
    return true;
}

void PsiParty::runAsFollower(const boost::shared_ptr<CommPartyTCPSynced> &leader) {
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

    uint shareSize = SIZE_OF_BLOCK * m_setSize;
    vector<boost::shared_ptr<block>> shares;

    std::cout << m_partyId << " here1" << std::endl;

    for (uint i = m_partyId+1; i <= m_otherParties.size()-1; i++) {
        block *share = (block *)_mm_malloc(elementSize, SIZE_OF_BLOCK);
        RAND_bytes(reinterpret_cast<unsigned char *>(share), elementSize);
        m_otherParties[i]->write(reinterpret_cast<const byte *>(share), elementSize);
        shares.push_back(boost::shared_ptr<block>(share, _mm_free));
    }

    std::cout << m_partyId << " here2" << std::endl;

    for (uint i = 1; i <= m_partyId-1; i++) {
        block *share = (block *)_mm_malloc(elementSize, SIZE_OF_BLOCK);
        std::cout << i << std::endl;
        m_otherParties[i]->read(reinterpret_cast<byte *>(share), elementSize);
        shares.push_back(boost::shared_ptr<block>(share, _mm_free));
    }

    std::cout << m_partyId << " here3" << std::endl;

    for (uint i = 0; i < numOfBlocks; i++) {
        block *totalShare = (block *)_mm_malloc(shareSize, SIZE_OF_BLOCK);
        for (auto &share : shares) {
            PRG prg(reinterpret_cast<byte*>(share.get())+i*SIZE_OF_BLOCK, shareSize);
            byte *expShare = prg.getRandomBytes();
            for (uint j = 0; j < shareSize; j += SIZE_OF_BLOCK) {
                __m128i v = _mm_load_si128((__m128i*)(expShare + j));
                _mm_xor_si128(v, *(totalShare + j / SIZE_OF_BLOCK));
            }
        }

        m_secretShares.push_back(boost::shared_ptr<block>(totalShare, _mm_free));
    }
     */
}