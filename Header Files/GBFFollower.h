//
// Created by root on 7/22/16.
//

#ifndef MULTIPARTYPSI_GBFFOLLOWER_H
#define MULTIPARTYPSI_GBFFOLLOWER_H


class GBFFollower {
public:
    GBFFollower(const FollowerSet& followerSet, const boost::shared_ptr<uint8_t> &secretShare, CSocket &leader);
    virtual ~GBFFollower() {};

    virtual void run();

    void buildGBF();

    void initServer(uint32_t securityParameter, uint8_t** input, uint32_t setSize, uint32_t dataLen,
                       uint8_t** masks);
private:

    boost::shared_ptr<Server> m_sver;

    COPY_CTR(GBFFollower);
    ASSIGN_OP(GBFFollower);
};


#endif //MULTIPARTYPSI_GBFFOLLOWER_H
