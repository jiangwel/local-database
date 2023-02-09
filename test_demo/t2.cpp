#include <bits/stdc++.h>
#include "t1.h"

int add(int a, int b) { return a + b; }
int (*add_p)(int a, int b);

int main() {
    unique_ptr<unordered_map<int, string>> map_ptr(new unordered_map<int, string>);
    map_ptr->insert({1,"alpot"});
    auto string_a = map_ptr->find(1)->second;
    std::cout << string_a << std::endl;

    return 0;
}