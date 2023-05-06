#include <bits/stdc++.h>
using namespace std;

int main(){
    cout<<"now()"<<endl;
    for(int i=0;i<4;i++){
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        cout<<time<<endl;

    }
    cout<<"time()"<<endl;
    for(int i=0;i<4;i++){
        auto time = std::time(nullptr);
        cout<<time<<endl;

    }
    return 0;
}