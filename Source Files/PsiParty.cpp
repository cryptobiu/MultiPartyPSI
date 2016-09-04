//
// Created by root on 6/20/16.
//


#include <boost/thread/thread.hpp>
#include "defs.h"
#include <immintrin.h>
#include "ot-psi.h"
#include "hashing_util.h"
#include "PsiParty.h"
#include "Follower.h"
#include "NaiveFollower.h"
#include "FollowerFactory.h"
#include "LeaderFactory.h"
#include "NaiveLeader.h"
#include <iomanip>
#include "primitives/Prg.hpp"

#define KEY_SIZE 16

PsiParty::PsiParty(uint partyId, ConfigFile &config, boost::asio::io_service &ioService) :
        BaseMPSIParty(partyId, config, ioService)
{
    m_serverSocket.Receive(reinterpret_cast<byte *>(&m_strategy), 1);
    PRINT_PARTY(m_partyId) << "is executing strategy " << getStrategy(m_strategy) << std::endl;
}

void PsiParty::initPsi() {
    setBinsParameters();

    m_statistics.partyId = m_partyId;

    initializeMaskSize();

    uint32_t elebytelen = ceil_divide(m_elementSizeInBits, 8);
    /*
    PRINT_PARTY(m_partyId) << "Hashing occurs" << std::endl;
    //Hash elements into a smaller domain
    m_eleptr.reset(new uint8_t[getMaskSizeInBytes() * m_setSize]);
    domain_hashing(m_setSize, m_elements.get(), elebytelen, m_eleptr.get(), getMaskSizeInBytes(), m_crypt.get());
    m_internal_bitlen = m_maskbitlen;
    */

    // TODO: solve the problem with the CRF so that domain hashing would be not needed even for very similar elements
    if(m_elementSizeInBits > m_maskbitlen) {
        //Hash elements into a smaller domain
        m_eleptr.reset(new uint8_t[getMaskSizeInBytes() * m_setSize]);
        domain_hashing(m_setSize, m_elements.get(), elebytelen, m_eleptr.get(), getMaskSizeInBytes(), m_crypt.get());
        m_internal_bitlen = m_maskbitlen;
    } else {
        m_eleptr = m_elements;
        m_internal_bitlen = m_elementSizeInBits;
    }

    //printShares(m_eleptr.get(),m_setSize,elebytelen);
}

void PsiParty::setBinsParameters() {

    m_numOfBins = ceil(EPSILON * m_setSize);

    if(ceil_divide(m_setSize, m_numOfBins) < 3)
        m_maxBinSize = 3*max(ceil_log2(m_setSize),3);
    else
        m_maxBinSize = 6*max((int) ceil_divide(m_setSize, m_numOfBins), 3);
}

void PsiParty::initializeMaskSize() {
    switch(m_strategy) {
        case Strategy::NAIVE_METHOD_SMALL_N:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter + (m_numOfParties-1)*ceil_log2(m_setSize), 8);
            break;
        case Strategy::NAIVE_METHOD_LARGE_N:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter + (m_numOfParties-1)*m_setSize, 8);
            break;
        case Strategy::SIMPLE_HASH:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter + (m_numOfParties-1)*m_maxBinSize, 8);
            break;
        case Strategy::GAUSS_SIMPLE_HASH:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter + (m_numOfParties-1)*m_maxBinSize, 8);
            break;
        case Strategy::CUCKOO_HASH:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter + (m_numOfParties-1)*(m_parameters.m_statSecParameter/ceil_log2(m_setSize)+2), 8);
            break;
        case Strategy::POLYNOMIALS:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter, 8);
            break;
        case Strategy::BLOOM_FILTER:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter, 8);
            break;
        case Strategy::BINARY_HASH:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter, 8);
            break;
        case Strategy::POLYNOMIALS_SIMPLE_HASH:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter, 8);
            break;
        case Strategy::BINARY_HASH_SIMPLE_HASH:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter, 8);
            break;
        case Strategy::CUCKOO_HASH_POLYNOMIALS:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter + (m_numOfParties-1)*3, 8);
            break;
        case Strategy::CUCKOO_HASH_BLOOM_FILTER:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter + (m_numOfParties-1)*3, 8);
            break;
        case Strategy::CUCKOO_HASH_BINARY_HASH:
            m_maskbitlen = pad_to_multiple(m_parameters.m_statSecParameter + (m_numOfParties-1)*3, 8);
            break;
        default:
            throw system_error();
    }
}

uint32_t PsiParty::run() {
    initPsi();

    m_statistics.beginTime = clock();

    PRINT_PARTY(m_partyId) << "additive secret sharing is runnning" << std::endl;

    additiveSecretShare();

    m_statistics.afterSharing = clock();

    uint32_t leaderId = stoi(getValFromConfig(m_config, "General", "leaderid"));

    if (m_partyId == leaderId) {
        PRINT_PARTY(m_partyId) << "run as leader" << std::endl;
        uint32_t intersectionSize = runAsLeader();
        return intersectionSize;
    }
    else {
        PRINT_PARTY(m_partyId) << "run as follower"  << std::endl;
        runAsFollower(*m_parties[leaderId]);
        return 0;
    }
}

void PsiParty::runLeaderAgainstFollower(const std::pair<uint32_t, boost::shared_ptr<CSocket>> &party, const boost::shared_ptr<uint8_t> &leaderResults,
                                        const boost::shared_ptr<uint32_t> &nelesinbin, uint32_t outbitlen, const boost::shared_ptr<uint8_t> &hash_table) {

    PRINT_PARTY(m_partyId) << "run leader against party " << party.first << std::endl;

    // uint8_t* seed_buf = (uint8_t*) malloc(m_seedSize);
    // RAND_bytes(seed_buf, m_seedSize);
    //m_crypt->gen_common_seed(&m_prfState, *party.second);

    //Perform the OPRG execution
    //cout << "otpsi client running ots" << endl;
    boost::shared_ptr<crypto> crypt(initializeCrypto());
    oprg_client(hash_table.get(), m_numOfBins, m_setSize, nelesinbin.get(), outbitlen, m_maskbitlen,
                crypt.get(), party.second.get(), 1, leaderResults.get());

    PRINT_PARTY(m_partyId) << "done running leader against party " << party.first << std::endl;
}

uint32_t PsiParty::runAsLeader() {

    map<uint32_t , boost::shared_ptr<uint8_t>> leaderResults;

    boost::shared_ptr<uint32_t> perm(new uint32_t[m_setSize]);
    memset(perm.get(),0,m_setSize*sizeof(uint32_t));

    boost::shared_ptr<uint32_t> nelesinbin(new uint32_t[m_numOfBins]);
    memset(nelesinbin.get(),0,m_numOfBins);

    boost::shared_ptr<uint8_t> hash_table;

    boost::shared_ptr<uint32_t> bin_ids;

    hs_t hs;
    init_hashing_state(&hs, m_setSize, m_internal_bitlen, m_numOfBins, &m_prfState);
    uint32_t outbitlen = hs.outbitlen;

    bin_ids.reset(new uint32_t[m_numOfBins]);
    memset(bin_ids.get(),0,m_numOfBins*sizeof(uint32_t));

    boost::shared_ptr<uint32_t> hashed_by(new uint32_t[m_numOfBins]);
    memset(hashed_by.get(),0,m_numOfBins * sizeof(uint32_t));

    hash_table.reset(cuckoo_hashing(m_eleptr.get(), m_setSize, m_numOfBins, hs,
                                nelesinbin.get(), perm.get(), 1, bin_ids.get(), hashed_by.get()));

    /*
    cout << "hashed by: ";
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        cout << hashed_by.get()[i] << " ";
    }
    cout << std::endl;
    */

    free_hashing_state(&hs);

    /*
    cout << "bin_ids: ";
    for (uint32_t i = 0; i < m_numOfBins; i++) {
        cout << bin_ids[i] << " ";
    }
    cout << endl;
    */

    boost::thread_group threadpool;

    for (auto &party : m_parties) {
        leaderResults[party.first] = boost::shared_ptr<uint8_t>(new uint8_t[m_setSize * getMaskSizeInBytes()]);
        threadpool.create_thread(boost::bind(&PsiParty::runLeaderAgainstFollower, this,
                                             party, leaderResults[party.first], nelesinbin, outbitlen, hash_table));
    }
    threadpool.join_all();

    boost::shared_ptr<CuckooHashInfo> hashInfo(new CuckooHashInfo[m_setSize]);
    for (uint32_t i = 0; i < m_setSize; i++) {
        hashInfo.get()[i].tableIndex = -1;
        hashInfo.get()[i].binIndex = -1;
    }

    for (uint32_t i = 0; i < m_numOfBins; i++) {
        // if bin is not empty
        if (bin_ids.get()[i] != 0) {
            uint32_t elementIndex = bin_ids.get()[i]-1;
            hashInfo.get()[elementIndex].binIndex = i;
            hashInfo.get()[elementIndex].hashedBy = hashed_by.get()[i];
        }
    }

    for (uint32_t i = 0; i < m_setSize; i++) {
        uint32_t elementIndex = perm.get()[i];
        hashInfo.get()[elementIndex].tableIndex = i;
    }

    m_statistics.afterOTs = clock();

    auto leader = LeaderFactory::getLeader(m_strategy, leaderResults, hashInfo,
                                             m_numOfBins, m_secretShare, getMaskSizeInBytes(), m_setSize,
                                           m_eleptr, ceil_divide(m_internal_bitlen, 8), m_parties, NUM_HASH_FUNCTIONS, m_maxBinSize, m_parameters);

    auto intersection = leader->run();

    m_statistics.intersectionSize = intersection.size();

    m_statistics.specificStats.aftetComputing = clock();

    return intersection.size();
}



void PsiParty::runAsFollower(CSocket &leader) {

    // m_crypt->gen_common_seed(&m_prfState, *leader);

    boost::shared_ptr<uint8_t> masks;

    boost::shared_ptr<uint8_t> hash_table;
    boost::shared_ptr<uint8_t> hashed_elements;
    boost::shared_ptr<uint32_t> nelesinbin;
    boost::shared_ptr<uint32_t> elements_to_hash_table;
    boost::shared_ptr<uint32_t> bin_to_elements_to_hash_table;

    hs_t hs;

    init_hashing_state(&hs, m_setSize, m_internal_bitlen, m_numOfBins, &m_prfState);
    //Set the output bit-length of the hashed elements
    uint32_t outbitlen = hs.outbitlen;

    struct simple_hash_output output = simple_hashing(m_eleptr.get(), m_setSize, m_numOfBins, 1, hs);
    hash_table.reset(output.res_bins);
    hashed_elements.reset(output.hashed_elements);
    nelesinbin.reset(output.nelesinbin);
    elements_to_hash_table.reset(output.elements_to_hash_table);
    bin_to_elements_to_hash_table.reset(output.bin_to_elements_to_hash_table);

    free_hashing_state(&hs);

    masks.reset(new uint8_t[NUM_HASH_FUNCTIONS * m_setSize * getMaskSizeInBytes()]);

    oprg_server(hash_table.get(), m_numOfBins, m_setSize * NUM_HASH_FUNCTIONS, nelesinbin.get(), outbitlen, m_maskbitlen, m_crypt.get(), &leader, 1, masks.get());

    struct FollowerSet set{hashed_elements, m_setSize, ceil_divide(m_internal_bitlen, 8), elements_to_hash_table, nelesinbin, m_numOfBins,
        NUM_HASH_FUNCTIONS, masks, getMaskSizeInBytes(), m_eleptr, bin_to_elements_to_hash_table, m_maxBinSize};

    auto follower = FollowerFactory::getFollower(m_strategy,set, m_secretShare, leader, m_parameters);
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
    }

    for (uint i = 1; i <= m_partyId-1; i++) {
        byte *share = new byte[KEY_SIZE];
        m_parties[i]->Receive(share, KEY_SIZE);
        shares.push_back(boost::shared_ptr<byte>(share));
    }

    m_secretShare.reset(new byte[shareSize]);
    memset(m_secretShare.get(), 0, shareSize);

    for (auto &share : shares) {
        SecretKey key(share.get(), KEY_SIZE, "PrgFromOpenSSLAES");
        PrgFromOpenSSLAES prg(ceil_divide(shareSize,16));

        prg.setKey(key);
        vector<byte> result;
        prg.getPRGBytes(result, 0,shareSize);

        XOR(m_secretShare.get(), result.data(), shareSize);

        /*
        for (uint j = 0; j < shareSize; j += SIZE_OF_BLOCK) {
            __m128i v = _mm_load_si128((__m128i*)(expShare + j));
            _mm_xor_si128(v, *(totalShare + j / SIZE_OF_BLOCK));
        }
        */
    }
}
