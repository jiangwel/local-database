#include <bits/stdc++.h>
using namespace std;

    std::mutex mtx;
int main() {
    std::unique_lock<std::mutex> lck(mtx, std::defer_lock);
    if(lck.try_lock())
        cout << "locked" << endl;
    else
        cout << "not locked" << endl;
    return 0;
}