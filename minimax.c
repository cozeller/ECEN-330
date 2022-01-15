#include "minimax.h"

#include<stdio.h>

#define TOP 0
#define MID 1
#define BOT 2
#define LFT 0
#define RGT 2
#define MEANINGLESS_SCORE -100

static minimax_move_t nextMove;

// helper function to check for vertical win
bool verticalWin(minimax_board_t *board) {
    for (int8_t i = 0; i < MINIMAX_BOARD_COLUMNS; i++) { // for loop to move through each column
        if (board->squares[TOP][i] != MINIMAX_EMPTY_SQUARE) { // check that the top square isn't empty to rule out an entirely empty column
            if ((board->squares[TOP][i] == board->squares[MID][i]) && (board->squares[MID][i] == board->squares[BOT][i])) { // if all the squares in a column are equal return true
                return true;
            }
        }
    }
    return false;
}

// helper function to check for horizontal win
bool horizontalWin(minimax_board_t *board) {
    for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) { // for loop to move through each row
        if (board->squares[i][LFT] != MINIMAX_EMPTY_SQUARE) { // check that the left square isn't empty to rule out an entirely empty row
            if ((board->squares[i][LFT] == board->squares[i][MID]) && (board->squares[i][MID] == board->squares[i][RGT])) { // if all the squares in a row are equal return true
                return true;
            }
        }
    }
    return false;
}

// helper function to check for a diagonal win
bool diagonalWin(minimax_board_t *board) {
    if (board->squares[MID][MID] == MINIMAX_EMPTY_SQUARE) { // check if the middle square is empty to rule out an empty diagonal
        return false;
    }
    else if (((board->squares[TOP][LFT] == board->squares[MID][MID]) && (board->squares[MID][MID] == board->squares[BOT][RGT])) || ((board->squares[TOP][RGT] == board->squares[MID][MID]) && (board->squares[MID][MID] == board->squares[BOT][LFT]))) { // if a diagonal has all the same values return true
        return true;
    }
    else { // otherwise return false
        return false;
    }
}

// helper function to check for a tie game
bool tieGame(minimax_board_t *board) {
    if (verticalWin(board) || horizontalWin(board) || diagonalWin(board)) { // check if a player has won the game
        return false;
    }
    for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) { // for loop to move through each row
        for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) { // for loop to move through each column
            if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) { // if an empty square is found, game is not over, return false
                return false;
            }
        }
    }
    return true;
}

// Recursive Minimax Function
// !!! add more comment here
minimax_score_t minimax(minimax_board_t *board, bool current_player_is_x) {
    minimax_score_t scoreTable[MINIMAX_BOARD_ROWS][MINIMAX_BOARD_COLUMNS]; // local variable for the score table, will be filled in the loop for the current level of recursion
    int8_t score = minimax_computeBoardScore(board, current_player_is_x); // initially set score equal to the current score to see if the game is over
    
    if (minimax_isGameOver(score)) {  // if thet game is over, return the score
        return score;
    }
    for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) { // for loop to move through each row
        for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) { // for loop to move through each column
            if (board->squares[i][j] == MINIMAX_EMPTY_SQUARE) { // if the square is empty, assign it with the value of the current player
                if (current_player_is_x) // if the current player is X put an X in the empty square
                    board->squares[i][j] = MINIMAX_X_SQUARE;
                else // if the current player is O put an O in the empty square
                    board->squares[i][j] = MINIMAX_O_SQUARE;
                scoreTable[i][j] = minimax(board, !current_player_is_x);
                // !!! add move to move-score table
                board->squares[i][j] = MINIMAX_EMPTY_SQUARE; // undo the change to the board
            }
            else { // if there is already a value in the square, assign the corresponding score in the score table to be meaningless
                scoreTable[i][j] = MEANINGLESS_SCORE;
            }
        }
    }
    if (current_player_is_x) { // if the current player is X we look for the highest score
        score = MINIMAX_O_WINNING_SCORE; // default: if no 10s or 0s are found in score table, return -10
        for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) { // for loop to move through each row
            for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) { // for loop to move through each column
                if (scoreTable[i][j] == MINIMAX_X_WINNING_SCORE) { // if the score is 10, save that location as the next move and return 10
                    nextMove.row = i;
                    nextMove.column = j;

                    // debug printf below
                    // printf("the next move is at: %d, %d\n", i, j);

                    return score = scoreTable[i][j];
                }
                else if (scoreTable[i][j] == MINIMAX_DRAW_SCORE) { // if the score is 0, save that location as the next move
                    score = scoreTable[i][j];
                    nextMove.row = i;
                    nextMove.column = j;
                }
                else if ((score == MINIMAX_X_WINNING_SCORE) && (scoreTable[i][j] == MINIMAX_X_WINNING_SCORE)) {
                    nextMove.row = i;
                    nextMove.column = j;
                }
                // debug printf below
                // printf("the score being returned is: %d\n", score);

            }
        }
    } 
    else { // if the current player is O we look for the lowest score 
        score = MINIMAX_X_WINNING_SCORE; // default: if no -10s or 0s are found in score table, return 10
        for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) { // for loop to move through each row
            for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) { // for loop to move through each column
                if (scoreTable[i][j] == MINIMAX_O_WINNING_SCORE) { // if the score is -10, save that location as the next move and return -10
                    nextMove.row = i;
                    nextMove.column = j;

                    // debug printf below
                    //printf("the next move is at: %d, %d\n", i, j);

                    return score = scoreTable[i][j];
                }
                else if (scoreTable[i][j] == MINIMAX_DRAW_SCORE) { // if the score is 0, save that location as the next move
                    score = scoreTable[i][j];
                    nextMove.row = i;
                    nextMove.column = j;
                }
                else if ((score == MINIMAX_X_WINNING_SCORE) && (scoreTable[i][j] == MINIMAX_X_WINNING_SCORE)) { // if a -10 or 0 hasn't been found, save the loaction of the 10 as the next move
                    nextMove.row = i;
                    nextMove.column = j;
                }
                // debug printf below
                // printf("the score being returned is: %d\n", score);
            }
        }
    }
    return score;
}

// This routine is not recursive but will invoke the recursive minimax function.
// You will call this function from the controlling state machine that you will
// implement in a later milestone. It computes the row and column of the next
// move based upon: the current board, the player. true means the computer is X.
// false means the computer is O. When called from the controlling state
// machine, you will call this function as follows:
// 1. If the computer is playing as X, you will call this function with
// current_player_is_x = true.
// 2. If the computer is playing as O, you will call this function with
// current_player_is_x = false. minimax_computeNextMove directly passes the
// current_player_is_x argument into the minimax() (helper) function. To assign
// values to the row and column arguments, you must use the following syntax in
// the body of the function: *row = move_row; *column = move_column; (for
// example).
void minimax_computeNextMove(minimax_board_t *board, bool current_player_is_x, uint8_t *row, uint8_t *column) {
    minimax(board, current_player_is_x);
    *row = nextMove.row;
    *column = nextMove.column;
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score){
    if (score == MINIMAX_NOT_ENDGAME) // check if the score argument is -1, indicating the game isn't complete
        return false;
    else
        return true;
}

// Returns the score of the board, based upon the player and the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the player argument makes it possible to speed up this function.
// Assumptions:
// (1) if current_player_is_x == true, the last thing played was an 'X'.
// (2) if current_player_is_x == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played,
// you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(minimax_board_t *board, bool player_is_x) {
    if (verticalWin(board) || horizontalWin(board) || diagonalWin(board)) { // check if a player has won
        if (player_is_x)
            return MINIMAX_O_WINNING_SCORE;
        else
            return MINIMAX_X_WINNING_SCORE;
    }
    else if (tieGame(board)) // check if the game is a draw
        return MINIMAX_DRAW_SCORE;
    else // else the game is not over
        return MINIMAX_NOT_ENDGAME;
}

// Init the board to all empty squares.
void minimax_initBoard(minimax_board_t *board) {
    for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) { // for loop to move through each row
        for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) { // for loop to move through each column
            board->squares[i][j] = MINIMAX_EMPTY_SQUARE;
        }
    }
}