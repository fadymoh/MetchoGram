#include <iostream>


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


    Message *temp;
    int c, serverPort;


    while(true){

        printf("1-To change target\n2-To check delivered messages\n3-To send a new message\n");
        cin >> c;

        if(c == 1){
            cout << "Please enter peer port number: ";
            cin >> serverPort;

        }else if(c == 2){

            if(s->checkMessages(temp)){
                printf("Message Received: %s\nMessageType: %d\nRPC ID: %d", (char*)temp->getMessage(), (int)temp->getMessageType(), temp->getrpc_Id());
            }else {

                cout << "No messages available " << endl;

            }

        }else if(c == 3){
            cin.ignore();
            string t;
            cout << "Please enter message to send: ";
            getline(cin,t);
            
            //if(temp != NULL)
            //    delete temp;

            char* to_send = new char[t.size() + 1];
            strcpy(to_send, t.c_str());


            // Should change this static ip with another one!
            temp = new Message(Request, t.size()+1, rpcId++, 0, 0, 0, to_send, "10.40.33.221", serverPort);
            s->sendMessage(temp);
        }

        
    }


    return 0;
}