//
// Created by root on 7/22/16.
//

#include <GarbledBF.h>
#include "Followers/GBFFollower.h"
#include "ot-psi.h"
#include <CryptoInfra/Key.hpp>
#include <primitives/Prg.hpp>

#define KEY_SIZE 16

GBFFollower::GBFFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader, const secParameters &parameters, uint32_t numCores) :
        Follower(followerSet, secretShare, leader, parameters, numCores),
        GarbledBloomFilter(m_followerSet.m_maskSizeInBytes, m_parameters.m_statSecParameter, m_followerSet.m_numOfElements) {

    generateHashKeys();
};

GBFFollower::~GBFFollower() {
    delete m_keys;
}

void *GBFFollower::buildGBFInThread(void *gbf_struct) {
    garbled_bf_struct *gbfStruct = reinterpret_cast<garbled_bf_struct*>(gbf_struct);

    RangeHash **hashFuncs = new RangeHash*[gbfStruct->bfParam->k];
    for(int i=0;i<gbfStruct->bfParam->k;i++){
        hashFuncs[i] = new RangeHash();
        RangeHash_Create(hashFuncs[i], gbfStruct->keys+i*gbfStruct->symSecParameter/8, gbfStruct->symSecParameter/8, gbfStruct->bfParam->m);
    }

    for (uint32_t j=0; j < gbfStruct->followerSet->m_numOfElements; j++) {
        uint32_t index = gbfStruct->followerSet->m_elements_to_hash_table.get()[j*gbfStruct->followerSet->m_numOfHashFunctions+gbfStruct->hashIndex];
        GBF_add(gbfStruct->filter, hashFuncs, gbfStruct->bfParam->k,
                gbfStruct->followerSet->m_realElements.get()+j*gbfStruct->followerSet->m_elementSizeInBytes,
                gbfStruct->followerSet->m_elementSizeInBytes,
                gbfStruct->followerSet->m_masks.get()+index*gbfStruct->followerSet->m_maskSizeInBytes);
    }

    for(int i=0;i<gbfStruct->bfParam->k;i++){
        delete hashFuncs[i];
    }
    delete[] hashFuncs;
}

void GBFFollower::buildGBF(){

    vector<pthread_t> check_threads;
    vector<boost::shared_ptr<garbled_bf_struct>> gbfStructs;

    uint8_t *seed = new uint8_t[KEY_SIZE];
    RAND_bytes(seed,KEY_SIZE);
    SecretKey key(seed, KEY_SIZE, "PrgFromOpenSSLAES");
    PrgFromOpenSSLAES prg(ceil_divide(m_bfParam->m*m_bfParam->maskSize*m_followerSet.m_numOfHashFunctions,16));
    prg.setKey(key);

    for (uint32_t i = 0; i < m_followerSet.m_numOfHashFunctions; i++) {

        vector<uint8_t> random;
        prg.getPRGBytes(random, 0, m_bfParam->m*m_bfParam->maskSize);

        pthread_t check_thread;
        boost::shared_ptr<garbled_bf_struct> gbf_struct(new garbled_bf_struct);
        gbf_struct->filter = GBF_Create(m_bfParam.get(), random.data());
        gbf_struct->hashIndex = i;
        gbf_struct->followerSet = &m_followerSet;
        gbf_struct->bfParam = m_bfParam.get();
        gbf_struct->symSecParameter = m_parameters.m_symSecParameter;
        gbf_struct->keys = m_keys;

        //GBFFollower::buildGBFInThread(gbf_struct.get());

        if(pthread_create(&check_thread, NULL, &GBFFollower::buildGBFInThread, (void*)gbf_struct.get())) {
            cerr << "Error in creating new pthread at check element!" << endl;
            exit(0);
        }

        check_threads.push_back(check_thread);

        gbfStructs.push_back(gbf_struct);
    }

    for (auto &check_thread : check_threads) {
        //meanwhile generate the hash table
        //GHashTable* map = otpsi_create_hash_table(ceil_divide(inbitlen,8), masks, neles, maskbytelen, perm);
        //intersect_size = otpsi_find_intersection(eleptr, result, ceil_divide(inbitlen,8), masks, neles, server_masks,
        //		neles * NUM_HASH_FUNCTIONS, maskbytelen, perm);
        //wait for receiving thread
        if(pthread_join(check_thread, NULL)) {
            cerr << "Error in joining pthread at check element!" << endl;
            exit(0);
        }
    }

    for (auto &gbfStruct : gbfStructs) {
        m_filters.insert(m_filters.end(), boost::shared_ptr<GarbledBF>(gbfStruct->filter));
    }
}

void GBFFollower::generateHashKeys() {
    AESRandom* rnd;

    int32_t keyLen=m_parameters.m_symSecParameter/8;

    AESRandom_Create(&rnd, keyLen);

    m_keys = new uint8_t[m_bfParam->k*keyLen];

    for(int i=0;i<m_bfParam->k;i++){
        AESRandom_NextBytes(rnd, m_keys+i*keyLen, keyLen);
    }
}

void GBFFollower::run() {
    xor_masks(m_followerSet.m_masks.get(), m_followerSet.m_maskSizeInBytes, m_secretShare.get(), m_followerSet.m_numOfBins, m_followerSet.m_numOfElementsInBin.get());

    buildGBF();

    m_leader.Send(m_keys,m_bfParam->k*m_parameters.m_symSecParameter/8);

    //send the masks to the receiver
    for (auto &filter : m_filters) {
        send_masks(filter->data, m_bfParam->m,
                   m_followerSet.m_maskSizeInBytes, m_leader);
    }
}