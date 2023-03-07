#include <bits/stdc++.h>
using namespace std;

    std::mutex mtx;
int sbfun(string * &parr){
    int t=1;
    return t==1?(*parr="hello",1):(*parr="world",0);
}
class Ant{
    public:
    Ant(){
        id=10;
    }
    ~Ant(){
        cout<<"~Ant"<<endl;
    }
    int id;
};
void f1(int* a){
    int* b = new int(3);
    a=b;
    cout<<a<<' '<<*a<<endl;
}
int main() {
    int *a = new int(10);
    cout<<a<<' '<<*a<<endl;
    f1(a);
    cout<<a<<' '<<*a<<endl;
    return 0;
}