//
// Created by root on 6/20/16.
//


#include <boost/thread/thread.hpp>
#include "../../include/primitives/AES_PRG.hpp"
#include "../../include/primitives/Prg.hpp"
//#include "PRG/PRG.hpp"
#include "../Header Files/defs.h"
#include <immintrin.h>
#include "../PSI/src/ot-based/ot-psi.h"
#include "../PSI/src/hashing/hashing_util.h"
#include "../Header Files/PsiParty.h"
#include "../Header Files/Follower.h"
#include "../Header Files/NaiveFollower.h"
#include "../Header Files/FollowerFactory.h"
#include "../Header Files/LeaderFactory.h"
#include "../Header Files/NaiveLeader.h"
#include <iomanip>

#define KEY_SIZE 16

PsiParty::PsiParty(uint partyId, ConfigFile &config, boost::asio::io_service &ioService) :
    MultiPartyPlayer(partyId, config, ioService)
{
    LoadConfiguration();

    m_numOfBins = ceil(EPSILON * m_setSize);

    m_elements.reset(new uint8_t[m_setSize*sizeof(uint32_t)]);

    m_serverSocket.Receive(reinterpret_cast<byte *>(&m_strategy), 1);

    m_elementSizeInBits = sizeof(uint32_t) * 8;

    PRINT_PARTY(m_partyId) << "is executing strategy " << (m_strategy == Strategy::NAIVE_METHOD_SMALL_N) << std::endl;

    PRINT_PARTY(m_partyId) << "is receiving elements" << std::endl;

    m_serverSocket.Receive(reinterpret_cast<byte *>(m_elements.get()), m_setSize*sizeof(uint32_t));

    m_statistics.partyId = partyId;

    initializeCrypto();

    m_maskbitlen = pad_to_multiple(m_crypt->get_seclvl().statbits + (m_numOfParties-1)*ceil_log2(m_setSize), 8);

    // PRINT_PARTY(m_partyId) << "Mask size in bytes is " << getMaskSizeInBytes() << std::endl;

    uint32_t elebytelen = ceil_divide(m_elementSizeInBits, 8);

    if(m_elementSizeInBits > m_maskbitlen) {
        //Hash elements into a smaller domain
        m_eleptr.reset(new uint8_t[getMaskSizeInBytes() * m_setSize]);
        domain_hashing(m_setSize, m_elements.get(), elebytelen, m_eleptr.get(), getMaskSizeInBytes(), m_crypt.get());
        m_internal_bitlen = m_maskbitlen;
    } else {
        m_eleptr = m_elements;
        m_internal_bitlen = m_elementSizeInBits;
    }

    syncronize();

}

void PsiParty::initializeCrypto() {

    boost::shared_ptr<uint8_t> seed_buf(new uint8_t[m_seedSize]);
    m_serverSocket.Receive(seed_buf.get(), m_seedSize*sizeof(uint8_t));

    uint64_t rnd;

    boost::shared_ptr<uint8_t> seed(new uint8_t[AES_BYTES]);

    memcpy(seed.get(), const_seed, AES_BYTES);
    (seed.get())[0] = static_cast<uint8_t>(m_partyId);
    m_crypt.reset(new crypto(m_symsecbits, seed.get()));

    m_crypt->gen_rnd((uint8_t*) &rnd, sizeof(uint64_t));
    srand((unsigned)rnd+time(0));

    m_crypt->init_prf_state(&m_prfState, seed_buf.get());
}

void PsiParty::LoadConfiguration() {
    m_setSize = stoi(m_config.Value("General", "setSize"));
    m_elementSizeInBits = stoi(m_config.Value("General", "elementSizeInBits"));

    m_symsecbits=stoi(m_config.Value("General", "securityParameter"));
    m_seedSize=stoi(m_config.Value("General", "seedSizeInBytes"));
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

    //PRINT_PARTY(m_partyId) << "additive secret sharing completed" << std::endl;

    m_statistics.afterSharing = clock();

    uint leaderId = stoi(m_config.Value("General", "leaderId"));

    if (m_partyId == leaderId) {
        PRINT_PARTY(m_partyId) << "run as leader" << std::endl;
        runAsLeader();
    }
    else {
        PRINT_PARTY(m_partyId) << "run as follower"  << std::endl;
        runAsFollower(*m_parties[leaderId]);
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

void PsiParty::runLeaderAgainstFollower(std::pair<uint32_t, CSocket*> party, uint8_t *leaderResults,
                                        const boost::shared_ptr<uint32_t> &nelesinbin, uint32_t outbitlen, const boost::shared_ptr<uint8_t> &hash_table) {

    PRINT_PARTY(m_partyId) << "run leader against party " << party.first << std::endl;

    // uint8_t* seed_buf = (uint8_t*) malloc(m_seedSize);
    // RAND_bytes(seed_buf, m_seedSize);
    //m_crypt->gen_common_seed(&m_prfState, *party.second);

    otpsi_client(m_setSize, m_numOfBins, m_maskbitlen, m_crypt.get(),
                            party.second, 1, leaderResults, outbitlen, nelesinbin.get(), hash_table.get());


    PRINT_PARTY(m_partyId) << "otpsi was successful" << std::endl;

    /*
    PRINT_PARTY(m_partyId) << "party " << party.first << " results: ";
    printShares(*partyResult, NUM_HASH_FUNCTIONS * m_setSize);

    PRINT_PARTY(m_partyId) << "leader results: ";
    printShares(*leaderResults, m_setSize);
    */

    PRINT_PARTY(m_partyId) << "done running leader against party " << party.first << std::endl;
}

void PsiParty::finishAndReportStatsToServer() {
    PRINT_PARTY(m_partyId) << "is reporting stats to server" << std::endl;
    m_serverSocket.Send(reinterpret_cast<byte *>(&m_statistics), sizeof(struct statistics));
}

void PsiParty::runAsLeader() {

    uint8_t **leaderResults = new uint8_t*[m_numOfParties];

    boost::shared_ptr<uint32_t> perm(new uint32_t[m_setSize]);
    memset(perm.get(),0,m_setSize);

    boost::shared_ptr<uint32_t> nelesinbin(new uint32_t[m_numOfBins]);
    memset(nelesinbin.get(),0,m_numOfBins);

    boost::shared_ptr<uint8_t> hash_table;

    boost::shared_ptr<uint32_t> bin_ids;

    hs_t hs;
    init_hashing_state(&hs, m_setSize, m_internal_bitlen, m_numOfBins, &m_prfState);
    uint32_t outbitlen = hs.outbitlen;

    bin_ids.reset(new uint32_t[m_numOfBins * hs.outbytelen]);
    memset(bin_ids.get(),0,m_numOfBins * hs.outbytelen);

    hash_table.reset(cuckoo_hashing(m_eleptr.get(), m_setSize, m_numOfBins, hs,
                                nelesinbin.get(), perm.get(), 1, bin_ids.get()));

    free_hashing_state(&hs);

    /*
    cout << "bin_ids: ";
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        cout << bin_ids[i] << " ";
    }
    cout << endl;
    */

/*
    boost::thread_group threadpool;

    for (auto &party : m_parties) {
        threadpool.create_thread(boost::bind(&PsiParty::runLeaderAgainstFollower, this,
                                             party, &leaderResults[party.first - 1], nelesinbin, outbitlen, hash_table));
    }
    threadpool.join_all();
*/


    for (auto &party : m_parties) {
        leaderResults[party.first - 1] = (uint8_t*) malloc(m_setSize * getMaskSizeInBytes());
        runLeaderAgainstFollower(party, leaderResults[party.first - 1], nelesinbin, outbitlen, hash_table);
    }

    PRINT_PARTY(m_partyId) << "otpsi was successful" << std::endl;

    m_statistics.afterOTs = clock();

    auto leader = LeaderFactory::getLeader(m_strategy, leaderResults, bin_ids, perm,
                                             m_numOfBins, m_secretShare, getMaskSizeInBytes(), m_setSize, m_parties,
                                             NUM_HASH_FUNCTIONS);

    auto intersection = leader->run();

    m_statistics.intersectionSize = intersection.size();

    PRINT_PARTY(m_partyId) << "found that intersection size is " << intersection.size() << std::endl;

    m_statistics.specificStats.aftetComputing = clock();
}



void PsiParty::runAsFollower(CSocket &leader) {

    // m_crypt->gen_common_seed(&m_prfState, *leader);

    boost::shared_ptr<uint8_t> masks;

    boost::shared_ptr<uint8_t> hash_table;
    boost::shared_ptr<uint8_t> hashed_elements;
    boost::shared_ptr<uint32_t> nelesinbin;

    hs_t hs;

    init_hashing_state(&hs, m_setSize, m_internal_bitlen, m_numOfBins, &m_prfState);
    //Set the output bit-length of the hashed elements
    uint32_t outbitlen = hs.outbitlen;

    struct simple_hash_output output = simple_hashing(m_eleptr.get(), m_setSize, m_numOfBins, 1, hs);
    hash_table.reset(output.res_bins);
    hashed_elements.reset(output.hashed_elements);
    nelesinbin.reset(output.nelesinbin);

    free_hashing_state(&hs);

    masks.reset(new uint8_t[NUM_HASH_FUNCTIONS * m_setSize * getMaskSizeInBytes()]);

    otpsi_server(m_setSize, m_numOfBins, m_internal_bitlen, m_maskbitlen, m_crypt.get(), &leader, 1,
                 hash_table.get(), masks.get(), nelesinbin.get(), outbitlen);


    struct FollowerSet set{hashed_elements, m_setSize, ceil_divide(m_internal_bitlen, 8), hash_table, nelesinbin, m_numOfBins,
        NUM_HASH_FUNCTIONS, masks, getMaskSizeInBytes()};

    auto follower = FollowerFactory::getFollower(m_strategy,set, m_secretShare, leader);
    follower->run();

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

    m_secretShare.reset(new byte[shareSize]);
    memset(m_secretShare.get(), 0, shareSize);

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

        XOR(m_secretShare.get(), result.data(), shareSize);

        /*
        for (uint j = 0; j < shareSize; j += SIZE_OF_BLOCK) {
            __m128i v = _mm_load_si128((__m128i*)(expShare + j));
            _mm_xor_si128(v, *(totalShare + j / SIZE_OF_BLOCK));
        }
        */
    }

    /*
    PRINT_PARTY(m_partyId) << "my secret shares are: ";
    printShares(m_secretShare, m_numOfBins);
    */
}