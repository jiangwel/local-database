#include <bits/stdc++.h>
#include "t1.h"
using namespace std;
int add(int a, int b) { return a + b; }
int (*add_p)(int a, int b);

int main() {
    priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> pq;
    pq.push({1,1});
    pq.push({2,2});
    pq.push({2,3});
    while(!pq.empty()){
        cout<<pq.top().first<<' '<<pq.top().second<<endl;
        pq.pop();
    }

    return 0;
}