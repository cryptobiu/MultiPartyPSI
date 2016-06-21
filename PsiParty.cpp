//
// Created by root on 6/20/16.
//

#include "PsiParty.h"
#include <openssl/rand.h>
#include "PRG.hpp"

#define SIZE_OF_BLOCK 16

void PsiParty::additiveSecretShare() {
    if (!(getElementSize() % SIZE_OF_BLOCK)) {
        throw std::system_error();
    }

    uint numOfBlocks = 2 * stoi(m_config.Value("General", "elementSizeInBits")) /
            stoi(m_config.Value("General", "blockSizeInBits"));

    uint elementSize = SIZE_OF_BLOCK * numOfBlocks;

    uint shareSize = SIZE_OF_BLOCK * stoi(m_config.Value("General", "setSize"));
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
        block *share = (block *)_mm_malloc(shareSize, SIZE_OF_BLOCK);
        for (auto &share : shares) {
            PRG prg(reinterpret_cast<byte*>((share.get())[i]), cachedSize=shareSize);
            byte *expShare = prg.getRandomBytes();
            for (j = 0; j < shareSize; j += BLOCK_SIZE) {
                __m128i v = _mm_load_si128(*(expShare + j));
                _mm_xor_si128(v, *(share + j / SIZE_OF_BLOCK));
            }
        }
    }

}