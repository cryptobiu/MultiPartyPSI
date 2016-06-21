//
// Created by root on 6/20/16.
//

#include "PsiParty.h"
#include <openssl/rand.h>

#define SIZE_OF_BLOCK 16

void PsiParty::additiveSecretShare() {
    if (!(getElementSize() % SIZE_OF_BLOCK)) {
        throw std::system_error();
    }

    uint elementSize = getElementSize();
    uint numOfBlocks = elementSize/SIZE_OF_BLOCK;

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

}