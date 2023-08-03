#include "bits/stdc++.h"

template <typename... Args>
void LaunchParallelTest(uint64_t num_threads, Args &&...args) {
  std::vector<std::thread> thread_group;

  // Launch a group of threads
  for (uint64_t thread_itr = 0; thread_itr < num_threads; ++thread_itr) {
    thread_group.push_back(std::thread(args..., thread_itr));
  }

  // Join the threads with the main thread
  // 等待所有线程结束
  for (uint64_t thread_itr = 0; thread_itr < num_threads; ++thread_itr) {
    thread_group[thread_itr].join();
  }
}
void InsertHelper(std::vector<int> *tree, const std::vector<int> &keys,
                  __attribute__((unused)) uint64_t thread_itr = 0) {
  for (auto key : keys) {
    tree->push_back(key);
    std::cout<<"insert: "<<key<<std::endl;
  }
}

int main(){
    std::vector<int> keys{1,2,3,4,5,6,7,8,9,10};
    std::vector<int> tree;
    LaunchParallelTest(2, InsertHelper, &tree, keys);
    for(auto i:tree){
        std::cout<<i<<" ";
    }
}