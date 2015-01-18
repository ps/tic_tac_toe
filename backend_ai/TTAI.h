#ifndef TTAI_H
#define TTAI_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

// board indexing board[2][1] ---> row 2, col 1

// 3 by 3 board

struct Move {
    int i;
    int j;
};

typedef struct Move Move;

const int BOARD_SIZE = 3;
void printBoard (char ** board);
char ** createEmptyBoard();
char ** createCustomBoard();
void doMove(char ** board, int i, int j, char player);
void undoMove(char ** board, int i, int j);
char getWinner(char ** board);
Move * nextMove(char ** board, char player);
int max(char ** board, Move * optimalMove, char player, int depth);
int min(char ** board, Move * optimalMove, char player, int depth);
char getOpponentMark(char player);
char ** createBoardFromInput(char * inputBoard);
void freeBoard(char ** board);
void AIvsAI();
void printInputMsg();

#endif
