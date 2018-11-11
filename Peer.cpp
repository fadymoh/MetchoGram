#include "Peer.h"
#include "steg.h"

Peer::Peer(int port):UDPSocket(port){
    
    int RPC_ID = 0;

    this->ServerIP = "123.12.1.23";
    this->ServerPort = 8080;

    this->flag = true;

}

void Peer::LogInServer(std::string userName, std::string Password){
    
    std::string msg = userName + "," + Password;

    char * c = new char[msg.size()+1];
    strcpy(c, msg.c_str());

    Message * m = new Message(Request,msg.size()+1, this->RPC_ID,0,0,LOGIN_OP,c,this->ServerIP,this->ServerPort);

    this->sendMessage(m);

}
void Peer::RegisterServer(std::string userName, std::string Password){
    
    std::string msg = userName + "," + Password;

    char * c = new char[msg.size()+1];
    strcpy(c, msg.c_str());

    Message * m = new Message(Request,msg.size()+1, this->RPC_ID,0,0,SIGNUP_OP,c,this->ServerIP,this->ServerPort);

    this->sendMessage(m);

}

void Peer::LogOutServer(std::string userName){
     
     std::string msg = userName;

    char * c = new char[msg.size()+1];
    strcpy(c, msg.c_str());

    Message * m = new Message(Request,msg.size()+1, this->RPC_ID,0,0,LOGOUT_OP,c,this->ServerIP,this->ServerPort);

    this->sendMessage(m);

}

void Peer::UploadImageServer(std::string userName, std::string imgName){

    std::string msg = userName + "," + imgName;

    char * c = new char[msg.size()+1];
    strcpy(c, msg.c_str());

    Message * m = new Message(Request,msg.size()+1, this->RPC_ID,0,0,UPIM_OP,c,this->ServerIP,this->ServerPort);

    this->sendMessage(m);


}

void Peer::RequestImagesServer(){

    std::string msg = "";

    char * c = new char[msg.size()+1];
    strcpy(c, msg.c_str());

    Message * m = new Message(Request,msg.size()+1, this->RPC_ID,0,0,RQIM_OP,c,this->ServerIP,this->ServerPort);

    this->sendMessage(m);


}

void Peer::RequestImagePeer(std::string){

}

void Peer::ChangeFlag(){
    flag = !flag;
}
void Peer::myHandler(Message *myMSG, Peer* p)
{
    int operationID = myMSG->getOperation();
    ofstream x("output.txt");
    x << "eh!\n";
    x.close();
    if (operationID == 0) //handle login
    {

    }
    else if (operationID == 1) //handle register
    {

    }
    else if (operationID == 2) //handle upload
    {
        
    }
    else if (operationID == 3) //handle logout
    {
        
    }
    else if (operationID == 4) //handle request images
    {
        
    }
    else if (operationID == 5) //handle image transfering
    {
        MessageType type = myMSG->getMessageType();
        if (type == Request) // A peer is requesting an image from me!
        {
            // need to hide the meta data + hide an image + sending to that peer
            steg mySteg;
            std::string meta_data = "user fady with count views of 3";
            std::string hide_image = string((char*)myMSG->getMessage());

            if (mySteg.encode(meta_data, p->cover_image, hide_image) == 0)
            {
                std::string img = p->retrieveImage(p->cover_image);
                char * c = new char[img.size()+1];
                strcpy(c, img.c_str());
                myMSG->setAck();
                auto it = myMSG->getDestination();
                Message *myMsg = new Message(Reply,img.size()+1,myMSG->getrpc_Id(),0,0,myMSG->getOperation(),c,it.first, it.second);
                ofstream xxx("gamedd.jpg");
                img = base64_decode(img);
                xxx << img;
                p->sendMessage(myMsg);
            }
            else
            {
                //p->output << "Something wrong with steg!\n";
            }
        }
        else if (type == Reply) // A peer is sending an image to me!
        {
            std::string img = string((char*) myMSG->getMessage());
            img = base64_decode(img);
            
            ofstream temp("temp.txt");
            temp << "ehh!!!\n" << endl;
            temp.close();
            ofstream out("gamed.jpg");
            out << img;
            out.close();
        }
    }
}

std::string Peer::retrieveImage(const std::string &path){

    ifstream input;
    input.open(path, ios_base::binary);

    input.seekg(0, ios::end);
    long long size = input.tellg();
    input.seekg(0, ios::beg);

    char * buffer = new char[size];

    input.read(buffer, size);
    input.close();

    std::string encoded = base64_encode(reinterpret_cast<const unsigned char*>(reinterpret_cast<const unsigned char*> (buffer)), size);
    
    delete [] buffer;
    return encoded; 

}
void Peer::doOperation()
{
    Message *m;
    ofstream out("bye.txt");
    while (true)
    {
        if (this->flag)
        {
            if (this->checkMessages(m))
            {
                std::thread* handler = new std::thread(Peer::myHandler,m, this);
            }
        }
    }
}
/*void Peer::MessageHandler(Peer * p){

    std::ofstream out("bye.txt");
    Message * m;
    while(true){
        
        if(p->flag)
        {
            if(p->checkMessages(m))
            {
                out << std::string((char*)m->getMessage()) << std::endl;
                out << "ehhh!" << endl;
                //p->ChangeFlag();
                p->myHandler(m);
            }

            
        }
    }
}
*/