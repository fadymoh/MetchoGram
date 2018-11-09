#ifndef CENTRALSERVER_H
#define CENTRALSERVER_H

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
using namespace std;

struct hamada{
    bool active = false;
    string socket_address = "";
    vector<string> images; 
};

struct html_syntax {
    string header_beg = "<name>";
    string header_end = "</name>";
    string ip_beg = "<ip>";
    string ip_end = "</ip>";
    string body_beg = "<body>";
    string body_end = "</body>";
    string list_beg = "<li>";
    string list_end = "</li>";
};
class CentralServer
{
    public:
        CentralServer();
        ~CentralServer();
        bool login(const string&, const string&, const string&);
        bool signup(const string&, const string&);
        void uploadimage(const string&, const string&, const string&);
        void unparsing(string);
        string requestimages();
        void logout();
        void init();
        bool is_registered(const string&, const string&);
        string Parser(const string&, const string&);
    private:
        string filepath = "users.txt";
        ifstream input;
        ofstream output;
        unordered_map <string, hamada> myUsers;
};

#endif // CENTRALSERVER_H
