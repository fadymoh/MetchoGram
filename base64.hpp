//
//  base64.hpp
//  Sockets_Last
//
//  Created by Ahmad Refaat on 9/25/18.
//  Copyright © 2018 Ahmad Refaat. All rights reserved.
//

#ifndef base64_hpp
#define base64_hpp

#include <stdio.h>

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);


#endif /* base64_hpp */
