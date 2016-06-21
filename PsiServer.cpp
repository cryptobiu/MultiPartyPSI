//
// Created by Naor on 21/06/2016.
//

PsiServer::PsiServer(ConfigFile config,
    boost::asio::io_service &ioService) :
    m_config(config), m_ioService(ioService) {}

void PsiServer::run() {
    prepareData();
    // TODO: change the model so you accept and all other only connect (not join !!!)
}

void PsiServer::prepareData() {

}

void PsiServer::sendSetsToUsers() {

}

void PsiServer::syncronize() {

}

void PsiServer::receiveStatistics() {

}
