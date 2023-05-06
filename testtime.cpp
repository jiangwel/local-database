// #include <iostream>
// #include <time.h>
// #include <bits/stdc++.h>
// using namespace std;

// timespec diff(timespec start, timespec end);

// int main()
// {
//     timespec time1, time2;
//     // timeval time1;
//     int temp;
//     for(int i=0;i<1;++i){
//         clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
//         cout<<i<<' '<<time1.tv_sec<<' '<<time1.tv_nsec<<endl;
//     }
//     // for (int i = 0; i< 242000000; i++)
//     //     temp+=temp;
//     // clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
//     // cout<<diff(time1,time2).tv_sec<<":"<<diff(time1,time2).tv_nsec<<endl;
//     return 0;
// }

#include <ctime>
#include <iostream>
#define ADD_ZERO_9_TIME 1000000000
using namespace std;

int main() {
    timespec ts;
    size_t time=0;
    char buff[0x80];

    //  auto [sec, nsec] = ts; // UB: structured bindings should not be used because the
                               // declaration order and data member list are unspecified
    for(int i=0;i<100;++i){
        timespec_get(&ts, TIME_UTC);
        // strftime(buff, sizeof buff, "%D %T", gmtime(&ts.tv_sec));
        // cout << ts.tv_sec << ' ' << ts.tv_nsec << '\n';
        time = ts.tv_sec*ADD_ZERO_9_TIME+ts.tv_nsec;
        cout<<time<<endl;
    }
    
}