#ifndef Peer_H
#define Peer_H

#include <iostream>
#include <string>
#include <vector>
#include "UDPSocket.hpp"
#include <thread>
#include <fstream>

#define LOGIN_OP 0
#define SIGNUP_OP 1
#define UPIM_OP 2
#define LOGOUT_OP 3
#define RQIM_OP 4

class Peer: public UDPSocket{

public:
    Peer();
    Peer(int port);

    void LogInServer(std::string, std::string);
    void RegisterServer(std::string, std::string);
    void LogOutServer(std::string);
    void UploadImageServer(std::string, std::string);
    void RequestImagesServer();
    void RequestImagePeer(std::string);
    std::string retrieveImage(const std::string&);
    //static void MessageHandler(Peer *);
    void ChangeFlag();
    void doOperation();
    bool flag;

private:
    static void myHandler(Message*, Peer*);

    int RPC_ID;
    std::string cover_image = "araf.jpg";
    std::string ServerIP;
    int ServerPort;
    std::ofstream output;
    //std::thread * handler; 
};

#endif