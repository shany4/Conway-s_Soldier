/**************************************************************************************************************
 * Bristol University Computer Science, C programming. Final Assignmemt
 * Conway's Soldier Game
 *
 * Created by Ying Shan on 10/12/2014.
 * Copyright (c) 2014 Ying Shan. All rights reserved.
 *
 *
 * Full Assignmnet: basic + exdent
 * Basic part: solve the game by using link list. traverse all the situations for every location of the board,
 * Extend part: solve the game by using Radix tree.
 *
 *  I use p to represent a pointer variable for this program, beacuse I deem thats easier for people to read
 *  I use a 2D array to decide the moving dirction of soldiers in the game, wihch shrink the length of the function a lot
 * **************************************************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"neillsdl2.h"

#define WIDTH 7 //width of the game board is 7
#define HEIGHT 8 //height of the game board is 8
#define STACKSIZE 200 //size of the stack is 200 (big enough to fill all the elements)
#define SOLDIER 1 //SOLDIER means there is a soldier in the specific block of the game board
#define EMPTY 0 //EMPTY means there is no soldier in the specific block of the game board

#define EMPTYBOARD_EDGE_LEFT 0
#define EMPTYBOARD_EDGE_RIGHT 6
#define EMPTYBOARD_EDGE_TOP 0
#define EMPTYBOARD_EDGE_BOTTOM 3//first 4 rows of the board, which suppose empty at the beginning

#define DIRECTION 4//left,right,up,down
#define DIMENSION 2//x,y

//direction that will use in PlayGame funttion, since the direction setting is based on a 2D space, there is two paramenters: first number represent Y-axis, second number represent X-axis 
#define UP -1,0
#define DOWN 1,0
#define LEFT 0,-1
#define RIGHT 0,1
#define Y 0 //used in the 2D array in PlayGame function that represent the first parameter in each col (y)
#define X 1 //used in the 2D array in PlayGame function that represent the second parameter in each col (x) 

#define YES 1 //when board been bassed, which means passed is true set moved = 1 
#define NOT 0 //if the the board not passed yet, which means passed is false set moved = 0
#define ACHIEVED 1 //when target is achieved, set flag = 1
#define NOT_ACHIEVED 0 //otherwise set flag = 0

//hash define numbers in SDL (include color, delay time)
#define EMPTY_COLOR 255,255,255//white
#define SOLDIER_COLOR 2,72,22//green
#define BORDER_COLOR 52,52,52//gray
#define DELAY 30//millisecond

//set link list struct that include: the array ,which is the board for the game, the *pNext pointer point to the next element in the link list, the *pParent pointer point back to the previous element in the link (use for find the path way of the game)
struct Node
{
  int Boards[HEIGHT][WIDTH];
  struct Node *pNext;
  struct Node *pParent;
};
typedef struct Node NODE;//give a names NODE for the Node struct

//set the stack
struct thestack{
  NODE *pStk[STACKSIZE];
  int top;
};
typedef struct thestack STACK;//give a names STACK for the thestack struct

//these 7 functions used to build the game board and play the game
NODE *InitialBoard(void);
void PlayGame(NODE *pBoard, int x, int y);
NODE *AllocateNode(NODE *board_new);
void PrintBoards(NODE *pHead);
int CompareList(NODE *pHead,NODE *pNew);
int CompareBoard(NODE *pHead,NODE *pNew);
void DisplayGoal(NODE *pGoal);
//these 3 fucntions used to creat a stack, which display the game solution from first step.
void InitialiseStack(STACK *pStack);
void Push(STACK *pStack, NODE *n);
NODE *Pop(STACK *pStack);
//this function used to print a memssage
void Wrong(char *message);
//this function used to free memory space
void DestroyBoard(NODE* pBoard);


//allow user inputs a target location for a tile to reach x (width) in argv[1], y (height) in argv[2]), call the PlayGame function to play the game and display the result
int main(int argc, char **argv)
{
  int x=0,y=0;
  NODE *pBoard = (NODE*)malloc(sizeof(NODE));
  if(pBoard==NULL){
    Wrong("Fail to allcoate");
  }
  if(argc > 2){
    //x and y start from 0
    x = atoi(argv[1]);
    y = atoi(argv[2]);
    if((x<EMPTYBOARD_EDGE_LEFT)||
       (x>EMPTYBOARD_EDGE_RIGHT)|| 
       (y<EMPTYBOARD_EDGE_TOP)|| 
       (y>EMPTYBOARD_EDGE_BOTTOM)){// user unable to input a target location that is not empty
       Wrong("Your target location is not within the confines of the unoccupied board\n");
    }
  }
  else{
    Wrong("You have entered an incorrect target location\n");
  }
  pBoard = InitialBoard();//build the initial board and the game is ready to play
  PlayGame(pBoard,x,y);//play the game, so that the soldier will follow the rules of the game and jump to the target location, when the target loction is found, display the solution
  DestroyBoard(pBoard);//free the memory that used by the link list
  return 0;
}


//creat the initial board with wide 7 * height 8
NODE *InitialBoard(void)
{
  int board[HEIGHT][WIDTH];
  int i,j;
  for(i=0;i<HEIGHT;i++){
    for(j=0;j<WIDTH;j++){
      if(i<HEIGHT/2){//top half of the board is empty
        board[i][j] = EMPTY;
      }
      else{//bottom half of the board has soldiers there
        board[i][j] = SOLDIER;
      }
    }
  }
  NODE *creat = (NODE*)malloc(sizeof(NODE));//allocate a space for initial board
  if(creat==NULL){
    Wrong("Fail to allcoate\n");
  }
  for(i=0; i<HEIGHT; i++){
    for(j=0; j<WIDTH; j++){
      creat -> Boards[i][j] = board[i][j];//set the initial borad that just build above to the Boards node
    }
  }
  creat -> pParent = NULL;//initial the link list, link to it's parent node and set is as NULL
  creat -> pNext = NULL;//initial the link list, link to it's next node and set it as NULL
  return creat;
}


//traverse all position of the game board, if a soldier is found in the board and the soldier is able to move, then the soldier make a step (up, donw, left, right)
void PlayGame(NODE *pBoard, int x, int y)
{
  int i=0, j=0, flag=0, d=0;
  int orient[DIRECTION][DIMENSION]={{UP},{DOWN},{LEFT},{RIGHT}};//this array represent 4 orientation in 2D space, which will use to decide the moving direction
  NODE *pHead=NULL, *pTail=NULL, *pCurrent=NULL;//creat three main node for the link list of the game
  NODE *pNew = (NODE*)malloc(sizeof(NODE));
  if(pNew==NULL){
    Wrong("Fail to allcoate\n");
  }
  pHead = pTail = pCurrent = pBoard;//assign head,tail,current node to initial board, and the game will based on the initial board
  while(pTail->Boards[y][x] != SOLDIER && pCurrent != NULL){
    for(i=0; i<HEIGHT; i++){
      for(j=0; j<WIDTH; j++){
        if((pCurrent->Boards[i][j]==SOLDIER)&&
	  (flag==NOT_ACHIEVED)){ 
          for(d=0;d<DIRECTION;d++){//d means direction,the loop goes 4 times that represent 4 directions
            //base on the rule of the game, if the soldier is able to move, it will move 2 units for each direction, therefore its necessary to make sure the the soldier will not dump to outside of the game board 
            if((i+2*orient[d][Y]>=0)&& 
               (i+2*orient[d][Y]<HEIGHT)&& 
               (j+2*orient[d][X]>=0)&&
               (j+2*orient[d][X]<WIDTH)&& 
               (flag==NOT_ACHIEVED)){
               //for each direction (horizontally and vertically), if a soldier that have another soldier on one side, yet empty on the other side, we switch the soldier side with the empty side and remove the middle soldier
              if((pCurrent->Boards[i][j]==SOLDIER)&&//this is the soldier that will move
                 (pCurrent->Boards[i+orient[d][Y]][j+orient[d][X]]==SOLDIER)&&//this is the soldier that in the middle, which will disappear after the movement
                 (pCurrent->Boards[i+2*(orient[d][Y])][j+2*(orient[d][X])]==EMPTY)){
               *pNew = *pCurrent;
                pNew->pParent = pCurrent;
                pNew->Boards[i][j]=EMPTY;
                pNew->Boards[i+orient[d][Y]][j+orient[d][X]]=EMPTY;
                pNew->Boards[i+2*(orient[d][Y])][j+2*(orient[d][X])]=SOLDIER;//soldier moved from Boards[i][j] to Boards[i+2][j] or Boards[i-2][j] or Boards[i][j+2] or Boards[i][j-2]
                //check if there is repetition situation of all boards first, and then traverse the list until the target location is found
                if(CompareList(pHead,pNew)){
                  pTail->pNext = AllocateNode(pNew);
                  pTail = pTail->pNext;
                  //if the target location is found, set flag is 1 (ACHIEVED)
                  if(pTail->Boards[y][x]==SOLDIER){
                    flag=ACHIEVED;
                  }
                }
              }
            }
          }
        }
      }
    }
    pCurrent = pCurrent->pNext;
  }
  DisplayGoal(pTail); //tail node store the game board that successfully achieved the goal of the game, use display function to display the final tail node, which will disply the final result of the solution step by step
  return;
}


//allocte a space from memory for the node and insert the game board to the node
NODE *AllocateNode(NODE *board_new )
{
  NODE *p = (NODE*)malloc(sizeof(NODE));
  if(p==NULL){
    Wrong("Fail to allcoate\n");
  }
  int i=0, j=0;
  for(i=0; i<HEIGHT; i++){
    for(j=0; j<WIDTH; j++){
      p->Boards[i][j] = board_new->Boards[i][j];
    }
  }
  p->pParent=board_new->pParent;
  p->pNext = NULL;
  return p;
}


//print game boards
void PrintBoards(NODE *pHead)
{
  int i,j;
  for(i=0;i<HEIGHT;i++){
    printf("\n");
    for(j=0;j<WIDTH;j++){
      printf("%d",pHead->Boards[i][j]);
    }
  }
  printf("\n");
  return;
}


//check if there is repetition of board (intuation) exit in the game or not
int CompareList(NODE *pHead,NODE *pNew)
{
    do{
      if(CompareBoard(pHead,pNew) == 0)
        return 0;// same return 0
    }
    while((pHead = pHead->pNext) != NULL);
    return 1;//diff. return 1 
}


//compare two board
int CompareBoard(NODE *pHead,NODE *pNew)
{
  int i=0, j=0;
  for (i=0; i<HEIGHT; i++){
    for(j=0; j<WIDTH; j++){
      if(pHead->Boards[i][j]!=pNew->Boards[i][j]){
        return 1;// if two boards are different return 1
      }
    }
  }
  return 0;//if two board are same return 0 
}


//display the final goal of the game, use the stack in order to display the result in order (backward),use SDL to make it looks beautiful
void DisplayGoal(NODE *pGoal)
{
  STACK stack;
  InitialiseStack(&stack);
  while(pGoal != NULL){
    Push(&stack, pGoal);
    pGoal = pGoal->pParent;
  }
  /*
  PrintBoards(Pop(&stack));
  while(stack.top > 0){
    PrintBoards(Pop(&stack));
  }
  */
    //SDL based on Neill's code  
    NODE *p = (NODE*)malloc(sizeof(NODE));
    if(p==NULL){
    Wrong("Fail to allcoate\n");
    }
    SDL_Simplewin sw;
    SDL_Rect rectangle;
    rectangle.h = WHEIGHT/HEIGHT;//WHEIGHT is the height of the screen 
    rectangle.w = WWIDTH/WIDTH;//WWIDTH is the width of screen
    Neill_SDL_Init(&sw);
    while(stack.top >=0){
    p=Pop(&stack);
    for(int y=0; y <HEIGHT; y++){
    rectangle.y = WHEIGHT/HEIGHT * y;
    for(int x=0; x < WIDTH; x++){
    rectangle.x = WWIDTH/WIDTH* x;
    if(p->Boards[y][x]==EMPTY){
    Neill_SDL_SetDrawColour(&sw, EMPTY_COLOR);
    SDL_RenderFillRect(sw.renderer, &rectangle);
    Neill_SDL_SetDrawColour(&sw, BORDER_COLOR);
    }
    if(p->Boards[y][x]==SOLDIER){
    Neill_SDL_SetDrawColour(&sw, SOLDIER_COLOR);
    SDL_RenderFillRect(sw.renderer, &rectangle);
    Neill_SDL_SetDrawColour(&sw, BORDER_COLOR);
    }
    SDL_RenderDrawRect(sw.renderer, &rectangle);
    SDL_Delay(DELAY);
    }
    }
    SDL_RenderPresent(sw.renderer);
    SDL_UpdateWindowSurface(sw.win);
    Neill_SDL_Events(&sw);
    }
    atexit(SDL_Quit);
    return;
}


//intialise the stack
void InitialiseStack(STACK *pStack)
{
  pStack->top = 0;
  return;
}


//push items into the stack
void Push(STACK *pStack, NODE *n)
{
  //Make sure stack doesnt overflow
  pStack->pStk[pStack->top] = n;
  pStack->top = pStack->top + 1;
  return;
}


//pop items out of stack
NODE *Pop(STACK *pStack)
{
  //Can't pop empty stack (underflow)
  pStack->top = pStack->top - 1;
  return pStack->pStk[pStack->top];
}


//print wrong message and exit program
void Wrong (char *message)
{
  printf("%s", message);
  exit(EXIT_FAILURE);
  return;
}


//delete node that used by link list 
void DestroyBoard(NODE* pBoard){
  // Use a loop to free the board to avoid stack overflow
  while (pBoard != NULL){
    NODE *temp = pBoard->pNext;
    free(pBoard);
    pBoard = temp;
  }
  return;
}
