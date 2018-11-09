#include <iostream>
#include "centralserver.h"
using namespace std;

int main()
{
    CentralServer myCS;
    if (myCS.login("Minawi", "bitch", "temporary"))
    {
        myCS.uploadimage("Minawi","img1");
        myCS.uploadimage("Minawi","img2");
        myCS.unparsing(myCS.Parser("Minawi"));
    }
    else
    {
        cout << "User not in the database\n";
    }

}
