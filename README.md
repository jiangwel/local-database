### Based on C++ Support for a Basic SQL Database Kernel Implementation

### Project Description:
This project is an open project from CMU15445. The goal is to develop a database kernel supporting basic SQL using C++. It is divided into four labs and requires passing 96 test cases.

### Project Contributions:
+ Implemented Buffer Pool Management: 
  + Implemented the LRU-K algorithm as the page replacement strategy for the Buffer Pool. 
+ Implemented B+ tree Index: 
  + Used the crabbing protocol to achieve concurrent operations on the B+ tree.
+ Implemented a Volcano model-based query executor with TopN optimization: 
  + Aggregation executor, Nested loop join executor, Nested index join executor, Sort executor.
+ Implemented a Lock Manager based on two-phase locking: 
  + Supported three isolation levels: RR, RC, RU.
  + Supported five lock types at both table and row granularities.
  + Implemented deadlock detection and handling.
   
  

# Result

<table style="table-layout:fixed; width:100%;">
<tr>
<th style="width:25%;">lab1(100/100)</th>
<th style="width:25%;">lab2(95/100)</th>  
<th style="width:25%;">lab3(100/100)</th>
<th style="width:25%;">lab4(100/100)</th>
</tr>

<tr>
<td style="width:25%;">
<img src="https://github.com/jiangwel/bustub/assets/84135487/8d4b5fde-656f-4fe1-8b38-13513ee405e2" style="max-width:100%;">
</td>
<td style="width:25%;">
<img src="https://github.com/jiangwel/bustub/assets/84135487/0ee0505b-b17a-4580-a056-55d94b8c57b8" style="max-width:20%;">
<img src="https://github.com/jiangwel/bustub/assets/84135487/8dd069c2-aa17-4e85-bdb5-ec78258163fb" style="max-width:20%;">
</td>
<td style="width:20%;">
<img src="https://github.com/jiangwel/bustub/assets/84135487/7e191ae2-c3bc-4073-aa6e-142cf2b92862" style="max-width:100%;">
</td>
<td style="width:35%;">
<img src="https://github.com/jiangwel/bustub/assets/84135487/68b0ac2f-5a22-4589-8768-29d6929d10cd" style="max-width:100%;">
</td>
</tr>
</table>

