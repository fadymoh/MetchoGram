#ifndef Message_hpp
#define Message_hpp

#include <stdio.h>
#include "base64.hpp"
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <cstring> 

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <sys/types.h>
#include <netdb.h>

typedef sockaddr_in SocketAddress;


enum MessageType { Request, Reply, Ack};

class Message 
{
    private:
        
        MessageType message_type;
        long long message_size;
        void * message;
        
        int rpc_id;
        int operation;

        int fragCount;
        int fragTotal;

        SocketAddress destination;
        std::string source;

        std::string destIp;
        int destPort;


    public:

        // Constructors

        Message(MessageType type, int p_message_size, int rpc_id, int fragC, int fragT, int operation, void * p_message, std::string, int);
        Message(char * marshalled_base64);
        Message();


        // Function 

        char * marshal();

        // Getters

        std::string decoded_string;
        void * getMessage() const;
        long long getMessageSize() const;
        MessageType getMessageType() const;
        int getOperation() const;
        int getrpc_Id() const;
        int getFragC() const;
        int getFragT() const;
        SocketAddress getDest();
        std::string getSource() const;

        std::pair<std::string, int> getDestination();

        // Setters

        void setMessage(void * message, long long message_size);
        void setMessageType(MessageType message_type);
        void setFrag(int fragC, int fragT);
        void setOperation(int operation);
        void setrpc_ID(int rpc_Id);
        void setDest(SocketAddress);
        void setSource(std::string);
        void setAck();

        void setDestination(std::string, int);
};

#endif /* Message_hpp */
