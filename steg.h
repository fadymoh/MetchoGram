#include <iostream>
#include <string>
#include <fstream>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */


using namespace std;


const static int SUCCESS = 0;

const static int ERROR_RETREIVING_IMAGE = 1;
const static int ERROR_RETREIVING_MESSAGE = 2;
const static int ERROR_READING_MESSAGE_FILE = 3;

const static int ERROR_HIDING_IMAGE = 4;
const static int ERROR_HIDING_MESSAGE = 5;
const static int ERROR_CREATING_MESSAGE_FILE = 6;



class steg
{
private:

    const string message_file_name = "hi.txt";
    const string passphrase = "DISTRO";

public:

    int encode(const string& message, const  string& cover_image, const string& to_hide_image);
    int decode(const string& cover_image, const string& revealed_image, string& hidden_message);

};


// function decode
// Parameters:
//  * cover_image (const): String => name of cover image where everything is hidden 
//  * revealed_image (const): String => name of file to reveal image into
//  * hidden_message : String => returns the hidden message

int steg::decode(const string& cover_image, const string& revealed_image, string& hidden_message){

    string revealImageCommand = "steghide extract -sf " + cover_image + " -xf " + revealed_image + " -p " + passphrase + " -f";
    int revealedImage = system(revealImageCommand.c_str());

    if(revealedImage) return ERROR_RETREIVING_IMAGE;
    

    string revealMessageCommand = "steghide extract -sf " + revealed_image + " -xf " + message_file_name + " -p " + passphrase + " -f";
    int revealedMessage = system(revealImageCommand.c_str());

    if(revealedMessage) return ERROR_RETREIVING_MESSAGE;

    ifstream in(message_file_name.c_str());

    if(!in.is_open()) return ERROR_READING_MESSAGE_FILE; 

    in.seekg(0, std::ios::end);   
    hidden_message.reserve(in.tellg());
    in.seekg(0, std::ios::beg);

    hidden_message.assign((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());

    return SUCCESS;

}


// function encode
// Encodes a message into an image => hides that image in another image.
//
// Parameters:
//  * message (const): String => message to be hidden
//  * cover_image (const): String => name of cover image where everything is hidden 
//  * to_hide_image (const): String => name of image to be hidden


int steg::encode(const string& message, const string& cover_image, const string& to_hide_image){

    
    ofstream out(message_file_name.c_str());
    
    if(!out.is_open()) return ERROR_CREATING_MESSAGE_FILE;

    out << message;
    out.close();


    string hideMessageCommand = "steghide embed -cf " + to_hide_image + " -ef " + message_file_name + " -p " + passphrase + " -Z";
    int hideMessage = system(hideMessageCommand.c_str());

    if(hideMessage) return ERROR_HIDING_MESSAGE;


    string hideImageCommand = "steghide embed -cf " + cover_image + " -ef " + to_hide_image + " -p " + passphrase + " -Z";
    int hideImage = system(hideImageCommand.c_str());

    if(hideImage) return ERROR_HIDING_IMAGE;


    return SUCCESS;
}
