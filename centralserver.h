#ifndef CENTRALSERVER_H
#define CENTRALSERVER_H

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
        void unparsing(const str&);
        static void login(const str&, const str&, Message* ,CentralServer*);
        static void signup(const str&, const str&, Message* ,CentralServer*);
        static void uploadimage(const str&, const str&, Message* ,CentralServer*);
        //static void unparsing(const str&, CentralServer*);
        static void requestImages(const str&, Message* ,CentralServer*);
        static void logout(const str&, Message*, CentralServer*);

        struct hamada{
            bool active = false;
            str socket_address = "";
            std::vector<str> images; 
            str password;
        };
        
        struct html_syntax {
            str html_beg = "<html>";
            str html_end = "</html>";
            str user_beg =  "<user>";
            str user_end = "</user>";
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
        bool user_found(const str&);
        str filepath = "users.txt";
        std::ifstream input;
        std::ofstream output;
        // they key will be the username only
        std::unordered_map <str, hamada> myUsers;
};

#endif // CENTRALSERVER_H
