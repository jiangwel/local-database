Inert value drectly, index_of_key=0,key is 1
Inert value drectly, index_of_key=0,key is 2
Inert value ,doublue dir, in buckte 0 ,new global_depth is 1,key is 3
Inert value ,doublue dir, in buckte 0 ,new global_depth is 2,key is 4
Inert value ,separate bucket, index_of_key=1,key is 5

1.add element 1 in bucket 0
2.add element 2 in bucket 0
3.add element 3 double kir
4.add element 4 double dir
5.add element 5 separate bucket //插入5时桶1没有满，不应该分桶

2023-03-17 12:00:58 [workspaces/bustub-jiangwei/src/container/hash/extendible_hash_table.cpp:114:Insert] INFO  - Inert value drectly, index_of_key=0,key is 0x602000000b70
2023-03-17 12:00:58 [workspaces/bustub-jiangwei/src/container/hash/extendible_hash_table.cpp:114:Insert] INFO  - Inert value drectly, index_of_key=0,key is 0x602000000b74
2023-03-17 12:00:58 [workspaces/bustub-jiangwei/src/container/hash/extendible_hash_table.cpp:107:Insert] INFO  - Inert value ,doublue dir, in buckte 0 ,new global_depth is 1,key is 0x602000000b78
2023-03-17 12:00:58 [workspaces/bustub-jiangwei/src/container/hash/extendible_hash_table.cpp:95:Insert] INFO  - update value in bucket 0,key is 0x7ffe462e4550
2023-03-17 12:00:58 [workspaces/bustub-jiangwei/src/container/hash/extendible_hash_table.cpp:95:Insert] INFO  - update value in bucket 0,key is 0x7ffe462e4560