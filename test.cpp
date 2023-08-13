:18:BPlusTree] INFO  - BPlusTree: leaf_max_size_=2, internal_max_size_=3
cheak point 1
:71:Insert] INFO  - @Insert key is: 1 rid.GetPageId:0
:71:Insert] INFO  - @Insert key is: 2 rid.GetPageId:0
:296:LockAndUnlock] INFO  - WLatch p1 thread 7091914218776077816
:335:InsertParent] INFO  - WUnlatch p1 thread 7091914218776077816
:71:Insert] INFO  - @Insert key is: 3 rid.GetPageId:0
:284:LockAndUnlock] INFO  - WLatch p3 thread 7091914218776077816
:296:LockAndUnlock] INFO  - WLatch p2 thread 7091914218776077816
:356:InsertParent] INFO  - WUnlatch p2 thread 7091914218776077816
:403:InsertParent] INFO  - WUnlatch p3 thread 7091914218776077816
:71:Insert] INFO  - @Insert key is: 4 rid.GetPageId:0
:71:Insert] INFO  - @Insert key is: 1 rid.GetPageId:0
:296:LockAndUnlock] INFO  - WLatch p3 thread 3446270584368679623
:296:LockAndUnlock] INFO  - WLatch p1 thread 3446270584368679623
:96:Insert] INFO  - WUnlatch p3 thread 3446270584368679623
:96:Insert] INFO  - WUnlatch p1 thread 3446270584368679623
:71:Insert] INFO  - @Insert key is: 2 rid.GetPageId:0
:296:LockAndUnlock] INFO  - WLatch p3 thread 3446270584368679623
:296:LockAndUnlock] INFO  - WLatch p2 thread 3446270584368679623 WLatch p3 thread 7091914218776077816
:96:Insert] INFO  - WUnlatch p3 thread 3446270584368679623
:96:Insert] INFO  - WUnlatch p2 thread 3446270584368679623
:71:Insert] INFO  - @Insert key is: 3 rid.GetPageId:0
:296:LockAndUnlock] INFO  - WLatch p3 thread 3446270584368679623
:296:LockAndUnlock] INFO  - WLatch p4 thread 3446270584368679623
:96:Insert] INFO  - WUnlatch p3 thread 3446270584368679623
:96:Insert] INFO  - WUnlatch p4 thread 3446270584368679623
:71:Insert] INFO  - @Insert key is: 4 rid.GetPageId:0
:296:LockAndUnlock] INFO  - WLatch p3 thread 3446270584368679623
:296:LockAndUnlock] INFO  - WLatch p4 thread 3446270584368679623
:356:InsertParent] INFO  - WUnlatch p4 thread 3446270584368679623

:335:InsertParent] INFO  - WUnlatch p3 thread 3446270584368679623
:71:Insert] INFO  - @Insert key is: 5 rid.GetPageId:0
:284:LockAndUnlock] INFO  - WLatch p7 thread 3446270584368679623
:284:LockAndUnlock] INFO  - WLatch p6 thread 3446270584368679623
:287:LockAndUnlock] INFO  - WUnlatch p7 thread 3446270584368679623
:296:LockAndUnlock] INFO  - WLatch p5 thread 3446270584368679623
:296:LockAndUnlock] INFO  - WLatch p2 thread 7091914218776077816
:356:InsertParent] INFO  - WUnlatch p5 thread 3446270584368679623
:403:InsertParent] INFO  - WUnlatch p6 thread 3446270584368679623
:356:InsertParent] INFO  - WUnlatch p2 thread 7091914218776077816
:403:InsertParent] INFO  - WUnlatch p3 thread 7091914218776077816
:71:Insert] INFO  - @Insert key is: 5 rid.GetPageId:0
:284:LockAndUnlock] INFO  - WLatch p7 thread 7091914218776077816
:296:LockAndUnlock] INFO  - WLatch p6 thread 7091914218776077816
:296:LockAndUnlock] INFO  - WLatch p8 thread 7091914218776077816
:96:Insert] INFO  - WUnlatch p7 thread 7091914218776077816
:96:Insert] INFO  - WUnlatch p6 thread 7091914218776077816
:96:Insert] INFO  - WUnlatch p8 thread 7091914218776077816
cheak point 2
key is: 1
:35:GetValue] INFO  - GetValue: key=1
key is: 2
:35:GetValue] INFO  - GetValue: key=2
key is: 3
:35:GetValue] INFO  - GetValue: key=3
key is: 4
:35:GetValue] INFO  - GetValue: key=4
key is: 5
:35:GetValue] INFO  - GetValue: key=5
cheak point 3
:154: Failure
Expected equality of these values:
  location.GetSlotNum()
    Which is: 4
  current_key
    Which is: 3
:154: Failure
Expected equality of these values:
  location.GetSlotNum()
    Which is: 3
  current_key
    Which is: 4
:154: Failure
Expected equality of these values:
  location.GetSlotNum()
    Which is: 4
  current_key
    Which is: 5
:154: Failure
Expected equality of these values:
  location.GetSlotNum()
    Which is: 5
  current_key
    Which is: 6
:159: Failure
Expected equality of these values:
  current_key
    Which is: 7
  keys.size() + 1
    Which is: 6