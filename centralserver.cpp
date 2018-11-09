#include "centralserver.h"
#include <unordered_map>
CentralServer::CentralServer()
{

    input.open(filepath.c_str());
    if (input.is_open())
        cout << "ah?" ;
    init();
}
void CentralServer::init()
{
    input.seekg(0, ios::beg);
    while (!input.eof())
    {
        string all;
        input >> all;
        cout << all << endl;
        myUsers[all];
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
        string all = username + ',' + pw;  
        myUsers[all] = curr_user;
        return true;
    }
    return false;
}
bool CentralServer::signup(const string &username,const  string &pw)
{
    if (is_registered(username, pw))
        return false;
    string all = username + ',' + pw;
    myUsers[all];
    output.open(filepath.c_str());
    if (output.fail()) cout << "eh?\n";
    for (auto it =myUsers.begin(); it != myUsers.end(); ++it)
        output << it->first << endl;
    return true;
}
bool CentralServer::is_registered(const string &username, const string &pw)
{
    //check later for duplicate users not users and password
    return myUsers.find(username + ',' + pw) != myUsers.end();
}
string CentralServer::Parser(const string& username, const string &pw)
{
    html_syntax head;
    string all = username + ',' + pw;
    hamada current = myUsers[all];
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
void CentralServer::unparsing(string s)
{
    html_syntax head;
    string username, addr, imgname;
    vector<string> imgs;
    int found = s.find(head.header_end);
    username = s.substr(head.header_beg.size(), found - head.header_beg.size());
    found = s.find(head.ip_end);
    int end_idx = s.find(head.ip_beg);
    addr = s.substr( end_idx + head.ip_beg.size(), found - (end_idx + head.ip_beg.size()));
    string im;
    vector<string> images;
    found = s.find(head.body_beg) + head.body_beg.size(); //pointing to start of lists
    end_idx = s.find(head.body_end);
    cout << s << endl;
    while(found != std::string::npos)
    {
        int end = s.find(head.list_end, found);
        string image_name = s.substr(found + head.list_beg.length(), end - (found + head.list_beg.length()));
        found = s.find(head.list_beg, found+1);
        images.push_back(image_name);
        //cout << found << endl;
    }
    cout << username << endl;
    cout << addr << endl;
    for (int i = 0; i < images.size(); ++i)
        cout << images[i] << endl;
}
void CentralServer::uploadimage(const string &username, const string &pw, const string &imagename)
{
    string all = username + ',' + pw;
    myUsers[all].images.push_back(imagename);
}
CentralServer::~CentralServer()
{

}
