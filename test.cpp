494:Remove] INFO  - Remove: key=4 thread: 2347962234996181608
251:GetLeaf] INFO  - lock root id 7 thread 2347962234996181608
344:LockAndUnlock] INFO  - T WLatch p7 thread 2347962234996181608
346:LockAndUnlock] INFO  - S WLatch p7 thread 2347962234996181608
324:LockAndUnlock] INFO  - T WLatch p3 thread 2347962234996181608
326:LockAndUnlock] INFO  - S WLatch p3 thread 2347962234996181608
328:LockAndUnlock] INFO  - T WUnlatch p7 thread 2347962234996181608
330:LockAndUnlock] INFO  - S WUnlatch p7 thread 2347962234996181608
333:LockAndUnlock] INFO  - unlock root id 7 thread 2347962234996181608
344:LockAndUnlock] INFO  - T WLatch p5 thread 2347962234996181608
346:LockAndUnlock] INFO  - S WLatch p5 thread 2347962234996181608
512:RemoveEntry] INFO  - RemoveEntry: key=4 thread: 2347962234996181608 node1: 5
520:RemoveEntry] INFO  - node1 is leaf
550:RemoveEntry] INFO  - find leaf_plus
573:RemoveEntry] INFO  - sibling_page id 8
579:RemoveEntry] INFO  - leaf coalesce leaf is 5 leaf_plus is 8
593:RemoveEntry] INFO  - T WUnlatch p5 thread 2347962234996181608
596:RemoveEntry] INFO  - S WUnlatch p5 thread 2347962234996181608
512:RemoveEntry] INFO  - RemoveEntry: key=5 thread: 2347962234996181608 node1: 3
> g my-tree.dot
> d2
494:Remove] INFO  - Remove: key=2 thread: 2347962234996181608
251:GetLeaf] INFO  - lock root id 7 thread 2347962234996181608
344:LockAndUnlock] INFO  - T WLatch p7 thread 2347962234996181608
346:LockAndUnlock] INFO  - S WLatch p7 thread 2347962234996181608
344:LockAndUnlock] INFO  - T WLatch p3 thread 2347962234996181608
terminate called after throwing an instance of 'std::system_error'
  what():  Resource deadlock avoided
Aborted (core dumped)