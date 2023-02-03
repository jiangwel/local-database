#include <iostream>
using namespace std;
namespace np {
    void cc() {
        cout << "c1c" << endl;
    }
    void dd();
}
void np::dd(){ cout << "dd"; }