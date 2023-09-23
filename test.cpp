38: [==========] Running 1 test from 1 test suite.
38: [----------] Global test environment set-up.
38: [----------] 1 test from BPlusTreeConcurrentTest
38: [ RUN      ] BPlusTreeConcurrentTest.InsertTest2
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:19:BPlusTree] INFO  - BPlusTree: leaf_max_size_=3, internal_max_size_=5
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 2 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 1 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=2 value=0 page id=1 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=1 value=0 page id=2 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 4 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 3 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 2 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p2 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p2 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=4 value=0 page id=2 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:128:Insert] INFO  - T WUnlatch p2 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:131:Insert] INFO  - S WUnlatch p2 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:137:Insert] INFO  - unlock root id 2 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 2 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 6 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p2 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p2 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p3 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p3 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=3 value=0 page id=2 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=3 value=3 page1 id=2 page2 id=3 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:385:InsertParent] INFO  - T WUnlatch p2 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:387:InsertParent] INFO  - S WUnlatch p2 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:391:InsertParent] INFO  - unlock root id 4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:385:InsertParent] INFO  - T WUnlatch p3 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:387:InsertParent] INFO  - S WUnlatch p3 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 5 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p3 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p3 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p5 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p5 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=6 value=0 page id=3 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=4 value=5 page1 id=3 page2 id=5 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=4 value=5 page id=4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:447:InsertParent] INFO  - unlock root id 4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p3 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p3 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p5 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p5 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p5 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 8 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p5 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p6 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p6 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=5 value=0 page id=5 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=5 value=6 page1 id=5 page2 id=6 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=5 value=6 page id=4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:447:InsertParent] INFO  - unlock root id 4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p5 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p5 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p6 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p6 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p6 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p6 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 7 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p7 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p7 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=8 value=0 page id=6 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=6 value=7 page1 id=6 page2 id=7 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=6 value=7 page id=4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:447:InsertParent] INFO  - unlock root id 4 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p6 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p6 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p7 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p7 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p7 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p7 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 10 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p8 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p8 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=7 value=0 page id=7 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=7 value=8 page1 id=7 page2 id=8 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:404:InsertParent] INFO  - parent is full
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=7 value=8 page id=4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=5 value=9 page1 id=4 page2 id=9 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:385:InsertParent] INFO  - T WUnlatch p4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:387:InsertParent] INFO  - S WUnlatch p4 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:391:InsertParent] INFO  - unlock root id 10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:385:InsertParent] INFO  - T WUnlatch p7 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:387:InsertParent] INFO  - S WUnlatch p7 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:385:InsertParent] INFO  - T WUnlatch p8 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p9 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:387:InsertParent] INFO  - S WUnlatch p8 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p9 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 9 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p8 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p8 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p9 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p11 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p11 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=10 value=0 page id=8 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=8 value=11 page1 id=8 page2 id=11 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=8 value=11 page id=9 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p9 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p9 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p9 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p8 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p8 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p11 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p11 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p11 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 12 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p11 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p9 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p12 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p12 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=9 value=0 page id=11 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=9 value=12 page1 id=11 page2 id=12 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=9 value=12 page id=9 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p9 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p9 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p9 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p11 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p11 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p12 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p12 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p12 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p12 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 11 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p13 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p13 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=12 value=0 page id=12 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=10 value=13 page1 id=12 page2 id=13 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:404:InsertParent] INFO  - parent is full
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=10 value=13 page id=9 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=8 value=14 page1 id=9 page2 id=14 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=8 value=14 page id=10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:447:InsertParent] INFO  - unlock root id 10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p9 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p9 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p12 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p14 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p12 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p14 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p13 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p13 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 14 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p13 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p13 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p14 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p15 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p15 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=11 value=0 page id=13 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=11 value=15 page1 id=13 page2 id=15 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=11 value=15 page id=14 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p14 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p14 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p14 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p13 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p13 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p15 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p15 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p15 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 13 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p15 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p14 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p16 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p16 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=14 value=0 page id=15 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=12 value=16 page1 id=15 page2 id=16 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=12 value=16 page id=14 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p14 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p14 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p14 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p15 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p16 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p15 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p16 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p16 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p16 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 16 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p17 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p17 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=13 value=0 page id=16 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=13 value=17 page1 id=16 page2 id=17 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:404:InsertParent] INFO  - parent is full
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=13 value=17 page id=14 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=11 value=18 page1 id=14 page2 id=18 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=11 value=18 page id=10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:447:InsertParent] INFO  - unlock root id 10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p14 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p14 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p16 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p16 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p18 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p17 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p18 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p17 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 15 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 10 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p17 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p17 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p18 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p19 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p19 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=16 value=0 page id=17 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=14 value=19 page1 id=17 page2 id=19 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=14 value=19 page id=18 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p18 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p18 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p18 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p17 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p17 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p10 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p19 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 10[...(truncated)...]:InsertParent] INFO  - S WUnlatch p134 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p134 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p133 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p133 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p136 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p136 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p136 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 96 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p136 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p137 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p137 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=93 value=0 page id=136 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=93 value=137 page1 id=136 page2 id=137 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=93 value=137 page id=134 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p134 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p134 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p134 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p134 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p136 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p137 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p136 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p137 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p137 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p137 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 95 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p138 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p138 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=96 value=0 page id=137 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=94 value=138 page1 id=137 page2 id=138 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:404:InsertParent] INFO  - parent is full
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=94 value=138 page id=134 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=92 value=139 page1 id=134 page2 id=139 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=92 value=139 page id=135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p134 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p134 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p137 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p139 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p137 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p139 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p138 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p138 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 98 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p138 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p138 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p140 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p140 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=95 value=0 page id=138 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=95 value=140 page1 id=138 page2 id=140 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=95 value=140 page id=139 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p139 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p139 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p139 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p139 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p138 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p138 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p140 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p140 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p140 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p140 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 97 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p141 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p141 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=98 value=0 page id=140 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=96 value=141 page1 id=140 page2 id=141 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=96 value=141 page id=139 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p139 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p139 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p140 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p140 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p139 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p141 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p139 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p141 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p141 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 100 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p141 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p142 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p142 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=97 value=0 page id=141 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=97 value=142 page1 id=141 page2 id=142 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:404:InsertParent] INFO  - parent is full
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=97 value=142 page id=139 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=95 value=143 page1 id=139 page2 id=143 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=95 value=143 page id=135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p139 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p139 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p143 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p141 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p143 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p141 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p142 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p142 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 99 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p142 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p142 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p144 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p144 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=100 value=0 page id=142 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p143 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=98 value=144 page1 id=142 page2 id=144 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=98 value=144 page id=143 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p143 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p143 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p143 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p142 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p142 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p144 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p144 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p135 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 102 rid.GetPageId:0 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p144 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p144 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p122 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:333:LockAndUnlock] INFO  - T WUnlatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:335:LockAndUnlock] INFO  - S WUnlatch p82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:339:LockAndUnlock] INFO  - unlock root id 82 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p145 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p145 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=99 value=0 page id=144 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p135 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=99 value=145 page1 id=144 page2 id=145 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p143 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=99 value=145 page id=143 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p143 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p143 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p143 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p144 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p145 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p144 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:441:InsertParent] INFO  - T WUnlatch p145 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:443:InsertParent] INFO  - S WUnlatch p145 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p145 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:76:Insert] INFO  - @Insert key is: 101 rid.GetPageId:0 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:290:GetLeaf] INFO  - success get leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:254:GetLeaf] INFO  - lock root id 82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:294:GetLeaf] INFO  - check if key exists in leaf
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:148:Insert] INFO  - T get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:325:LockAndUnlock] INFO  - S WLatch p82 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:150:Insert] INFO  - S get leaf2 page thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:323:LockAndUnlock] INFO  - T WLatch p122 thread B
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:152:Insert] INFO  - T WLatch p146 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:154:Insert] INFO  - S WLatch p146 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=102 value=0 page id=145 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:352:InsertParent] INFO  - InsertParent: key=100 value=146 page1 id=145 page2 id=146 thread A
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:404:InsertParent] INFO  - parent is full
38: 2023-09-23 07:58:00 [autograder/source/bustub/src/storage/index/b_plus_tree.cpp:183:InsertNode] INFO  - InsertNode: key=100 value=146 page id=143 thread A
38: AddressSanitizer:DEADLYSIGNAL