# Conway-s_Soldier

## Conway's Soldier Game
Created by Ying Shan on 10/12/2014.

Copyright (c) 2014 Ying Shan. All rights reserved.


## Full Assignmnet: basic + exdent
Basic part: solve the game by using link list. traverse all the situations for every location of the board,
Extend part: solve the game by using tree. 

use p to represent a pointer variable for this program, beacuse I deem thats easier for people to read

## Solution
1. Create board array[7][8]
2. Initialize board. First 4 rows to zero, last 4 rows to 1. This is going to be Board0.
3. Add Node To List
4. Select next element in Node list (first time will be Board0).
5. Convert node.key to array[7][8].
6. For each element of the array (use i, j counters),
a. if it is 1,
i. Is it possible to move it up, down, right, left?
1. AddNodeToList
2. Is array[i][j] == 1 in the position to reach?
a. Then current board (node) is the end of solution.
7. Create solution stack (LIFO) starting with current board-Parent-Parent-…-Board0
8. Print solution in SDL.

## Extend
Tree is used for extend method for the Conway’s game assignment. Encode the board as sequence of 1|0 that only has character 1 and 0. Using a radix-like tree to determine if the string is duplicated or not. If not duplicate, insert the sequence in to the radix tree, otherwise, keep the tree same. Unlike regular tree, in radix tree, the stuff at each node is compared chunkof-bits by chunk-of-bits, where the quantity of bits in that chunk at that node
is the radix of the radix trie. For this question, it uses the radix sort to find a position for every single board.

http://en.wikipedia.org/wiki/Radix_tree

The runtime of the algorithm is constant = length of the string. (Binary tree height = 56) In addition, the useless leaf of the tree is cut during the searching. If a node exist in both side of a tree, the node is useless. Then delete the node the leaf of the tree (cut the leaf). That may improve the performance of the
searching. The result is much improve than the original method, tree may search way faster than link list. However, it still requires a lot of memory space to achieve the very top of the game board.
