#include "centralserver.h"
#include <unordered_map>
CentralServer::CentralServer(int port):UDPSocket(port)
{
    input.open(filepath.c_str());
    if (input.fail())
    {
        std::cout << "Failed to open users.txt\n";
        exit(1);
    }
    init();
}
void CentralServer::init()
{
    input.seekg(0, std::ios::beg);
    while (!input.eof())
    {
        str username;
        str password;
        input >> username;
        if (username == "") continue;
            input >> password;
        std::cout << username << ' ' << password << '\n';
        myUsers[username].password = password;
    }
    input.close();
}
void CentralServer::login(const str &username, const str &pw, Message* req, CentralServer* myCS)
{
    bool flag = false;
    if (myCS->is_registered(username, pw))
    {
        hamada curr_user;
        curr_user.active = 1;
        curr_user.socket_address = req->getSource();
        myCS->myUsers[username] = curr_user;
        flag = true;
    }
    str m = (flag) ? "Successful" : "Incorrect Username or Password";
    char * c = new char[m.size()+1];
    strcpy(c, m.c_str());
    req->setAck();
    auto it = req->getDestination();
    Message *myMsg = new Message(Reply,m.size()+1,req->getrpc_Id(),0,0,req->getOperation(),c,it.first, it.second);
    myCS->sendMessage(myMsg);
}
void CentralServer::signup(const str &username,const  str &pw, Message* req,CentralServer* myCS)
{
    bool flag = true;
    if (myCS->user_found(username))
        flag = false;
    if (flag){
        myCS->myUsers[username].password = pw;
        myCS->output.open(myCS->filepath.c_str());
        if (myCS->output.fail()) std::cout << "eh?\n";
        for (auto it =myCS->myUsers.begin(); it != myCS->myUsers.end(); ++it)
            myCS->output << it->first << ' ' << it->second.password << '\n';
        myCS->output.close();
    }
    str m = (flag) ? "Successful": "Unsuccessful";

    char * c = new char[m.size()+1];
    strcpy(c, m.c_str());
    req->setAck();
    auto it = req->getDestination();
    Message *myMsg = new Message(Reply,m.size()+1,req->getrpc_Id(),0,0,req->getOperation(),c,it.first, it.second);
    myCS->sendMessage(myMsg); 

}
bool CentralServer::is_registered(const str &username, const str &pw)
{
    //check later for duplicate users not users and password
    if (myUsers.find(username) != myUsers.end())
        return myUsers[username].password == pw;
    return false;
}
bool CentralServer::user_found(const str &username)
{
    return myUsers.find(username) != myUsers.end();
}
void CentralServer::requestImages(const str& username, Message* req, CentralServer* myCS)
{
    html_syntax head;
    str out = head.html_beg;
    std::ofstream xx("testing.txt");
    for (auto it = myCS->myUsers.begin(); it != myCS->myUsers.end(); ++it)
    {
        if (it->second.active){
            hamada current = it->second;
            out += head.user_beg + head.header_beg + it->first + head.header_end + head.ip_beg 
                    + current.socket_address + head.ip_end 
                    + head.body_beg;
            for (int i = 0; i < current.images.size(); ++i)
            {
                out += head.list_beg;
                out += current.images[i];
                out += head.list_end;
            }
            out += head.body_end;
            out += head.user_end;
        }
    }
    out += head.html_end;
    xx << out << '\n';
    char * c = new char[out.size()+1];
    strcpy(c, out.c_str());
    req->setAck();
    auto it = req->getDestination();
    myCS->unparsing(out);
    Message *myMsg = new Message(Reply,out.size()+1,req->getrpc_Id(),0,0,req->getOperation(),c,it.first, it.second);
    myCS->sendMessage(myMsg);

}

void CentralServer::unparsing(const str &xml_syntax_input)
{
    html_syntax head;
    int current_point = 0;
    std::unordered_map<str, hamada> myMap;
    std::ofstream temp_out("zeft.txt");
    int end_pointer = xml_syntax_input.find(head.user_end);
    while (end_pointer != str::npos)
    {
        int finder = xml_syntax_input.find(head.header_end, current_point+1);
        str username, addr;
        current_point = xml_syntax_input.find(head.header_beg, current_point+1) + head.header_beg.length();
        username = xml_syntax_input.substr(current_point, finder - current_point);
        hamada current_user;
        current_point = xml_syntax_input.find(head.ip_beg, current_point +1);
        finder = xml_syntax_input.find(head.ip_end, finder+1);
        current_point += head.ip_beg.length();
        addr = xml_syntax_input.substr(current_point, finder - current_point);
        current_user.socket_address = addr;
        int end_of_body = xml_syntax_input.find(head.body_end, finder + 1);
        finder = xml_syntax_input.find(head.body_beg, finder+1) + head.body_beg.length();
        while (finder != str::npos && (finder < (end_of_body)))
        {
            int end = xml_syntax_input.find(head.list_end, finder);
            if (end == str::npos) break;
            str image_name = xml_syntax_input.substr(finder + head.list_beg.length(), end - (finder+head.list_beg.length()));
            finder = xml_syntax_input.find(head.list_beg, finder+1);
            current_user.images.push_back(image_name);
        }
        end_pointer = xml_syntax_input.find(head.user_end,end_pointer+1);
        myMap[username] = current_user;
    }
    for (auto it = myMap.begin(); it != myMap.end(); ++it)
    {
        temp_out << it->first << ' ';
        for (int i = 0; i < it->second.images.size(); ++i)
            temp_out << it->second.images[i] << ' ';
        temp_out << '\n';
    }
}

void CentralServer::uploadimage(const str &username, const str &imagename,  Message* req, CentralServer *myCS)
{
    myCS->myUsers[username].images.push_back(imagename);
    str m = "Successful";

    char * c = new char[m.size()+1];
    strcpy(c, m.c_str());
    req->setAck();
    auto it = req->getDestination();
    Message *myMsg = new Message(Reply,m.size()+1,req->getrpc_Id(),0,0,req->getOperation(),c,it.first, it.second);
    myCS->sendMessage(myMsg); 
}
void CentralServer::doOperation(Message *request)
{
    int operationID = request->getOperation();
    std::thread* receiverThread;
    str input = str((char*)request->getMessage());
    int found = input.find(',');
    str first = input.substr(0, found);
    str second, third;
    if (found != str::npos) //two argument
    {
        second = input.substr(found+1);
    }
    if (operationID == 0)
    {
        receiverThread = new std::thread(CentralServer::login, first, second ,request, this); //done
    }
    else if (operationID == 1) 
    {
        receiverThread = new std::thread(CentralServer::signup, first, second, request, this); //done
    }
    else if (operationID == 2) 
    {
        receiverThread = new std::thread(CentralServer::uploadimage, first, second, request,this); //done
    }
    else if (operationID == 3)
    {
        receiverThread = new std::thread(CentralServer::logout, first, request,this); //done
    }
    else if (operationID == 4) 
    {
        receiverThread = new std::thread(CentralServer::requestImages, first, request,this); //done
    }
}
void CentralServer::listen()
{
    Message* myMessage;
    while (1)
    {
        if (this->checkMessages(myMessage))
        {
            std::cout << "Message itself "<<(char*) myMessage->getMessage() << '\n';
            std::cout << "operationID " << myMessage->getOperation() << '\n';
           this->doOperation(myMessage); 
        }
    }
}
void CentralServer::logout(const str& username, Message* req, CentralServer *myCS)
{
   myCS->myUsers[username].active = 0;
   str m = "Successful";
   char * c = new char[m.size()+1];
   strcpy(c, m.c_str());
   req->setAck();
   auto it = req->getDestination();
   Message *myMsg = new Message(Reply,m.size()+1,req->getrpc_Id(),0,0,req->getOperation(),c,it.first, it.second);
   myCS->sendMessage(myMsg); 
}
CentralServer::~CentralServer()
{

}