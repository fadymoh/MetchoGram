#include "centralserver.h"
#include <unordered_map>
CentralServer::CentralServer(int port):UDPSocket(port)
{
    input.open(filepath.c_str());
    if (input.fail())
    {
        cout << "Failed to open users.txt\n";
        exit(1);
    }
    init();
}
void CentralServer::init()
{
    input.seekg(0, ios::beg);
    while (!input.eof())
    {
        string username;
        string password;
        input >> username;
        input >> password;
        myUsers[username].password = password;
    }
    input.close();
}
bool CentralServer::login(const string &username, const string &pw, const string &addr)
{
    if (is_registered(username, pw))
    {
        hamada curr_user;
        curr_user.active = 1;
        curr_user.socket_address = addr;
        myUsers[username] = curr_user;
        return true;
    }
    return false;
}
bool CentralServer::signup(const string &username,const  string &pw)
{
    if (is_registered(username, pw))
        return false;
    myUsers[username].password = pw;
    output.open(filepath.c_str());
    if (output.fail()) cout << "eh?\n";
    for (auto it =myUsers.begin(); it != myUsers.end(); ++it)
        output << it->first << ' ' << it->second.password << endl;
    output.close();
    return true;
}
bool CentralServer::is_registered(const string &username, const string &pw)
{
    //check later for duplicate users not users and password
    if (myUsers.find(username) != myUsers.end())
        return myUsers[username].password == pw;
    return false;
}
string CentralServer::Parser(const string& username)
{
    html_syntax head;
    hamada current = myUsers[username];
    string out = head.header_beg + username + head.header_end + head.ip_beg 
            + current.socket_address + head.ip_end 
            + head.body_beg;
    for (int i = 0; i < current.images.size(); ++i)
    {
        out += head.list_beg;
        out += current.images[i];
        out += head.list_end;
    }
    return out + head.body_end;
}
void CentralServer::unparsing(const string &xml_syntax_input)
{
    html_syntax head;
    string username, addr, imgname;
    vector<string> imgs;
    int found = xml_syntax_input.find(head.header_end);
    username = xml_syntax_input.substr(head.header_beg.size(), found - head.header_beg.size());
    found = xml_syntax_input.find(head.ip_end);
    int end_idx = xml_syntax_input.find(head.ip_beg);
    addr = xml_syntax_input.substr( end_idx + head.ip_beg.size(), found - (end_idx + head.ip_beg.size()));
    string im;
    vector<string> images;
    found = xml_syntax_input.find(head.body_beg) + head.body_beg.size(); //pointing to start of lists
    end_idx = xml_syntax_input.find(head.body_end);

    while(found != std::string::npos)
    {
        int end = xml_syntax_input.find(head.list_end, found);
        string image_name = xml_syntax_input.substr(found + head.list_beg.length(), end - (found + head.list_beg.length()));
        found = xml_syntax_input.find(head.list_beg, found+1);
        images.push_back(image_name);
    }
    cout << username << endl;
    cout << addr << endl;
    for (int i = 0; i < images.size(); ++i)
        cout << images[i] << endl;
}
void CentralServer::uploadimage(const string &username, const string &imagename)
{
    myUsers[username].images.push_back(imagename);
    return NULL;
}
void CentralServer::doOperation(Message *request)
{
    int operationID = request.getOperation();
    std::thread* receiverThread;
    if (operationID == 0)
    {
        receiverThread = new std::(CentralServer::login, this);
    }
    else if (operationID == 1) 
    {
        receiverThread = new std::(CentralServer::signup, this);
    }
    else if (operationID == 2) 
    {
        receiverThread = new std::(CentralServer::upload, this);
    }
    else if (operationID == 3)
    {
        receiverThread = new std::(CentralServer::logout, this);
    }
    else if (operationID == 4) 
    {
        receiverThread = new std::(CentralServer::requestimages, this);
    }
}
void CentralServer::listen()
{
    Message* myMessage;
    while (1)
    {
        if (this->checkMessages(myMessage))
        {
           this->doOperation(myMessage); 
        }
    }
}
CentralServer::~CentralServer()
{

}


