#include <iostream>
#include "steg.h"
using namespace std;

int main(){

    steg mySteg;

    int type;
    string t1, t2, t3;


    cout << "1-HIDE\n2-REVEAL\n0-EXIT\n";
    cin >> type;


    while(type == 1 || type == 2){

        if(type == 1){
            cout << "Enter cover Image: ";
            cin >> t1;
            cout << "Enter image to hide: ";
            cin >> t2;
            cin.ignore();
            cout << "Enter message to hide: ";
            getline(cin,t3);

            int ret = mySteg.encode(t3,t1,t2);
            if(ret == SUCCESS) cout << "ENCODING SUCCESSFUL! \n";
            else cout << "ERROR ENCODING " << ret << endl;
        }else {

            cout << "Enter cover Image: ";
            cin >> t1;
            cout << "Enter image to be revealed: ";
            cin >> t2;
            
            int ret = mySteg.decode(t1,t2,t3);
            if(ret == SUCCESS) cout << "ENCODED MESSAGE: " << t3 << endl;
            else cout << "ERROR DECODING " << ret << endl; 
        }

        cout << "1-HIDE\n2-REVEAL\n0-EXIT\n";
        cin >> type;

    }


    return 0;
}