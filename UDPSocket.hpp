//
//  UDPSocket.hpp
//  Sockets_Last
//
//  Created by Ahmad Refaat on 9/25/18.
//  Copyright © 2018 Ahmad Refaat. All rights reserved.
//

#ifndef UDPSocket_hpp
#define UDPSocket_hpp

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "Message.hpp"
#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>  
#include <condition_variable>
#include <queue>
#include <chrono>  
#include <fstream>

using namespace std::chrono_literals;


typedef sockaddr_in SocketAddress;

enum Status {Failure, Success, Pending};

struct Progress{

    Status stat;
    int totalReply, currentReply;
    Message * Reply;

    Progress(){
        stat = Pending;
        totalReply = 1;
        currentReply = 0;
        Reply = NULL;
    }

};
class UDPSocket {

protected:

    int defaultPort = 8080;
    const static int MAX_SIZE = 50000;
    const static int MSG_SIZE = 30000;

    int sock;
    struct sockaddr_in myAddr;
    char * myAddress;
    int myPort;
    char buff[MAX_SIZE];


    void initializeMySocket();
    void GetPrimaryIp(char* buffer, size_t buflen);

    static char* getIPfromSocketAddress(SocketAddress&);
    static std::string generateId(Message* mess);

    static void receiveHandler(UDPSocket*);
    static void sendingHandler(UDPSocket*);

    static void prepareMessage(std::vector<Message*> &, Message *);
    static bool checkACK(UDPSocket *, Message*);
    static void addACK(UDPSocket * sock, Message * mess);

    std::queue<Message *> receiveArray;
    std::queue<Message *> senderArray;

    std::unordered_map<std::string, Message *> ackArray;
    std::unordered_map<std::string, Progress> ProgArray;
    std::mutex receiveMx;

    std::mutex ackMx;

    std::mutex sendMx;
    std::condition_variable sendCond;

    std::thread* senderThread;
    std::thread* receiverThread;

    std::condition_variable cv_ack;
    std::mutex cv_m_ack;

public:

    UDPSocket ();
    UDPSocket (int port);

    char * getSocketAddress();
    bool checkMessages(Message*& m);
    void checkMessages(Message *, Message *& , int& , float&);
    void sendMessage(Message* m);
};

#endif /* UDPSocket_hpp */
