#ifndef CENTRALSERVER_H
#define CENTRALSERVER_H

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
using namespace std;

class CentralServer
{
    public:
        CentralServer();
        ~CentralServer();
        bool login(const string&, const string&, const string&);
        bool signup(const string&, const string&);
        void uploadimage(const string&, const string&);
        void unparsing(const string&);
        string requestimages();
        void logout();
        string Parser(const string&);
    private:
        void init();

        struct hamada{
            bool active = false;
            string socket_address = "";
            vector<string> images; 
            string password;
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
        bool is_registered(const string&, const string&);
        string filepath = "users.txt";
        ifstream input;
        ofstream output;
        // they key will be the username only
        unordered_map <string, hamada> myUsers;
};

#endif // CENTRALSERVER_H
