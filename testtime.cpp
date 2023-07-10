#include <bits/stdc++.h>
using namespace std;

int main(){
    list<int> a{1,2,3,4,5};
    list<int> b;
    int mid = 3;
    b.splice(b.begin(), a, a.begin(), next(a.begin(), mid));
    for(auto i : a){
        cout << i << " ";
    }
}