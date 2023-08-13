18:BPlusTree] INFO  - BPlusTree: leaf_max_size_=3, internal_max_size_=5
71:Insert] INFO  - @Insert key is: 2 rid.GetPageId:0
71:Insert] INFO  - @Insert key is: 1 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p1 thread A
71:Insert] INFO  - @Insert key is: 4 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p1 thread B
113:Insert] INFO  - WUnlatch p1 thread A
71:Insert] INFO  - @Insert key is: 3 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p1 thread A
343:InsertParent] INFO  - WUnlatch p1 thread B
71:Insert] INFO  - @Insert key is: 6 rid.GetPageId:0
113:Insert] INFO  - WUnlatch p1 thread A
289:LockAndUnlock] INFO  - WLatch p3 thread B
71:Insert] INFO  - @Insert key is: 5 rid.GetPageId:0
303:LockAndUnlock] INFO  - WLatch p2 thread B
289:LockAndUnlock] INFO  - WLatch p3 thread A
364:InsertParent] INFO  - WUnlatch p2 thread B
411:InsertParent] INFO  - WUnlatch p3 thread B
71:Insert] INFO  - @Insert key is: 8 rid.GetPageId:0
303:LockAndUnlock] INFO  - WLatch p4 thread A
289:LockAndUnlock] INFO  - WLatch p3 thread B
364:InsertParent] INFO  - WUnlatch p4 thread A
411:InsertParent] INFO  - WUnlatch p3 thread A
71:Insert] INFO  - @Insert key is: 7 rid.GetPageId:0
303:LockAndUnlock] INFO  - WLatch p5 thread B
289:LockAndUnlock] INFO  - WLatch p3 thread A
364:InsertParent] INFO  - WUnlatch p5 thread B
411:InsertParent] INFO  - WUnlatch p3 thread B
71:Insert] INFO  - @Insert key is: 10 rid.GetPageId:0
303:LockAndUnlock] INFO  - WLatch p6 thread A
303:LockAndUnlock] INFO  - WLatch p3 thread B
364:InsertParent] INFO  - WUnlatch p6 thread A
343:InsertParent] INFO  - WUnlatch p3 thread A
71:Insert] INFO  - @Insert key is: 9 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p4 thread B
289:LockAndUnlock] INFO  - WLatch p9 thread A
292:LockAndUnlock] INFO  - WUnlatch p3 thread B
289:LockAndUnlock] INFO  - WLatch p8 thread A
113:Insert] INFO  - WUnlatch p4 thread B
292:LockAndUnlock] INFO  - WUnlatch p9 thread A
71:Insert] INFO  - @Insert key is: 12 rid.GetPageId:0
303:LockAndUnlock] INFO  - WLatch p7 thread A
289:LockAndUnlock] INFO  - WLatch p9 thread B
364:InsertParent] INFO  - WUnlatch p7 thread A
289:LockAndUnlock] INFO  - WLatch p8 thread B
411:InsertParent] INFO  - WUnlatch p8 thread A
71:Insert] INFO  - @Insert key is: 11 rid.GetPageId:0
292:LockAndUnlock] INFO  - WUnlatch p9 thread B
289:LockAndUnlock] INFO  - WLatch p9 thread A
303:LockAndUnlock] INFO  - WLatch p10 thread B
289:LockAndUnlock] INFO  - WLatch p8 thread A
364:InsertParent] INFO  - WUnlatch p10 thread B
411:InsertParent] INFO  - WUnlatch p8 thread B
292:LockAndUnlock] INFO  - WUnlatch p9 thread A
71:Insert] INFO  - @Insert key is: 14 rid.GetPageId:0
303:LockAndUnlock] INFO  - WLatch p11 thread A
289:LockAndUnlock] INFO  - WLatch p9 thread B
303:LockAndUnlock] INFO  - WLatch p8 thread B
364:InsertParent] INFO  - WUnlatch p11 thread A
364:InsertParent] INFO  - WUnlatch p8 thread A
38: AddressSanitizer:DEADLYSIGNAL
289:LockAndUnlock] INFO  - WLatch p7 thread B
38: =================================================================
292:LockAndUnlock] INFO  - WUnlatch p9 thread B
292:LockAndUnlock] INFO  - WUnlatch p8 thread B
113:Insert] INFO  - WUnlatch p7 thread B
71:Insert] INFO  - @Insert key is: 16 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p9 thread B
289:LockAndUnlock] INFO  - WLatch p13 thread B
292:LockAndUnlock] INFO  - WUnlatch p9 thread B
38: ==2348==ERROR: AddressSanitizer: SEGV on unknown address (pc 0x0000005aa680 bp 0x7f3b16afcb70 sp 0x7f3b16afc2e0 T2)
303:LockAndUnlock] INFO  - WLatch p12 thread B
364:InsertParent] INFO  - WUnlatch p12 thread B
411:InsertParent] INFO  - WUnlatch p13 thread B
38: ==2348==The signal is caused by a READ memory access.
71:Insert] INFO  - @Insert key is: 18 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p9 thread B
38: ==2348==Hint: this fault was caused by a dereference of a high value address (see register values below).  Disassemble the provided pc to learn which register was used.
289:LockAndUnlock] INFO  - WLatch p13 thread B
292:LockAndUnlock] INFO  - WUnlatch p9 thread B
303:LockAndUnlock] INFO  - WLatch p14 thread B
364:InsertParent] INFO  - WUnlatch p14 thread B
411:InsertParent] INFO  - WUnlatch p13 thread B
71:Insert] INFO  - @Insert key is: 20 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p9 thread B
303:LockAndUnlock] INFO  - WLatch p13 thread B
303:LockAndUnlock] INFO  - WLatch p15 thread B
364:InsertParent] INFO  - WUnlatch p15 thread B
364:InsertParent] INFO  - WUnlatch p13 thread B
411:InsertParent] INFO  - WUnlatch p9 thread B
71:Insert] INFO  - @Insert key is: 22 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p9 thread B
289:LockAndUnlock] INFO  - WLatch p17 thread B
292:LockAndUnlock] INFO  - WUnlatch p9 thread B
303:LockAndUnlock] INFO  - WLatch p16 thread B
364:InsertParent] INFO  - WUnlatch p16 thread B
411:InsertParent] INFO  - WUnlatch p17 thread B
71:Insert] INFO  - @Insert key is: 24 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p9 thread B
289:LockAndUnlock] INFO  - WLatch p17 thread B
292:LockAndUnlock] INFO  - WUnlatch p9 thread B
303:LockAndUnlock] INFO  - WLatch p18 thread B
364:InsertParent] INFO  - WUnlatch p18 thread B
411:InsertParent] INFO  - WUnlatch p17 thread B
71:Insert] INFO  - @Insert key is: 26 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p9 thread B
303:LockAndUnlock] INFO  - WLatch p17 thread B
303:LockAndUnlock] INFO  - WLatch p19 thread B
364:InsertParent] INFO  - WUnlatch p19 thread B
364:InsertParent] INFO  - WUnlatch p17 thread B
411:InsertParent] INFO  - WUnlatch p9 thread B
71:Insert] INFO  - @Insert key is: 28 rid.GetPageId:0
303:LockAndUnlock] INFO  - WLatch p9 thread B
289:LockAndUnlock] INFO  - WLatch p21 thread B
292:LockAndUnlock] INFO  - WUnlatch p9 thread B
303:LockAndUnlock] INFO  - WLatch p20 thread B
364:InsertParent] INFO  - WUnlatch p20 thread B
411:InsertParent] INFO  - WUnlatch p21 thread B
71:Insert] INFO  - @Insert key is: 30 rid.GetPageId:0
303:LockAndUnlock] INFO  - WLatch p9 thread B
289:LockAndUnlock] INFO  - WLatch p21 thread B
292:LockAndUnlock] INFO  - WUnlatch p9 thread B
303:LockAndUnlock] INFO  - WLatch p22 thread B
364:InsertParent] INFO  - WUnlatch p22 thread B
411:InsertParent] INFO  - WUnlatch p21 thread B
71:Insert] INFO  - @Insert key is: 32 rid.GetPageId:0
303:LockAndUnlock] INFO  - WLatch p9 thread B
303:LockAndUnlock] INFO  - WLatch p21 thread B
303:LockAndUnlock] INFO  - WLatch p23 thread B
364:InsertParent] INFO  - WUnlatch p23 thread B
364:InsertParent] INFO  - WUnlatch p21 thread B
343:InsertParent] INFO  - WUnlatch p9 thread B
71:Insert] INFO  - @Insert key is: 34 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p26 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p25 thread B
292:LockAndUnlock] INFO  - WUnlatch p26 thread B
303:LockAndUnlock] INFO  - WLatch p24 thread B
364:InsertParent] INFO  - WUnlatch p24 thread B
411:InsertParent] INFO  - WUnlatch p25 thread B
71:Insert] INFO  - @Insert key is: 36 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p26 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p25 thread B
292:LockAndUnlock] INFO  - WUnlatch p26 thread B
303:LockAndUnlock] INFO  - WLatch p28 thread B
364:InsertParent] INFO  - WUnlatch p28 thread B
411:InsertParent] INFO  - WUnlatch p25 thread B
71:Insert] INFO  - @Insert key is: 38 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p26 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p25 thread B
303:LockAndUnlock] INFO  - WLatch p29 thread B
364:InsertParent] INFO  - WUnlatch p29 thread B
364:InsertParent] INFO  - WUnlatch p25 thread B
411:InsertParent] INFO  - WUnlatch p26 thread B
71:Insert] INFO  - @Insert key is: 40 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p26 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p31 thread B
292:LockAndUnlock] INFO  - WUnlatch p26 thread B
303:LockAndUnlock] INFO  - WLatch p30 thread B
364:InsertParent] INFO  - WUnlatch p30 thread B
411:InsertParent] INFO  - WUnlatch p31 thread B
71:Insert] INFO  - @Insert key is: 42 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p26 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p31 thread B
292:LockAndUnlock] INFO  - WUnlatch p26 thread B
303:LockAndUnlock] INFO  - WLatch p32 thread B
364:InsertParent] INFO  - WUnlatch p32 thread B
411:InsertParent] INFO  - WUnlatch p31 thread B
71:Insert] INFO  - @Insert key is: 44 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p26 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p31 thread B
303:LockAndUnlock] INFO  - WLatch p33 thread B
364:InsertParent] INFO  - WUnlatch p33 thread B
364:InsertParent] INFO  - WUnlatch p31 thread B
411:InsertParent] INFO  - WUnlatch p26 thread B
71:Insert] INFO  - @Insert key is: 46 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p26 thread B
289:LockAndUnlock] INFO  - WLatch p35 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
292:LockAndUnlock] INFO  - WUnlatch p26 thread B
303:LockAndUnlock] INFO  - WLatch p34 thread B
364:InsertParent] INFO  - WUnlatch p34 thread B
411:InsertParent] INFO  - WUnlatch p35 thread B
71:Insert] INFO  - @Insert key is: 48 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p26 thread B
289:LockAndUnlock] INFO  - WLatch p35 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
292:LockAndUnlock] INFO  - WUnlatch p26 thread B
303:LockAndUnlock] INFO  - WLatch p36 thread B
364:InsertParent] INFO  - WUnlatch p36 thread B
411:InsertParent] INFO  - WUnlatch p35 thread B
71:Insert] INFO  - @Insert key is: 50 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p26 thread B
303:LockAndUnlock] INFO  - WLatch p35 thread B
303:LockAndUnlock] INFO  - WLatch p37 thread B
364:InsertParent] INFO  - WUnlatch p37 thread B
364:InsertParent] INFO  - WUnlatch p35 thread B
364:InsertParent] INFO  - WUnlatch p26 thread B
411:InsertParent] INFO  - WUnlatch p27 thread B
71:Insert] INFO  - @Insert key is: 52 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p40 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p39 thread B
292:LockAndUnlock] INFO  - WUnlatch p40 thread B
303:LockAndUnlock] INFO  - WLatch p38 thread B
364:InsertParent] INFO  - WUnlatch p38 thread B
411:InsertParent] INFO  - WUnlatch p39 thread B
71:Insert] INFO  - @Insert key is: 54 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p40 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p39 thread B
292:LockAndUnlock] INFO  - WUnlatch p40 thread B
303:LockAndUnlock] INFO  - WLatch p41 thread B
364:InsertParent] INFO  - WUnlatch p41 thread B
411:InsertParent] INFO  - WUnlatch p39 thread B
71:Insert] INFO  - @Insert key is: 56 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p40 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p39 thread B
303:LockAndUnlock] INFO  - WLatch p42 thread B
364:InsertParent] INFO  - WUnlatch p42 thread B
364:InsertParent] INFO  - WUnlatch p39 thread B
411:InsertParent] INFO  - WUnlatch p40 thread B
71:Insert] INFO  - @Insert key is: 58 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p40 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p44 thread B
292:LockAndUnlock] INFO  - WUnlatch p40 thread B
303:LockAndUnlock] INFO  - WLatch p43 thread B
364:InsertParent] INFO  - WUnlatch p43 thread B
411:InsertParent] INFO  - WUnlatch p44 thread B
71:Insert] INFO  - @Insert key is: 60 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p40 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p44 thread B
292:LockAndUnlock] INFO  - WUnlatch p40 thread B
303:LockAndUnlock] INFO  - WLatch p45 thread B
364:InsertParent] INFO  - WUnlatch p45 thread B
411:InsertParent] INFO  - WUnlatch p44 thread B
71:Insert] INFO  - @Insert key is: 62 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p40 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p44 thread B
303:LockAndUnlock] INFO  - WLatch p46 thread B
364:InsertParent] INFO  - WUnlatch p46 thread B
364:InsertParent] INFO  - WUnlatch p44 thread B
411:InsertParent] INFO  - WUnlatch p40 thread B
71:Insert] INFO  - @Insert key is: 64 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p40 thread B
289:LockAndUnlock] INFO  - WLatch p48 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
292:LockAndUnlock] INFO  - WUnlatch p40 thread B
303:LockAndUnlock] INFO  - WLatch p47 thread B
364:InsertParent] INFO  - WUnlatch p47 thread B
411:InsertParent] INFO  - WUnlatch p48 thread B
71:Insert] INFO  - @Insert key is: 66 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p40 thread B
289:LockAndUnlock] INFO  - WLatch p48 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
292:LockAndUnlock] INFO  - WUnlatch p40 thread B
303:LockAndUnlock] INFO  - WLatch p49 thread B
364:InsertParent] INFO  - WUnlatch p49 thread B
411:InsertParent] INFO  - WUnlatch p48 thread B
71:Insert] INFO  - @Insert key is: 68 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p40 thread B
303:LockAndUnlock] INFO  - WLatch p48 thread B
303:LockAndUnlock] INFO  - WLatch p50 thread B
364:InsertParent] INFO  - WUnlatch p50 thread B
364:InsertParent] INFO  - WUnlatch p48 thread B
364:InsertParent] INFO  - WUnlatch p40 thread B
411:InsertParent] INFO  - WUnlatch p27 thread B
71:Insert] INFO  - @Insert key is: 70 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p53 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p52 thread B
292:LockAndUnlock] INFO  - WUnlatch p53 thread B
303:LockAndUnlock] INFO  - WLatch p51 thread B
364:InsertParent] INFO  - WUnlatch p51 thread B
411:InsertParent] INFO  - WUnlatch p52 thread B
71:Insert] INFO  - @Insert key is: 72 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p53 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p52 thread B
292:LockAndUnlock] INFO  - WUnlatch p53 thread B
303:LockAndUnlock] INFO  - WLatch p54 thread B
364:InsertParent] INFO  - WUnlatch p54 thread B
411:InsertParent] INFO  - WUnlatch p52 thread B
71:Insert] INFO  - @Insert key is: 74 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p53 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
303:LockAndUnlock] INFO  - WLatch p52 thread B
303:LockAndUnlock] INFO  - WLatch p55 thread B
364:InsertParent] INFO  - WUnlatch p55 thread B
364:InsertParent] INFO  - WUnlatch p52 thread B
411:InsertParent] INFO  - WUnlatch p53 thread B
71:Insert] INFO  - @Insert key is: 76 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p53 thread B
292:LockAndUnlock] INFO  - WUnlatch p27 thread B
289:LockAndUnlock] INFO  - WLatch p57 thread B
292:LockAndUnlock] INFO  - WUnlatch p53 thread B
303:LockAndUnlock] INFO  - WLatch p56 thread B
364:InsertParent] INFO  - WUnlatch p56 thread B
38: 2023-08-11 06:50:14 [autograder/source[...(truncated)...]AndUnlock] INFO  - WLatch p331 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p322 thread B
303:LockAndUnlock] INFO  - WLatch p332 thread B
364:InsertParent] INFO  - WUnlatch p332 thread B
411:InsertParent] INFO  - WUnlatch p331 thread B
71:Insert] INFO  - @Insert key is: 446 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p322 thread B
303:LockAndUnlock] INFO  - WLatch p331 thread B
303:LockAndUnlock] INFO  - WLatch p333 thread B
364:InsertParent] INFO  - WUnlatch p333 thread B
364:InsertParent] INFO  - WUnlatch p331 thread B
364:InsertParent] INFO  - WUnlatch p322 thread B
411:InsertParent] INFO  - WUnlatch p323 thread B
71:Insert] INFO  - @Insert key is: 448 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p336 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p335 thread B
292:LockAndUnlock] INFO  - WUnlatch p336 thread B
303:LockAndUnlock] INFO  - WLatch p334 thread B
364:InsertParent] INFO  - WUnlatch p334 thread B
411:InsertParent] INFO  - WUnlatch p335 thread B
71:Insert] INFO  - @Insert key is: 450 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p336 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p335 thread B
292:LockAndUnlock] INFO  - WUnlatch p336 thread B
303:LockAndUnlock] INFO  - WLatch p337 thread B
364:InsertParent] INFO  - WUnlatch p337 thread B
411:InsertParent] INFO  - WUnlatch p335 thread B
71:Insert] INFO  - @Insert key is: 452 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p336 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
303:LockAndUnlock] INFO  - WLatch p335 thread B
303:LockAndUnlock] INFO  - WLatch p338 thread B
364:InsertParent] INFO  - WUnlatch p338 thread B
364:InsertParent] INFO  - WUnlatch p335 thread B
411:InsertParent] INFO  - WUnlatch p336 thread B
71:Insert] INFO  - @Insert key is: 454 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p336 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p340 thread B
292:LockAndUnlock] INFO  - WUnlatch p336 thread B
303:LockAndUnlock] INFO  - WLatch p339 thread B
364:InsertParent] INFO  - WUnlatch p339 thread B
411:InsertParent] INFO  - WUnlatch p340 thread B
71:Insert] INFO  - @Insert key is: 456 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p336 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p340 thread B
292:LockAndUnlock] INFO  - WUnlatch p336 thread B
303:LockAndUnlock] INFO  - WLatch p341 thread B
364:InsertParent] INFO  - WUnlatch p341 thread B
411:InsertParent] INFO  - WUnlatch p340 thread B
71:Insert] INFO  - @Insert key is: 458 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p336 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
303:LockAndUnlock] INFO  - WLatch p340 thread B
303:LockAndUnlock] INFO  - WLatch p342 thread B
364:InsertParent] INFO  - WUnlatch p342 thread B
364:InsertParent] INFO  - WUnlatch p340 thread B
411:InsertParent] INFO  - WUnlatch p344 thread B
71:Insert] INFO  - @Insert key is: 460 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p336 thread B
289:LockAndUnlock] INFO  - WLatch p344 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p336 thread B
303:LockAndUnlock] INFO  - WLatch p343 thread B
364:InsertParent] INFO  - WUnlatch p343 thread B
411:InsertParent] INFO  - WUnlatch p344 thread B
71:Insert] INFO  - @Insert key is: 462 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p336 thread B
289:LockAndUnlock] INFO  - WLatch p344 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p336 thread B
303:LockAndUnlock] INFO  - WLatch p345 thread B
364:InsertParent] INFO  - WUnlatch p345 thread B
411:InsertParent] INFO  - WUnlatch p344 thread B
71:Insert] INFO  - @Insert key is: 464 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
289:LockAndUnlock] INFO  - WLatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p336 thread B
303:LockAndUnlock] INFO  - WLatch p344 thread B
303:LockAndUnlock] INFO  - WLatch p346 thread B
364:InsertParent] INFO  - WUnlatch p346 thread B
364:InsertParent] INFO  - WUnlatch p344 thread B
364:InsertParent] INFO  - WUnlatch p336 thread B
411:InsertParent] INFO  - WUnlatch p323 thread B
71:Insert] INFO  - @Insert key is: 466 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p349 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p348 thread B
292:LockAndUnlock] INFO  - WUnlatch p349 thread B
303:LockAndUnlock] INFO  - WLatch p347 thread B
364:InsertParent] INFO  - WUnlatch p347 thread B
411:InsertParent] INFO  - WUnlatch p348 thread B
71:Insert] INFO  - @Insert key is: 468 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p349 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p348 thread B
292:LockAndUnlock] INFO  - WUnlatch p349 thread B
303:LockAndUnlock] INFO  - WLatch p350 thread B
364:InsertParent] INFO  - WUnlatch p350 thread B
411:InsertParent] INFO  - WUnlatch p348 thread B
71:Insert] INFO  - @Insert key is: 470 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p349 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
303:LockAndUnlock] INFO  - WLatch p348 thread B
303:LockAndUnlock] INFO  - WLatch p351 thread B
364:InsertParent] INFO  - WUnlatch p351 thread B
364:InsertParent] INFO  - WUnlatch p348 thread B
411:InsertParent] INFO  - WUnlatch p349 thread B
71:Insert] INFO  - @Insert key is: 472 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p349 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p353 thread B
292:LockAndUnlock] INFO  - WUnlatch p349 thread B
303:LockAndUnlock] INFO  - WLatch p352 thread B
364:InsertParent] INFO  - WUnlatch p352 thread B
411:InsertParent] INFO  - WUnlatch p353 thread B
71:Insert] INFO  - @Insert key is: 474 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p349 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p353 thread B
292:LockAndUnlock] INFO  - WUnlatch p349 thread B
303:LockAndUnlock] INFO  - WLatch p354 thread B
364:InsertParent] INFO  - WUnlatch p354 thread B
411:InsertParent] INFO  - WUnlatch p353 thread B
71:Insert] INFO  - @Insert key is: 476 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p323 thread B
289:LockAndUnlock] INFO  - WLatch p349 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
303:LockAndUnlock] INFO  - WLatch p353 thread B
303:LockAndUnlock] INFO  - WLatch p355 thread B
364:InsertParent] INFO  - WUnlatch p355 thread B
364:InsertParent] INFO  - WUnlatch p353 thread B
411:InsertParent] INFO  - WUnlatch p357 thread B
71:Insert] INFO  - @Insert key is: 478 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p323 thread B
303:LockAndUnlock] INFO  - WLatch p349 thread B
289:LockAndUnlock] INFO  - WLatch p357 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p349 thread B
303:LockAndUnlock] INFO  - WLatch p356 thread B
364:InsertParent] INFO  - WUnlatch p356 thread B
411:InsertParent] INFO  - WUnlatch p357 thread B
71:Insert] INFO  - @Insert key is: 480 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p323 thread B
303:LockAndUnlock] INFO  - WLatch p349 thread B
289:LockAndUnlock] INFO  - WLatch p357 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
292:LockAndUnlock] INFO  - WUnlatch p242 thread B
292:LockAndUnlock] INFO  - WUnlatch p323 thread B
292:LockAndUnlock] INFO  - WUnlatch p349 thread B
303:LockAndUnlock] INFO  - WLatch p358 thread B
364:InsertParent] INFO  - WUnlatch p358 thread B
411:InsertParent] INFO  - WUnlatch p357 thread B
71:Insert] INFO  - @Insert key is: 482 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
303:LockAndUnlock] INFO  - WLatch p242 thread B
303:LockAndUnlock] INFO  - WLatch p323 thread B
303:LockAndUnlock] INFO  - WLatch p349 thread B
303:LockAndUnlock] INFO  - WLatch p357 thread B
303:LockAndUnlock] INFO  - WLatch p359 thread B
364:InsertParent] INFO  - WUnlatch p359 thread B
364:InsertParent] INFO  - WUnlatch p357 thread B
364:InsertParent] INFO  - WUnlatch p349 thread B
364:InsertParent] INFO  - WUnlatch p323 thread B
364:InsertParent] INFO  - WUnlatch p242 thread B
411:InsertParent] INFO  - WUnlatch p243 thread B
71:Insert] INFO  - @Insert key is: 484 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p364 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p363 thread B
292:LockAndUnlock] INFO  - WUnlatch p364 thread B
289:LockAndUnlock] INFO  - WLatch p362 thread B
292:LockAndUnlock] INFO  - WUnlatch p363 thread B
289:LockAndUnlock] INFO  - WLatch p361 thread B
292:LockAndUnlock] INFO  - WUnlatch p362 thread B
303:LockAndUnlock] INFO  - WLatch p360 thread B
364:InsertParent] INFO  - WUnlatch p360 thread B
411:InsertParent] INFO  - WUnlatch p361 thread B
71:Insert] INFO  - @Insert key is: 486 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p364 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p363 thread B
292:LockAndUnlock] INFO  - WUnlatch p364 thread B
289:LockAndUnlock] INFO  - WLatch p362 thread B
292:LockAndUnlock] INFO  - WUnlatch p363 thread B
289:LockAndUnlock] INFO  - WLatch p361 thread B
292:LockAndUnlock] INFO  - WUnlatch p362 thread B
303:LockAndUnlock] INFO  - WLatch p365 thread B
364:InsertParent] INFO  - WUnlatch p365 thread B
411:InsertParent] INFO  - WUnlatch p361 thread B
71:Insert] INFO  - @Insert key is: 488 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p364 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p363 thread B
292:LockAndUnlock] INFO  - WUnlatch p364 thread B
289:LockAndUnlock] INFO  - WLatch p362 thread B
292:LockAndUnlock] INFO  - WUnlatch p363 thread B
303:LockAndUnlock] INFO  - WLatch p361 thread B
303:LockAndUnlock] INFO  - WLatch p366 thread B
364:InsertParent] INFO  - WUnlatch p366 thread B
364:InsertParent] INFO  - WUnlatch p361 thread B
411:InsertParent] INFO  - WUnlatch p362 thread B
71:Insert] INFO  - @Insert key is: 490 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p364 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p363 thread B
292:LockAndUnlock] INFO  - WUnlatch p364 thread B
289:LockAndUnlock] INFO  - WLatch p362 thread B
292:LockAndUnlock] INFO  - WUnlatch p363 thread B
289:LockAndUnlock] INFO  - WLatch p368 thread B
292:LockAndUnlock] INFO  - WUnlatch p362 thread B
303:LockAndUnlock] INFO  - WLatch p367 thread B
364:InsertParent] INFO  - WUnlatch p367 thread B
411:InsertParent] INFO  - WUnlatch p368 thread B
71:Insert] INFO  - @Insert key is: 492 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p364 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p363 thread B
292:LockAndUnlock] INFO  - WUnlatch p364 thread B
289:LockAndUnlock] INFO  - WLatch p362 thread B
292:LockAndUnlock] INFO  - WUnlatch p363 thread B
289:LockAndUnlock] INFO  - WLatch p368 thread B
292:LockAndUnlock] INFO  - WUnlatch p362 thread B
303:LockAndUnlock] INFO  - WLatch p369 thread B
364:InsertParent] INFO  - WUnlatch p369 thread B
411:InsertParent] INFO  - WUnlatch p368 thread B
71:Insert] INFO  - @Insert key is: 494 rid.GetPageId:0
289:LockAndUnlock] INFO  - WLatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p364 thread B
292:LockAndUnlock] INFO  - WUnlatch p243 thread B
289:LockAndUnlock] INFO  - WLatch p363 thread B
292:LockAndUnlock] INFO  - WUnlatch p364 thread B
289:LockAndUnlock] INFO  - WLatch p362 thread B
292:LockAndUnlock] INFO  - WUnlatch p363 thread B
303:LockAndUnlock] INFO  - WLatch p368 thread B
303:LockAndUnlock] INFO  - WLatch p370 thread B
38: terminate called after throwing an instance of 'std::system_error'
38:   what():  Resource deadlock avoided