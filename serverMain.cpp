#include <iostream>
#include "centralserver.h"
using namespace std;

int main()
{
    CentralServer myCS(8080);
    myCS.listen();
    return 0;
}
