#ifndef Peer_H
#define Peer_H

#include <iostream>
#include <string>
#include <vector>
#include "UDPSocket.hpp"


class Peer: public UDPSocket{

public:
    Peer();
    Peer(int port);

    void LogInServer(std::string, std::string);
    void RegisterServer(std::string, std::string);

    void LogOutServer(std::string, std::string);

    void UploadImageServer(std::string, std::string);
    void RequestImagesServer(std::string);

    void RequestImagePeer(std::string);

private:

    int RPC_ID;

    std::string ServerIP;
    int ServerPort;

};

#endif