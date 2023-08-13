18:BPlusTree] INFO  - BPlusTree: leaf_max_size_=2, internal_max_size_=3
cheak point 1
71:Insert] INFO  - @Insert key is: 1 rid.GetPageId:0
71:Insert] INFO  - @Insert key is: 2 rid.GetPageId:0
296:LockAndUnlock] INFO  - WLatch p1 thread A
298:LockAndUnlock] INFO  - WLatch p1 thread A success
336:InsertParent] INFO  - WUnlatch p1 thread A //A插入2成功
71:Insert] INFO  - @Insert key is: 3 rid.GetPageId:0
284:LockAndUnlock] INFO  - WLatch p3 thread A
296:LockAndUnlock] INFO  - WLatch p2 thread A
298:LockAndUnlock] INFO  - WLatch p2 thread A success
357:InsertParent] INFO  - WUnlatch p2 thread A
404:InsertParent] INFO  - WUnlatch p3 thread A//A插入3成功
71:Insert] INFO  - @Insert key is: 1 rid.GetPageId:0
296:LockAndUnlock] INFO  - WLatch p3 thread B
298:LockAndUnlock] INFO  - WLatch p3 thread B success
296:LockAndUnlock] INFO  - WLatch p1 thread B
298:LockAndUnlock] INFO  - WLatch p1 thread B success
96:Insert] INFO  - WUnlatch p3 thread B
96:Insert] INFO  - WUnlatch p1 thread B//b插入1失败
71:Insert] INFO  - @Insert key is: 2 rid.GetPageId:0
296:LockAndUnlock] INFO  - WLatch p3 thread B
298:LockAndUnlock] INFO  - WLatch p3 thread B success
296:LockAndUnlock] INFO  - WLatch p2 thread B
298:LockAndUnlock] INFO  - WLatch p2 thread B success
96:Insert] INFO  - WUnlatch p3 thread B
96:Insert] INFO  - WUnlatch p2 thread B//B插入2失败
71:Insert] INFO  - @Insert key is: 3 rid.GetPageId:0
296:LockAndUnlock] INFO  - WLatch p3 thread B
298:LockAndUnlock] INFO  - WLatch p3 thread B success
296:LockAndUnlock] INFO  - WLatch p4 thread B
298:LockAndUnlock] INFO  - WLatch p4 thread B success
96:Insert] INFO  - WUnlatch p3 thread B
96:Insert] INFO  - WUnlatch p4 thread B//B插入3失败
71:Insert] INFO  - @Insert key is: 4 rid.GetPageId:0
71:Insert] INFO  - @Insert key is: 4 rid.GetPageId:0
296:LockAndUnlock] INFO  - WLatch p3 thread A //A锁rootid
298:LockAndUnlock] INFO  - WLatch p3 thread A success
296:LockAndUnlock] INFO  - WLatch p4 thread A
298:LockAndUnlock] INFO  - WLatch p4 thread A success
357:InsertParent] INFO  - WUnlatch p4 thread A
336:InsertParent] INFO  - WUnlatch p3 thread A//A插入4成功 解锁rootid
71:Insert] INFO  - @Insert key is: 5 rid.GetPageId:0
284:LockAndUnlock] INFO  - WLatch p7 thread A
284:LockAndUnlock] INFO  - WLatch p6 thread A
287:LockAndUnlock] INFO  - WUnlatch p7 thread A
296:LockAndUnlock] INFO  - WLatch p5 thread A
298:LockAndUnlock] INFO  - WLatch p5 thread A success
357:InsertParent] INFO  - WUnlatch p5 thread A
404:InsertParent] INFO  - WUnlatch p6 thread A//A插入5成功
296:LockAndUnlock] INFO  - WLatch p3 thread B//这个时候3还是root
//我猜测,B想fetch p3,但是p3一直被A占用,所以B一直等等等等,等到p3被释放时,p3已经不是root了
//旧的root_id已经被传入B了
298:LockAndUnlock] INFO  - WLatch p3 thread B success
296:LockAndUnlock] INFO  - WLatch p2 thread B
298:LockAndUnlock] INFO  - WLatch p2 thread B success
357:InsertParent] INFO  - WUnlatch p2 thread B
404:InsertParent] INFO  - WUnlatch p3 thread B//B插入4成功
71:Insert] INFO  - @Insert key is: 5 rid.GetPageId:0
284:LockAndUnlock] INFO  - WLatch p7 thread B
296:LockAndUnlock] INFO  - WLatch p6 thread B
298:LockAndUnlock] INFO  - WLatch p6 thread B success
296:LockAndUnlock] INFO  - WLatch p8 thread B
298:LockAndUnlock] INFO  - WLatch p8 thread B success
96:Insert] INFO  - WUnlatch p7 thread B
96:Insert] INFO  - WUnlatch p6 thread B
96:Insert] INFO  - WUnlatch p8 thread B
cheak point 2
key is: 1
35:GetValue] INFO  - GetValue: key=1
key is: 2
35:GetValue] INFO  - GetValue: key=2
key is: 3
35:GetValue] INFO  - GetValue: key=3
key is: 4
35:GetValue] INFO  - GetValue: key=4
key is: 5
35:GetValue] INFO  - GetValue: key=5
cheak point 3
154: Failure
Expected equality of these values:
  location.GetSlotNum()
    Which is: 4
  current_key
    Which is: 3
154: Failure
Expected equality of these values:
  location.GetSlotNum()
    Which is: 3
  current_key
    Which is: 4
154: Failure
Expected equality of these values:
  location.GetSlotNum()
    Which is: 4
  current_key
    Which is: 5
154: Failure
Expected equality of these values:
  location.GetSlotNum()
    Which is: 5
  current_key
    Which is: 6
159: Failure
Expected equality of these values:
  current_key
    Which is: 7
  keys.size() + 1
    Which is: 6