//
// Created by root on 6/20/16.
//

#include "PsiParty.h"
#include <boost/thread/thread.hpp>
#include "PRG.hpp"
#include "common/defs.h"
#include <immintrin.h>

#define SIZE_OF_BLOCK 16

PsiParty::PsiParty(uint partyId, ConfigFile config, boost::asio::io_service &ioService) :
    MultiPartyPlayer(partyId, config, ioService)
{
    m_setSize = stoi(m_config.Value("General", "setSize"));
    m_elementSizeInBits = stoi(m_config.Value("General", "elementSizeInBits"));
    m_blockSizeInBits = stoi(m_config.Value("General", "blockSizeInBits"));


    uint *elementsBytes = new uint[m_setSize];
    m_serverProxy->read(reinterpret_cast<byte *>(elementsBytes), m_setSize*sizeof(uint));

    m_elements.insert(m_elements.end(), &elementsBytes[0], &elementsBytes[m_setSize]);
    m_statistics.partyId = partyId;

    syncronize();
}

void PsiParty::syncronize() {
    char c;
    m_serverProxy->write(reinterpret_cast<const byte*>("1"),1);
    m_serverProxy->read(reinterpret_cast<byte*>(&c),1);
}

void PsiParty::run() {
    m_statistics.beginTime = clock();

    additiveSecretShare();

    m_statistics.afterSharing = clock();
    uint leaderId = stoi(m_config.Value("General", "leaderId"));

    if (m_partyId = leaderId) {
        runAsLeader();
    }
    else {
        runAsFollower(m_otherParties[leaderId]);
    }

    finishAndReportStatsToServer();
}


void PsiParty::runLeaderAgainstFollower(const boost::shared_ptr<CommPartyTCPSynced> &leader) {

}

void PsiParty::finishAndReportStatsToServer() {
    m_serverProxy->write(reinterpret_cast<byte *>(&m_statistics), sizeof(struct statistics));
}

void PsiParty::runAsLeader() {
    boost::thread_group threadpool;

    for (auto &party : m_otherParties) {
        threadpool.create_thread(boost::bind(&PsiParty::runLeaderAgainstFollower, this, party.second));
    }
    threadpool.join_all();

    m_statistics.afterOTs = clock();

    vector<uint> intersection;

    for (auto &element : m_elements) {
        if (isElementInAllSets(element)) {
            intersection.push_back(element);
        }
    }

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
    if (!(getElementSize() % SIZE_OF_BLOCK)) {
        throw std::system_error();
    }

    uint numOfBlocks = 2 * m_elementSizeInBits / m_blockSizeInBits;

    uint elementSize = SIZE_OF_BLOCK * numOfBlocks;

    uint shareSize = SIZE_OF_BLOCK * m_setSize;
    vector<boost::shared_ptr<block>> shares;

    for (uint i = m_partyId; i <= m_otherParties.size()-1; i++) {
        block *share = (block *)_mm_malloc(elementSize, SIZE_OF_BLOCK);
        RAND_bytes(reinterpret_cast<unsigned char *>(share), elementSize);
        m_otherParties[i]->write(reinterpret_cast<const byte *>(share), elementSize);
        shares.push_back(boost::shared_ptr<block>(share, _mm_free));
    }

    for (uint i = 0; i <= m_partyId-1; i++) {
        block *share = (block *)_mm_malloc(elementSize, SIZE_OF_BLOCK);
        m_otherParties[i]->read(reinterpret_cast<byte *>(share), elementSize);
        shares.push_back(boost::shared_ptr<block>(share, _mm_free));
    }

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
}