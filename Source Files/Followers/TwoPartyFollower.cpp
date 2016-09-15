//
// Created by naor on 9/15/16.
//

#include <ot-psi.h>
#include "Followers/TwoPartyFollower.h"

void TwoPartyFollower::run() {
    //send the masks to the receiver
    send_masks(m_followerSet.m_masks.get(), m_followerSet.m_numOfElements * m_followerSet.m_numOfHashFunctions, m_followerSet.m_maskSizeInBytes, m_leader);
}