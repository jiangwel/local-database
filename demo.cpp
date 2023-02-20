#include <bits/stdc++.h>
using namespace std;

int global_depth_ = 3;

auto IndexOf(const int& key) -> size_t {
    int mask = (1 << global_depth_) - 1;
    return hash<int>()(key) & mask;
}

int main() {
    vector<int> v{1,2,3,4};
    for(auto &it:v){
        cout<<it<<endl;
    }
    return 0;
}