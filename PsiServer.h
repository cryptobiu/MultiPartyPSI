//
// Created by Naor on 21/06/2016.
//

#ifndef MULTIPARTYPSI_PSISERVER_H
#define MULTIPARTYPSI_PSISERVER_H

class PsiServer {
public:
    PsiServer(ConfigFile config,
              boost::asio::io_service &ioService);
    virtual ~PsiServer() {};

    void run();
private:
    void prepareData();
    void sendSetsToUsers();
    void syncronize();
    void receiveStatistics();

    ConfigFile m_config;
    boost::asio::io_service &m_ioService;
};

#endif //MULTIPARTYPSI_PSISERVER_H
