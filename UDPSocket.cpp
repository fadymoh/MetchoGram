//
//  UDPSocket.cpp
//  Sockets_Last
//
//  Created by Ahmad Refaat on 9/25/18.
//  Copyright © 2018 Ahmad Refaat. All rights reserved.
//

#include "UDPSocket.hpp"
#include <string>
#include <iostream>
#include <unistd.h>
#include <poll.h>

char * UDPSocket::getSocketAddress(){

  char * x = new char[32];
  size_t buflen = 32;
  this->GetPrimaryIp(x,buflen);

  std::string p = std::to_string(this->myPort);
  std::string i(x);

  std::string fullIp = i + ":" + p;

  char * to_return = new char[fullIp.length() + 1];
  strcpy (to_return,fullIp.c_str());
  return to_return;
}


void UDPSocket::GetPrimaryIp(char* buffer, size_t buflen){
    //assert(buflen >= 16);

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    //assert(sock != -1);

    const char* kGoogleDnsIp = "8.8.8.8";
    uint16_t kDnsPort = 53;
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(kGoogleDnsIp);
    serv.sin_port = htons(kDnsPort);

    int err = connect(sock, (const sockaddr*) &serv, sizeof(serv));
    //assert(err != -1);

    sockaddr_in name;
    socklen_t namelen = sizeof(name);
    err = getsockname(sock, (sockaddr*) &name, &namelen);
    //assert(err != -1);

    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, buflen);
    //assert(p);

    close(sock);
}

void UDPSocket::initializeMySocket(){

    myAddress =  getSocketAddress();
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        printf("Socket creation failed!\n");
        return;
    }
    // Filling server information

    memset((char *)&myAddr, 0, sizeof(myAddr));

    myAddr.sin_family    = AF_INET; // IPv4
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myAddr.sin_port = htons(this->myPort);

    // Bind the socket with the server address
    if (bind(sock, (struct sockaddr *)&myAddr, sizeof(myAddr)) < 0 ){
        perror("bind failed");
        return;
    }



    // Spawn the two threads that will handle all sending and receiving from now on!

    receiverThread = new std::thread(&UDPSocket::receiveHandler,this);
    senderThread = new std::thread(&UDPSocket::sendingHandler,this);    
}


UDPSocket::UDPSocket(){
    this->myPort = this->defaultPort;
    this->initializeMySocket();
}

UDPSocket::UDPSocket(int port){
    this->myPort = port;
    this->initializeMySocket();
}

// Function that should run on the main thread
// The purpose of this function is to check if there are any available messages to be read!
void UDPSocket::checkMessages(Message * m, Message *& rep, int& status, float &percentReceived){

    receiveMx.lock();

    std::ofstream out("hi.txt");

    out << "Before generateID" << std::endl;
    out << "RPC" << m->getrpc_Id()<<std::endl;
    out << "Source" << m->getSource()<<std::endl;


    std::string UID = generateId(m);
    out << "After generateID" << std::endl;

    if(this->ProgArray[UID].stat == Pending)
        rep = NULL, status = 2, percentReceived = float(this->ProgArray[UID].currentReply) / float(this->ProgArray[UID].totalReply);
    else if(this->ProgArray[UID].stat == Success)
            out << "In Success" << std::endl, rep = this->ProgArray[UID].Reply, status = 1;
    else
        rep = NULL, status = 0;

    receiveMx.unlock();

}

// Function that should run on the main thread
// The purpose of this function is to check if there are any available messages to be read!

bool UDPSocket::checkMessages(Message *& m){

    receiveMx.lock();

    // check if there any received messages!
    // If not just return false
    if(receiveArray.size() <= 0) {
        receiveMx.unlock();
        return false;
    }

    printf("Unread Messages: %d\n",(int)receiveArray.size());

    // Else capture that message and return true!
    m = receiveArray.front();
    receiveArray.pop();
    
    receiveMx.unlock();

    return true;

}

// This function should run in the main thread
// The purpose of this function is to add new messages to the send Queue
void UDPSocket::sendMessage(Message * m){

    // printf("Message will be added in Queue\n");

    std::unique_lock<std::mutex> locker(sendMx);

    // Push the message on the sender Queue!
    senderArray.push(m);

    locker.unlock();
    sendCond.notify_all();

    // printf("Message added in Queue\n");

}


// This function runs in a specific thread and should not be called elsewhere.
// The purpose of this function is to handle the receiving of any new messages 
// and adding them to the received queue

// This function should extend functionality by allowing big packets and to assemble 
// fragmented messages 


// Messages received as replies should be handled in another array 

void UDPSocket::receiveHandler(UDPSocket* myUDPSocket){

    SocketAddress from;
    socklen_t fromlen = sizeof(from);

    char buff[myUDPSocket->MAX_SIZE];

    std::unordered_map<std::string, std::pair<int,std::vector<Message *>>> fragmented_messages;

    while(true){
        
        // out << "Waiting for packets received " << std::endl;

        //  DONT FORGET HISTORY
        int ret = (int) recvfrom(myUDPSocket->sock, buff, myUDPSocket->MAX_SIZE, 0, (struct sockaddr *)& from, &fromlen);

       
        if(ret < 0){
            printf("Receive Failed...");
        }

        Message  * m = new Message(buff);

        Message * ack = new Message(buff);
        ack->setAck();

        if(m->getMessageType() == Ack){
            addACK(myUDPSocket,m);
            (myUDPSocket->cv_ack).notify_all();
            continue;
        }

        ////////////////
        // send ack
        ////////////////

        std::string ss = ack->marshal();

        char * to_send = new char[ss.size() + 1];
        strcpy(to_send, ss.c_str());

        int addrlen = sizeof(from);
        int ln = (int)strlen(to_send) + 1;

        int ret_2 = (int) sendto(myUDPSocket->sock,to_send, ln, 0, (struct sockaddr *)& from, addrlen);

        ////////////////
        // ack sent
        ////////////////

        // Generate UID for request
        std::string uid = myUDPSocket->generateId(m);


        if(m->getMessageType == Reply){
            myUDPSocket->ProgArray[uid].totalReply = m->getFragT();
            myUDPSocket->ProgArray[uid].currentReply = m->getFragC()+1;
        }

        // CHECK IF MESSAGE WAS NEVER RECEIVED 
        // CREATE THE VECTOR OF FRAGMENTS

        if(fragmented_messages.find(uid) == fragmented_messages.end()){
            std::vector<Message *> frags;
            frags.resize(m->getFragT());

            for(int i = 0; i < frags.size(); i++) frags[i] = NULL;

            fragmented_messages[uid] = std::pair<int,std::vector<Message *>>(0,frags) ;
        }

        // ATTACH 

        if(((fragmented_messages[uid]).second)[m->getFragC()] == NULL){
            ((fragmented_messages[uid]).second)[m->getFragC()] = m;
            (fragmented_messages[uid]).first++;
        }


        if((fragmented_messages[uid]).first == (fragmented_messages[uid]).second.size()){
            
            // all fragments have been received
            // concatenate and push to received array

            std::string tot((char *)(fragmented_messages[uid]).second[0]->getMessage());

            Message * complete = (fragmented_messages[uid]).second[0];
            for(int i = 1; i < complete->getFragT(); i++){
                tot += std::string((char *)(fragmented_messages[uid]).second[i]->getMessage()) ;
            }

            char * c = new char[tot.size()+1];
            strcpy(c, tot.c_str());

            complete->setMessage(c, tot.size()+1);
            

             // check with mr rouby
            (myUDPSocket->receiveMx).lock();
            
            if(m->getMessageType == Reply){

                myUDPSocket->ProgArray[uid].Reply = complete;
                myUDPSocket->ProgArray[uid].stat = Success;
                
            }   
            // Here check if it's a regular message
            // If not then should notify the sending Handler as this will be the reply for a sent packet.
            (myUDPSocket->receiveArray).push(complete);

            // After receiving, we should manually send a reply, to the address acknowledging the receiving of packet.
            // From here onwards, every packet received, it should be used to assemble its corresponding message
        
            (myUDPSocket->receiveMx).unlock();
        }
       
    }

}

// This function runs in a specific thread and should not be called elsewhere.
// The purpose of this function is to handle the sending of any new messages 

// The function should handle sending each message one by one.
// For each message, it should be divided into fragments.
// Each fragment should be delivered properly before attempting to send another fragment.
// Receiving acknowledgments will be be handled in the replies array away from regular requests.

void UDPSocket::sendingHandler(UDPSocket* myUDPSocket){

    std::vector<Message*> frags;
    int cnt;
    bool x;
    while(true){
        std::unique_lock<std::mutex> locker(myUDPSocket->sendMx);
        (myUDPSocket->sendCond).wait(locker, [&](){return (myUDPSocket->senderArray).size() > 0;});

        Message* toBeUsed = (myUDPSocket->senderArray).front();
        (myUDPSocket->senderArray).pop();
        locker.unlock();

        (myUDPSocket->sendCond).notify_all();
        toBeUsed->setSource(std::string(myUDPSocket->myAddress));
        
        std::ofstream out("hi.txt");
        out << std::string(myUDPSocket->myAddress) << std::endl;
        
        std::string uid = myUDPSocket->generateId(toBeUsed);

        Progress nw;
        myUDPSocket->ProgArray[uid] = nw;

        prepareMessage(frags, toBeUsed);

        std::pair<std::string, int> des =  toBeUsed->getDestination();

        SocketAddress destinationAdd;

        memset((char*)&destinationAdd, 0, sizeof(destinationAdd));
        destinationAdd.sin_family = AF_INET;
        destinationAdd.sin_addr.s_addr = htonl(INADDR_ANY);
        destinationAdd.sin_port = htons(des.second);
        // char * host = new char[des.first.size()+1];
        // strcpy(host, des.first.c_str());

        // struct hostent *hp;     /* host information */
        // hp = gethostbyname(host);
        // if (!hp) {
        // fprintf(stderr, "could not obtain address of %s\n", host);
        //         return;
        // }
            
        // memcpy((void *)&destinationAdd.sin_addr, hp->h_addr_list[0], hp->h_length);

        for(int i = 0; i < frags.size(); i++)
        {

            std::string s = frags[i]->marshal();

        
            char * to_send = new char[s.size() + 1];
            strcpy(to_send, s.c_str());

            int addrlen = sizeof(destinationAdd);
            int ln = (int)strlen(to_send) + 1;

            cnt = 3;
            do {
                int ret = (int) sendto(myUDPSocket->sock,to_send, ln, 0, (struct sockaddr *)& destinationAdd, addrlen);

                if(ret < 0)
                    printf("Error sending packet");
                
                std::unique_lock<std::mutex> lk(myUDPSocket->cv_m_ack);
                auto now = std::chrono::system_clock::now();
                (myUDPSocket->cv_ack).wait_until(lk, (now + 5*100ms));                
                lk.unlock();


                x = checkACK(myUDPSocket,frags[i]);

            }while(cnt-- && !x);

            if(!x){
                myUDPSocket->ProgArray[uid].stat = Failure;
                printf("Error sending request!\n");
                break;
            }
            else
                printf("Received ACK %d\n", frags[i]->getFragC());
                
        }
        for(int i = 0; i <frags.size();i++)
            delete frags[i];
        frags.clear();
        // Here we should handle dividing the message into fragments
        // Also we should make sure that each individual fragment was delivered succesffully.
        // Each packet should be attempted to be sent max 10 times.
        // with a timeout of 1 second.
        // If a packet can't be sent, report this and add to failed queue.
     }
}

void UDPSocket::prepareMessage(std::vector<Message *> & frags , Message * rep){

    std::string rep_mess = std::string((char*)rep->getMessage());

    int n = (rep_mess.size() - 1) / MSG_SIZE + 1;

    int ID = rep->getrpc_Id();
    int oper = rep->getOperation();
    MessageType ms = rep->getMessageType();

    std::vector<std::string> subs;

    subs.resize(n);

    for(int i = 0; i < n; i++)
        subs[i] =  (i != n-1) ? rep_mess.substr(MSG_SIZE*i,MSG_SIZE) : rep_mess.substr(MSG_SIZE*i);

    for(int i = 0; i < n; i++)
    {
        Message * mess = new Message;
        char * s = new char[subs[i].size()+1];
        strcpy(s, subs[i].c_str());

        mess->setFrag(i,n);
        mess->setMessage(s, subs[i].size()+1);
        mess->setMessageType(ms);
        mess->setrpc_ID(ID);
        mess->setOperation(oper);

        std::pair<std::string, int> des =  rep->getDestination();
        mess->setDestination(des.first, des.second);    
        mess->setSource(rep->getSource());
        frags.push_back(mess);
    }
}

void UDPSocket::addACK(UDPSocket * sock, Message * mess){

    int id = mess->getrpc_Id();
    int frgc = mess->getFragC();
    int frgt = mess->getFragT();

    std::string UID = (mess->getSource()) + std::to_string(id) + std::to_string(frgc) + std::to_string(frgt);

    (sock->ackMx).lock();

    sock->ackArray[UID] = mess;
    
    (sock->ackMx).unlock();

}

bool UDPSocket::checkACK(UDPSocket * sock, Message * mess)
{
    int id = mess->getrpc_Id();
    int frgc = mess->getFragC();
    int frgt = mess->getFragT();

    std::string UID = mess->getDestination().first + ":" + std::to_string(mess->getDestination().second)  + std::to_string(id) + std::to_string(frgc) + std::to_string(frgt);

    (sock->ackMx).lock();

    bool flag = true;

    if(sock->ackArray.find(UID) == sock->ackArray.end())
        flag = false;

    if(flag)
        sock->ackArray.erase(sock->ackArray.find(UID));

    (sock->ackMx).unlock();

    return flag;

}

char* UDPSocket::getIPfromSocketAddress(SocketAddress& targetSocket)
{
    std::string ip(inet_ntoa(targetSocket.sin_addr));
    std::string port = std::to_string(targetSocket.sin_port);

    std::string adrr = ip + ":" + port;
    char* to_return = new char[adrr.size() + 1];
    strcpy(to_return, adrr.c_str());
    
    return to_return;
}

std::string UDPSocket::generateId(Message* mess){
    return mess->getSource() + std::to_string(mess->getrpc_Id());
}