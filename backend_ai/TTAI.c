#include "TTAI.h"

int main(int argc, char ** argv) {
    int showHelp = 0;
    if(argc > 1 && strcmp(argv[1], "-h") == 0) {
        printInputMsg();
        return EXIT_SUCCESS;
    }
    /* check number of arguments passed */
    if(argc != 4) {
        printf("{\"error\":\"Invalid number of arguments passed!\"}\n");
        /* returning success so that python script doesn't crash */
        return EXIT_SUCCESS;
    }
    
    /* initialize and check board */
    char ** board = createBoardFromInput(argv[1]);
    if(board == NULL) {
        printf("{\"error\":\"Invalid board data inputted!\"}\n");
        /* same reason as above */
        return EXIT_SUCCESS;
    }

    /* check player */
    char player = tolower(argv[2][0]);
    if(player != 'o' && player != 'x') {
        printf("{\"error\":\"Invalid player inputted!\"}\n");
        /* same reason as above */
        return EXIT_SUCCESS;
    }

    int checkWinner = strcmp(argv[3], "-w");
    int findMove = strcmp(argv[3], "-m");
    
    if (checkWinner == 0) {
        /* perform winner check */
        char win = getWinner(board);
        /*printf("stuff: %c\n", win);*/

        printf("{\"winner\": \"%c\"}\n", win);
    } else if (findMove == 0) {
        /* perform next move calculation */
        Move * mv = nextMove(board, player);
        if(mv != NULL) {
            doMove(board, mv->i, mv->j, player);
            char win = getWinner(board);
            printf("{\"move_x\": \"%i\", \"move_y\":\"%i\", \"winner\": \"%c\"}\n", mv->i, mv->j, win);
            free(mv);
        } else {
            char win = getWinner(board);
            /* no good move found */
            printf("{\"move_x\": \"-1\", \"move_y\":\"-1\", \"winner\": \"%c\"}\n", win);
        }
    } else {
        printf("{\"error\":\"Invalid flag passed!\"}\n");
    }

    freeBoard(board);
    return EXIT_SUCCESS;
}

/* 
 * Prints message about expected input. 
 */
void printInputMsg() {
    printf("Format expected: ./TTAI <board> <player> <flag>\n");
    printf("-<board>: board in row by row format, enter '-' for blank spot. Ex: xox---o--\n");
    printf("-<player>: either 'o' or 'x'\n");
    printf("-<flag>: '-w' to determine if player winner, '-m' for next move with determination of winner\n");
}

/*
 * Minimax plays against itself
 */
void AIvsAI() {
    char ** board = createEmptyBoard();
    printBoard(board);
    int q = 0;
    for(q = 0; q < 5; q++) {
        printf("player x move\n");
        char player = 'x';
        printf("before move\n");
        Move * mv = nextMove(board, player);
        printf("got move\n");
        if(mv != NULL) {
            printf("suggestion: (%i, %i)\n", mv->i, mv->j);
            doMove(board, mv->i, mv->j, player);
            printBoard(board);
            free(mv);
        } else {
            printf("no good move\n");
        }

        printf("===================\nplayer o move\n");

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

/*
 * Parses string representation of a board and returns
 * a 2d array representation.
 */
char ** createBoardFromInput(char * inputBoard) {
    /* wrong input */
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

/*
 * Free allocated board.
 */
void freeBoard(char ** board) {
    int i;
    for(i = 0; i < BOARD_SIZE; i++) {
        free(board[i]);
    }
    free(board);
}

/*
 * Maximization move, in other words the computer move. The AI wants to
 * maximize in order to win.
 */
int max(char ** board, Move * optimalMove, char player, int depth) {
    int max = INT_MIN;
    int won = 0;
    int i,j;

    /* the higher the depth, the less desirable the move is */
    depth++;

    /* loop through all avaialble moves */
    for(i = 0; i < BOARD_SIZE; i++) {
        for(j = 0; j < BOARD_SIZE; j++) {
            int value = 0;
            //available move
            if(board[i][j] == '-') {
                doMove(board, i, j, player);
                char isWin = getWinner(board);
                if(isWin == player) {
                    /* the player won with this move */
                    value = 10 - depth;
                    won = 1;
                } else if(isWin == '-') {
                    /* draw, less desirable than a win */
                    value = 0;
                } else {
                    /* the game is not over yet so continue to opponent player move simulation */
                    value = min(board, optimalMove, getOpponentMark(player), depth);
                }
                undoMove(board, i, j);
                
                /* we have found a more desirable move */
                if(value > max) {
                    max = value;

                    /* update best move only on first level since we want to pick
                     * the maximum optimal move out of all possible
                     */
                    if(depth==1) {
                        optimalMove -> i = i;
                        optimalMove -> j = j;
                    }
                }
            }
        }
    }
    return max;
}

/*
 * Minimizes move, in other words the opponent move. The AI needs to simulate
 * the opponent move in order to see if the previous move is the most optimal
 * compared to other moves.
 */
int min(char ** board, Move * optimalMove, char player, int depth) {
    int min = INT_MAX;
    int won = 0;
    int i,j;
    depth++;
    /* iterate through all available moves */
    for(i = 0; i < BOARD_SIZE; i++) {
        for(j = 0; j < BOARD_SIZE; j++) {
            int value = 0;
            //available move
            if(board[i][j] == '-') {
                doMove(board, i, j, player);
                char isWin = getWinner(board);
                if(isWin == player) {
                    /* this move generates a win */
                    value = depth - 10;
                    won = 1;
                } else if(isWin == '-') {
                    /* draw */
                    value = 0;
                } else {
                    /* no winner has been found, continue the simulation */
                    value = max(board, optimalMove, getOpponentMark(player), depth);
                }
                undoMove(board, i, j);
                if(value < min) {
                    min = value;
                }
            }
        }
    }
    return min;
}

/*
 * Figures out the mark of the opponent.
 */
char getOpponentMark(char player) {
    if(player == 'x')
        return 'o';
    if(player == 'o')
        return 'x';
    return 'E';
}

/*
 * Given a board and player mark, returns a struct with the next best move.
 */
Move * nextMove(char ** board, char player) {
    Move * optimalMove = (Move *)malloc(sizeof(Move *));
    optimalMove -> i = -1;
    optimalMove -> j = -1;

    int initialDepth = 0;
    /* runs minimax algorithm to find the next best move */
    max(board, optimalMove, player, initialDepth);

    /* no optimal move has been found */
    if(optimalMove -> i == -1 || optimalMove -> j == -1) {
        free(optimalMove);
        return NULL;
    }
    return optimalMove;
}

/*
 * Prints board, mainly used for debugging.
 */
void printBoard (char ** board) {
    int i,j;
    for(i = 0; i < BOARD_SIZE; i++) {
        for(j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
}

/*
 * Returns a board based on the hard coded positions. Was used 
 * for debugging during development.
 */
char ** createCustomBoard(){
    char ** board = (char **)malloc(sizeof(char *) * BOARD_SIZE);
     int i;
    for(i = 0; i < BOARD_SIZE; i++) {
        board[i] = (char *)malloc(sizeof(char) * BOARD_SIZE);
    }

    /* defined custom hypothetical board here */
    board[0][0] = 'o';  board[0][1] = 'x';  board[0][2] = 'o';
    board[1][0] = 'x';  board[1][1] = 'x';  board[1][2] = 'o';
    board[2][0] = '-';  board[2][1] = '-';  board[2][2] = '-';
    return board;
}

/*
 * Creates an empty board. Used when facing an AI vs AI.
 */
char ** createEmptyBoard() {
    char ** board = (char **)malloc(sizeof(char *) * BOARD_SIZE);
    int i,j;
    for(i = 0; i < BOARD_SIZE; i++) {
        board[i] = (char *)malloc(sizeof(char) * BOARD_SIZE);
    }
    for(i = 0; i < BOARD_SIZE; i++) {
        for(j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = '-';
        }
    }
    return board;
}

/*
 * Performs a specified move on the board.
 */
void doMove(char ** board, int i, int j, char player) {
    board[i][j] = player;
}

/*
 * Reverses the specified move.
 */
void undoMove(char ** board, int i, int j) {
    board[i][j] = '-';
}

/*
 * Determines if winner. If the given player is a winner, the 
 * method returns the player mark. If player is looser then returns the
 * opponent mark. If draw returns '-'. If no winner returns ' '
 */
char getWinner(char ** board) {
    char player = 'o';
    char oppPlayer = getOpponentMark(player);
    int col = 0, row = 0, ldiag = 0, rdiag = 0;
    int col_op = 0, row_op = 0, ldiag_op = 0, rdiag_op = 0;
    int i;
    /* check diagonals */
    for(i = 0; i < BOARD_SIZE; i++) {

        if(board[i][i] == player)
            ldiag++;
        else if(board[i][i] == oppPlayer)
            ldiag_op++;

        if(board[i][BOARD_SIZE - i - 1] == player)
            rdiag++;
        else if(board[i][BOARD_SIZE - i - 1] == oppPlayer)
            rdiag_op++;
    }
    
    if(ldiag == BOARD_SIZE || rdiag == BOARD_SIZE) {
        /* given player won */
        return player;
    } else if(ldiag_op == BOARD_SIZE || rdiag_op == BOARD_SIZE) {
        /* given player lost */
        return oppPlayer;
    }
    int allOccupied = 0;
    int j,k;
    /* check rows/cols */
    for(j = 0; j < BOARD_SIZE; j++) {
        col = 0;
        row = 0;
        col_op = 0;
        row_op = 0;
        for(k = 0; k < BOARD_SIZE; k++) {
            if(board[j][k] != '-')
                allOccupied++;
            
            if(board[j][k] == player) {
                col++;
                if (col == BOARD_SIZE) {
                    /* given player won */
                    return player;
                }
            } else if(board[j][k] == oppPlayer) {
                col_op++;
                if (col_op == BOARD_SIZE) {
                    /* other player won */
                    return oppPlayer;
                }
            }
    
            if(board[k][j] == player) {
                row++;
                if(row == BOARD_SIZE) {
                    /* given player won */
                    return player;
                }
            } else if(board[k][j] == oppPlayer) {
                row_op++;
                if(row_op == BOARD_SIZE) {
                    /* other player won */
                    return oppPlayer;
                }
            }
        }
    }
    
    if(col == BOARD_SIZE || row == BOARD_SIZE) 
        return player;
    else if(col_op == BOARD_SIZE || row_op == BOARD_SIZE) 
        return oppPlayer;

    /* it's a draw */
    if(allOccupied == BOARD_SIZE * BOARD_SIZE)
        return '-';
    /* no winner yet */
    return ' ';
}
