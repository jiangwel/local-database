#include "bits/stdc++.h"
using namespace std;

int main() {
    std::mutex mtx;
    std::condition_variable cv;
    std::unique_lock<std::mutex> lck(mtx);

    std::thread t0([&] {
        cout<<"Waiting..."<<endl;
        cv.wait(lck);
        cout<<"Wait end"<<endl;

        if(!mtx.try_lock()){
            cout<<"Mutex is locked"<<endl;
        }
    });
    std::thread t1([&] {

        cv.notify_one();
        cout<<"Notified"<<endl;
    });

    t0.join();
    t1.join();

    return 0;
}