#include <bits/stdc++.h>
using namespace std;

auto IndexOf(const int &key,int global_depth_) -> size_t {
  int mask = (1 << global_depth_) - 1;
  return hash<int>()(key) & mask;
}
void KeyAndIndex(int* keys, int buctke_size, int global_depth) {
    printf("global depth is %d\n", global_depth);
    for (int i = 0;keys[i]!=-1;i++) {
        printf("key is %d, index is %zu\n", keys[i],IndexOf(keys[i], global_depth));
    }
    printf("\n");
}
int main() {
    int keys[33] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
                     21,22,23,24,25,26,27,28,29,30,31,32,-1};
    KeyAndIndex(keys, 4, 0);
    KeyAndIndex(keys, 4, 1);
    KeyAndIndex(keys, 4, 2);
    KeyAndIndex(keys,4, 3);
}

