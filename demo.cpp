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
int main() {
    Ant* ant=new Ant[5];
    cout<<ant[3].id<<endl;
    return 0;
}