#include <iostream>


#include "Peer.h"
#include "UDPSocket.hpp"
#include "Message.hpp"

using namespace std;



int main(){


    
    UDPSocket *s;
    int rpcId = 0;


    int p;
    cout << "Please enter the port to bind on: " ;
    cin >> p;

    s = new UDPSocket(p);
    printf("You're computer is running on: %s\n", s->getSocketAddress()); 


    SocketAddress servaddr;


    Message *sent;
    Message *received;
    int c, serverPort;


    while(true){

        printf("1-To change target\n2-To check delivered messages\n3-To send a new message\n");
        cin >> c;

        if(c == 1){
            cout << "Please enter peer port number: ";
            cin >> serverPort;

        }else if(c == 2){
            int stat;
            float percent;

            //s->doOperation();
            if (s->checkMessages(received))
                printf("1. Message Received: %s\nMessageType: %d\nRPC ID: %d\n", (char*)received->getMessage(), (int)received->getMessageType(), received->getrpc_Id());

            // s->checkMessages(sent,received,stat, percent);

            // if(!stat)
            //     cout << "Failed to send Message Main" <<endl;
            // else if(stat == 1)
            //     printf("Hello"),printf("2. Message Received: %s\nMessageType: %d\nRPC ID: %d\n", (char*)received->getMessage(), (int)received->getMessageType(), received->getrpc_Id());
            // else
            //     printf("Pending Message\nPercentate Received: %0.2f\n",percent);

        }else if(c == 3){
            cin.ignore();
            string t;
            int opID;
        
            cout << "Enter OperationID: ";
            cin >> opID;
            cin.ignore();
            cout << "Please enter message to send: ";
            getline(cin,t);
            //if(temp != NULL)
            //    delete temp;

            char* to_send = new char[t.size() + 1];
            strcpy(to_send, t.c_str());

            // Should change this static ip with another one!
            sent = new Message(Request, t.size()+1, rpcId++, 0, 0, opID, to_send, "10.40.33.221", serverPort);
            s->sendMessage(sent);
        }

        
    }


    return 0;
}