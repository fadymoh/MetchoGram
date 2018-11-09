#include "Message.hpp"
#include <fstream>

template< typename T >
std::string int_to_hex( T i ){
    
    std::stringstream stream;
    stream
    << std::setfill ('0') << std::setw(sizeof(T)*2)
    << std::hex << i;
    return stream.str();
    
}

template <typename T>
void hex_to_T(std::string sz, T & pnt){
    sz = "0x" + sz;
    std::istringstream iss(sz);
    iss >> std::hex >> pnt;
}

Message::Message( MessageType type, int p_message_size, int rpc_id, int fragC, int fragT, int operation, void * p_message, std::string ip, int port){
    this->message = p_message;
    this->message_size = p_message_size;
    this->message_type = type;
    this->operation = operation;
    this->rpc_id = rpc_id;
    this->fragCount = fragC;
    this->fragTotal = fragT;


    this->destIp = ip;
    this->destPort = port;

}

Message::Message(){
}

Message::Message(char * marshalled_base64){    
    
    

    std::string encoded_message(marshalled_base64);
    std::string decod = base64_decode(encoded_message);
    this->decoded_string = decod;

    std::string decoded(decod);

    // std::string serialized = msgType + size + rpc_Id_s + fragC_s + fragT_s + op_s + destPort + ssize + Source + dSize + destIP +  message;
    //                            1        16       8         8         8         8       8         8       m        8      n         Rest

    

    this->message_type = (MessageType)(decoded[0] - 48);
    
    int Dsize;
    int Ssize; 

    hex_to_T(decoded.substr(1,16), this->message_size);
    hex_to_T(decoded.substr(17,8), this->rpc_id);
    hex_to_T(decoded.substr(25,8), this->fragCount);
    hex_to_T(decoded.substr(33,8), this->fragTotal);
    hex_to_T(decoded.substr(41,8), this->operation);
    hex_to_T(decoded.substr(49,8), this->destPort);

    hex_to_T(decoded.substr(57,8), Ssize);
    hex_to_T(decoded.substr(65 + Ssize,8), Dsize);

    this->source = decoded.substr(65,Ssize);
    this->destIp = decoded.substr(65 + Ssize + 8,Dsize);

    this->message = new char[decoded.substr(73 + Ssize + Dsize).size()+1];
    std::strcpy((char *)this->message , decoded.substr(73 + Ssize + Dsize).c_str());

}



char * Message::marshal(){    
    


    char msgType = char ((int)this->message_type + 48);             
    std::string size = int_to_hex((long long) this->message_size); 
    std::string rpc_Id_s =  int_to_hex((int)this->rpc_id);         
    std::string fragC_s =  int_to_hex((int)this->fragCount);       
    std::string fragT_s =  int_to_hex((int)this->fragTotal);        
    std::string op_s =  int_to_hex((int)this->operation);           
    std::string message((char * ) this->message);                  
    
    std::string Ssize = int_to_hex((int)this->source.size());
    std::string Ssource = this->source;

    std::string Dsize =  int_to_hex((int)this->destIp.size());
    std::string SdestIp = this->destIp;

    std::string destPortS = int_to_hex(this->destPort);



    std::string serialized = msgType + size + rpc_Id_s + fragC_s + fragT_s + op_s + destPortS + Ssize + Ssource + Dsize + SdestIp + message;

    std::string encoded = base64_encode(reinterpret_cast<const unsigned char*>(reinterpret_cast<const unsigned char*> (serialized.c_str())), serialized.size() + 1);
    
    char * encoded_cstring = new char[encoded.size()+1];
    std::strcpy(encoded_cstring, encoded.c_str());
    
    return encoded_cstring;
}

void * Message::getMessage() const{
    return this->message;
}

long long Message::getMessageSize() const{
    return this->message_size;
}

MessageType Message::getMessageType() const{
    return this->message_type;
}

int Message::getOperation () const{
    return this->operation;
}

int Message::getrpc_Id() const{
    return this->rpc_id;
}

int Message::getFragC() const{
    return this->fragCount;
}

int Message::getFragT() const{
    return this->fragTotal;
}

std::string Message::getSource()const{
    return this->source;
}

SocketAddress Message::getDest(){
    return (this->destination);
}

void Message::setMessage (void * message, long long message_size){
    this->message = message;
    this->message_size = message_size;
}

void Message::setMessageType (MessageType message_type){
    this->message_type = message_type;
}

void Message::setFrag(int fragC, int fragT){
    this->fragCount = fragC;
    this->fragTotal = fragT;
}

void Message::setOperation(int operation){
    this->operation = operation;
}

void Message::setrpc_ID(int rpc_Id){
    this->rpc_id = rpc_Id;
}

void Message::setDest(SocketAddress sock){
    this->destination = sock;
}


void Message::setSource(std::string x){
    this->source = x;
}


void Message::setAck(){
    this->message_type = Ack;
    this->message_size = 1;
    this->message = (char*)" ";


    std::string tempSrc = this->source;
    this->source = this->destIp + ":" + std::to_string(this->destPort);

}



std::pair<std::string, int> Message::getDestination(){
    return std::pair<std::string, int>(this->destIp, this->destPort);
}


void Message::setDestination(std::string x,  int p ){
    this->destIp = x;
    this->destPort = p;
}
