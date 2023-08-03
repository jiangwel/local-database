#include "bits/stdc++.h"
using namespace std;

int main(){
    const int a=1; //常量
    // 引用,语法错误,所以不能使用引用要使用常量引用
    // int &b=a;
    const int &c=a; //常量引用
    int const &d=a; //这是什么?
    cout<<"d "<<d<<endl;
    cout<<"c "<<c<<endl;

}