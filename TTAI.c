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
char isWinner(char ** board, char player);
Move * nextMove(char ** board, char player);
int max(char ** board, Move * optimalMove, char player, int depth);
int min(char ** board, Move * optimalMove, char player, int depth);
char getOpponentMark(char player);
char ** createBoardFromInput(char * inputBoard);
void freeBoard(char ** board);
void AIvsAI();



int main(int argc, char ** argv) {
    if(argc != 4) {
        printf("Invalid number of arguments passed!\n");
        return EXIT_FAILURE;
    }
    //printf("board: %s, player: %s, playerChar: %c\n", argv[1], argv[2], argv[2][0]);
    char ** board = createBoardFromInput(argv[1]);
    if(board == NULL) {
        printf("Invalid board data inputted!\n");
        return EXIT_FAILURE;
    }
    char player = argv[2][0];
    int checkWinner = strcmp(argv[3], "-w");
    int findMove = strcmp(argv[3], "-m");
    printf("findMove: %i, checkWinner:%i\n", findMove, checkWinner);
    if (checkWinner == 0) {
        char win = isWinner(board, player);
        printf("isWinner: %i\n", win == player);
    } else if (findMove == 0) {
        //printBoard(board);
        Move * mv = nextMove(board, player);
        if(mv != NULL) {
            doMove(board, mv->i, mv->j, player);
            char win = isWinner(board, player);
            printf("(%i,%i), isWinner: %i\n", mv->i, mv->j, win == player);
            free(mv);
        } else 
            printf("no good move\n");
    } else {
        printf("error: wrong flag passed\n");
    }
    //printBoard(board);
    freeBoard(board);
    //AIvsAI();
    return EXIT_SUCCESS;
}
void AIvsAI() {
    char ** board = createEmptyBoard();//createBoardFromInput("xxx---ooo");
    printBoard(board);
    int x = 2;
    int y = 2;
    //doMove(board,x,y,'x');
//    doMove(board,2,1,'o');
    //printf("Winner: %c\n", isWinner(board,2,1));
    
    int q = 0;
    for(q = 0; q < 5; q++) {
        printf("player x move\n");
//        printBoard(board);
        char player = 'x';
        Move * mv = nextMove(board, player);
        if(mv != NULL) {
            printf("suggestion: (%i, %i)\n", mv->i, mv->j);
            doMove(board, mv->i, mv->j, player);
            printBoard(board);
            free(mv);
        } else {
            printf("no good move\n");
        }

        printf("===================\nplayer o move\n");

  //      printBoard(board);
        char player1 = 'o';
        Move * mv1 = nextMove(board, player1);
        if(mv1 != NULL) {
            printf("suggestion: (%i, %i)\n", mv1->i, mv1->j);
            doMove(board, mv1->i, mv1->j, player1);
            printBoard(board);
            free(mv1);
        } else {
            printf("no good move\n");
        }
    }
    freeBoard(board);

}
char ** createBoardFromInput(char * inputBoard) {
   if(strlen(inputBoard) != BOARD_SIZE * BOARD_SIZE) {
       return NULL;
   }
   char ** outputBoard = (char **)malloc(sizeof(char *) * BOARD_SIZE);
   int i,j,k=0;
   for(i = 0; i < BOARD_SIZE; i++) {
       outputBoard[i] = (char *)malloc(sizeof(char) * BOARD_SIZE);
   }

   for(i = 0; i < BOARD_SIZE; i++) {
       for(j = 0; j < BOARD_SIZE; j++) { 
           char c = tolower(inputBoard[k]);
           if(c != 'x' && c != 'o' && c != '-')
               return NULL;
           outputBoard[i][j] = c;
           k++;
       }
   }
   return outputBoard;
}

void freeBoard(char ** board) {
    int i;
    for(i = 0; i < BOARD_SIZE; i++) {
        free(board[i]);
    }
    free(board);
}

int max(char ** board, Move * optimalMove, char player, int depth) {
    int max = INT_MIN;
    int won = 0;
    int i,j;
    depth++;
    for(i = 0; i < BOARD_SIZE; i++) {
        for(j = 0; j < BOARD_SIZE; j++) {
            int value = 0;
            //available move
            if(board[i][j] == '-') {
                doMove(board, i, j, player);
                char isWin = isWinner(board, player);
                if(isWin == player) {
                    value = 10 - depth;
                    won = 1;

                    //printf("it is a '%c' player win\n", player);
                } else if(isWin == '-') {
                    //draw
                    //printf("2its a draw\n");
                    //printBoard(board);
                    value = 0;
                } else {
                    value = min(board, optimalMove, getOpponentMark(player), depth);
                }
                undoMove(board, i, j);
              
                if(value > max) {
                    //printf("changing coords from (%i,%i) to (%i,%i) at depth: %i, max:%i, value:%i \n", optimalMove->i,optimalMove->j,i,j,depth,max,value);
                    max = value;
                    if(depth==1) {
                        optimalMove -> i = i;
                        optimalMove -> j = j;
                    }
                }
                if(depth == 1) {
                    //printf("coor: (%i,%i) max: %i\n", i,j,max);
                }
                //printf("move: (%i,%i), value:%i, max: %i, player:%c, depth: %i\n",i,j,value,max,player,depth);
                //if(won == 1) break;

            }
        }
        //if(won == 1) break;
    }
    return max;
}
int min(char ** board, Move * optimalMove, char player, int depth) {
    int min = INT_MAX;
    int won = 0;
    int i,j;
    depth++;
    for(i = 0; i < BOARD_SIZE; i++) {
        for(j = 0; j < BOARD_SIZE; j++) {
            int value = 0;
            //available move
            if(board[i][j] == '-') {
                doMove(board, i, j, player);
                char isWin = isWinner(board, player);
                if(isWin == player) {
                    //value = -10 - depth;
                    value = depth - 10;
                    won = 1;
                    //printf("it is a '%c' player win\n", player);
                } else if(isWin == '-') {
                    //draw 
                    //printf("1its a draw\n");
                    //printBoard(board);
                    value = 0;
                } else {
                    value = max(board, optimalMove, getOpponentMark(player), depth);
                }
                undoMove(board, i, j);
                if(value < min) {
                    min = value;
                }
                //printf("move: (%i,%i), value:%i, min: %i, player:%c, depth: %i\n",i,j,value,min,player,depth);

                //if(won == 1) break;
            }
        }
        //if(won == 1) break;
    }
    return min;
}
char getOpponentMark(char player) {
    if(player == 'x')
        return 'o';
    if(player == 'o')
        return 'x';
    return 'E';
}
Move * nextMove(char ** board, char player) {
    Move * optimalMove = (Move *)malloc(sizeof(Move *));
    optimalMove -> i = -1;
    optimalMove -> j = -1;

    int initialDepth = 0;
    max(board, optimalMove, player, initialDepth);
    if(optimalMove -> i == -1 || optimalMove -> j == -1) {
        free(optimalMove);
        return NULL;
    }
    return optimalMove;

}
void printBoard (char ** board) {
    int i,j;
    for(i = 0; i < BOARD_SIZE; i++) {
        for(j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
//    printf("%c\n", board[2][1]);
}
char ** createCustomBoard(){
    char ** board = (char **)malloc(sizeof(char *) * BOARD_SIZE);
     int i;
    for(i = 0; i < BOARD_SIZE; i++) {
        board[i] = (char *)malloc(sizeof(char) * BOARD_SIZE);
    }

    board[0][0] = 'o';  board[0][1] = 'x';  board[0][2] = 'o';
    board[1][0] = 'x';  board[1][1] = 'x';  board[1][2] = 'o';
    board[2][0] = '-';  board[2][1] = '-';  board[2][2] = '-';
 
    return board;
}
char ** createEmptyBoard() {
    char ** board = (char **)malloc(sizeof(char *) * BOARD_SIZE);
    int i,j;
    for(i = 0; i < BOARD_SIZE; i++) {
        board[i] = (char *)malloc(sizeof(char) * BOARD_SIZE);
    }
    int k = 0;
    for(i = 0; i < BOARD_SIZE; i++) {
        for(j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = '-';
            k++;
        }
    }
    return board;
}

void doMove(char ** board, int i, int j, char player) {
    board[i][j] = player;
}
void undoMove(char ** board, int i, int j) {
    board[i][j] = '-';
}
// returns the character at x,y (so x or o) if the 
// character at that position has won the game with the move
//
// returns '-' if it's a draw
// return ' ' if no winner
char isWinner(char ** board, char player) {
    //printBoard(board);
    int col = 0, row = 0, ldiag = 0, rdiag = 0;
    int i;
    // check diagonals
    for(i = 0; i < BOARD_SIZE; i++) {
        //printf("ldiag pos (%i, %i): %c\n", i,i, board[i][i]);
        if(board[i][i] == player)
            ldiag++;

        //printf("rdiag pos (%i, %i): %c\n", i,BOARD_SIZE-i-1, board[i][BOARD_SIZE-i-1]);
        if(board[i][BOARD_SIZE - i - 1] == player)
            rdiag++;
    }
    if(ldiag == BOARD_SIZE || rdiag == BOARD_SIZE) {
        return player;
    }
    int allOccupied = 0;
    int j,k;
    // check rows/cols
    for(j = 0; j < BOARD_SIZE; j++) {
        col = 0;
        row = 0;
        for(k = 0; k < BOARD_SIZE; k++) {
            if(board[j][k] != '-')
                allOccupied++;
            //printf("col pos (%i, %i): %c\n", j,k, board[j][k]);
            if(board[j][k] == player) {
                col++;
                if (col == BOARD_SIZE) {
                    // we found a winner
                    return player;
                }
            }
            //printf("row pos (%i, %i): %c\n", k,j, board[k][j]);
            if(board[k][j] == player) {
                row++;
                if(row == BOARD_SIZE) {
                    // we have a winner
                    return player;
                }
            }
        }

        //printf("done!\n");
    }
    //printf("col: %i, row:%i, rdiag:%i, ldiag:%i, player: %c\n", col,row,rdiag,ldiag,player);
    if(col == BOARD_SIZE || row == BOARD_SIZE) 
        return player;
    // it's a draw
    if(allOccupied == BOARD_SIZE * BOARD_SIZE)
        return '-';
    
    // no winner yet
    return ' ';
}
