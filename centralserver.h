#ifndef CENTRALSERVER_H
#define CENTRALSERVER_H

#include <str>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include "UDPSocket.hpp"
//using namespace std;
typedef std::string str;
class CentralServer: public UDPSocket
{
    public:
        CentralServer(int);
        ~CentralServer();
  
        void listen();
        void doOperation(Message *);
    private:
        void init();
        static void receiveHandling();
      
        static bool login(const str&, const str&, const str&);
        static bool signup(const str&, const str&);
        static void uploadimage(const str&, const str&);
        static void unparsing(const str&);
        static str requestimages();
        static void logout();
        static str Parser(const str&);

        struct hamada{
            bool active = false;
            str socket_address = "";
            vector<str> images; 
            str password;
        };

        struct html_syntax {
            str header_beg = "<name>";
            str header_end = "</name>";
            str ip_beg = "<ip>";
            str ip_end = "</ip>";
            str body_beg = "<body>";
            str body_end = "</body>";
            str list_beg = "<li>";
            str list_end = "</li>";
        };
        bool is_registered(const str&, const str&);
        str filepath = "users.txt";
        ifstream input;
        ofstream output;
        // they key will be the username only
        std::unordered_map <str, hamada> myUsers;
};

#endif // CENTRALSERVER_H
