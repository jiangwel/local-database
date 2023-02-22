#include <bits/stdc++.h>
using namespace std;

    std::mutex mtx;
int sbfun(string * &parr){
    int t=1;
    return t==1?(*parr="hello",1):(*parr="world",0);
}
int main() {
    string* a;
    cout<<sbfun(a)<<*a<<endl;
    return 0;
}