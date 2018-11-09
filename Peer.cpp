#include "Peer.h"

Peer::Peer(int port):UDPSocket(port){
    int RPC_ID = 0;

    std::string ServerIP = "123.12.1.23";
    int ServerPort = 8080;

}

void Peer::LogInServer(std::string userName, std::string Password){
    
    std::string msg = userName + "," + Password;

    char * c = new char[msg.size()+1];
    strcpy(c, msg.c_str());

    Message * m = new Message(Request,msg.size()+1, this->RPC_ID,0,0,1,c,this->ServerIP,this->ServerPort);

    this->sendMessage(m);

}
void Peer::RegisterServer(std::string, std::string){
 
}


void Peer::LogOutServer(std::string, std::string){
 
}

void Peer::UploadImageServer(std::string, std::string){

}
void Peer::RequestImagesServer(std::string){

}

void Peer::RequestImagePeer(std::string){


}
