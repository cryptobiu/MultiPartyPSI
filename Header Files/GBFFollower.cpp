//
// Created by root on 7/22/16.
//

#include "GBFFollower.h"

GBFFollower::GBFFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader) :
        Follower(followerSet, secretShare, leader), m_sver(new Server()) {
    initServer();
};

void GBFFollower::buildGBF(){
    m_sver->hashFuncs= calloc(m_sver->bfParam->k, sizeof(RangeHash*));
    for(int i=0;i<m_sver->bfParam->k;i++){
        RangeHash_Create(&m_sver->hashFuncs[i], m_sver->keys[i], m_sver->secLev/8, m_sver->bfParam->m);
    }
    AESRandom* rnd;

    AESRandom_Create(&rnd, m_sver->secLev/8);
    for(int i=0;i<m_sver->n;i++){
        GBF_add(m_sver->filter, m_sver->hashFuncs, m_sver->bfParam->k, m_sver->set[i], defaultDatalen, m_sver->eHash[i], rnd);

    }
    //GBF_doFinal(sver->filter, rnd);
    AESRandom_Destroy(rnd);
}

void GBFFollower::initServer(uint32_t securityParameter, uint8_t** input, uint32_t setSize, uint32_t dataLen,
                   uint8_t** masks){
/*
    in_port_t port=defaultPort;

    int32_t n=setSize;
    int32_t k=securityParameter;

    int32_t multi=1;
    int32_t verify=0;
    int32_t partial=0;
    int parts=1;

    for(int i=1;i<argc;i+=2){
        if(strcmp(argv[i], "-p")==0){
            port=strtol(argv[i+1],NULL,10);
        }

        }else if(strcmp(argv[i], "-t")==0){
            multi=(int32_t)strtol(argv[i+1],NULL,10);
        }else if (strcmp(argv[i], "-v")==0){
            verify=(int32_t)strtol(argv[i+1],NULL,10);
        }else if(strcmp(argv[i], "-d")==0){
            partial=1;
            parts=(int32_t)strtol(argv[i+1],NULL,10);

        }
    }
*/
    // #define GBFSigmaByteLen 10

    m_sver.new Server();

    m_sver->secLev=securityParameter;
    m_sver->set=input;
    m_sver->n=setSize;
    m_sver->eHash=masks;
    BF_GenerateParameters(&m_sver->bfParam, setSize, securityParameter);
    GBF_Create(&m_sver->filter, m_sver->bfParam->m, m_sver->bfParam->k);

/*
    sver->partialTransfer=partial;
    sver->parts=parts;

    OTExtSndr_Create(&sver->sndr, sver->bfParam->m, sver->bfParam->k, sver->filter->data, GBFSigmaByteLen);
    sver->multi=multi;

    if(multi==1){
        int32_t cores= num_cores();
        sver->threads=cores;
    }
    sver->verify=verify;

    int32_t r = sver->bfParam->m%chunkSize;

    if(r==0){
        sver->count=sver->bfParam->m/chunkSize;
    }else{
        sver->count=sver->bfParam->m/chunkSize+1;
    }

    sver->startBit=0;
    r=sver->bfParam->m%(sver->parts*8);
    int32_t stepByte;
    if(r==0){
        stepByte=sver->bfParam->m/(sver->parts*8);
    }else{
        stepByte=sver->bfParam->m/(sver->parts*8)+1;
    }

    sver->stepBit=stepByte*8;
    sver->endBit=sver->startBit+sver->stepBit;
    sver->port=port;
    */
    return m_sver;


}