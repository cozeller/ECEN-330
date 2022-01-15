#include "ticTacToeControl.h"
#include "ticTacToeDisplay.h"
#include "minimax.h"
#include "display.h"
#include "buttons.h"

#include <stdio.h>

#define TOP 0
#define MID 1
#define BOT 2
#define LFT 0
#define RGT 2
#define BTN_0_MASK 0x0001
#define TEXT_SIZE 2
#define LINE_1_CUROSR_X 35
#define LINE_2_CURSOR_X 120
#define LINE_3_CURSOR_X 35
#define LINE_4_CURSOR_X 85
#define LINE_1_CURSOR_Y 92
#define LINE_2_CURSOR_Y 107
#define LINE_3_CURSOR_Y 122
#define LINE_4_CURSOR_Y 137

#define ADC_COUNTER_MAX_VALUE 1
#define START_SCREEN_COUNTER_MAX_VALUE 40
// right now the player start timer will wait for 3s
#define PLAYER_START_COUNTER_MAX_VALUE 40

//helper function to handle all the display pieces for the start screen
void displayStartScreen(bool erase) {
    if (erase)
        display_setTextColor(DISPLAY_BLACK); // set the text color to black if erasing
    else
        display_setTextColor(DISPLAY_WHITE); // set the text color to white if not erasing
    display_setTextSize(TEXT_SIZE); // set the test size
    display_setCursor(LINE_1_CUROSR_X, LINE_1_CURSOR_Y); // set the cursor for line 1
    display_print("Touch board to play X"); // print line 1
    display_setCursor(LINE_2_CURSOR_X, LINE_2_CURSOR_Y); // set the cursor for line 2
    display_print("-or-"); // print line 2
    display_setCursor(LINE_3_CURSOR_X, LINE_3_CURSOR_Y); // set the cursor for line 3
    display_print("wait for the computer"); // print line 3
    display_setCursor(LINE_4_CURSOR_X, LINE_4_CURSOR_Y); // set the cursor for line 4
    display_print("and play O."); // print line 4
}

// States of the clockControl state machine
enum ticTacToeControl_st_t {
    init_st, // Start here, transition out of this state on the first tick.
    start_screen_st, // Display the start screen here, transition out when the counter expires
    blank_board_st,    // display the board and wait for player input or counter expiration
    adc_counter_running_st,  // waiting for the touch-controller ADC to settle.
    check_valid_move_st, // check that the touched square isn't already filled
    player_move_st, // add the player's move to the board
    computer_move_st, // add the computer's move to the board
    waiting_for_player_st, // wait for the board to be touched again
    game_over_st   // the game ended, wait for button 0 to be pressed to start a new game
};

static enum ticTacToeControl_st_t currentState;

// Tick the tic-tac-toe conroller state machine
void ticTacToeControl_tick() {
    static uint8_t adcCounter = 0;
    static uint8_t startScreenCounter = 0;
    static uint8_t playerStartCounter = 0;
    static minimax_move_t nextMove;
    static bool board_is_empty = true;
    static bool current_player_is_x = true;

    static minimax_board_t gameBoard; // initialize this in the blank board state

    // Perform state updates first. Mealy actions go here as well
    switch (currentState) {
        case init_st:
            currentState = start_screen_st;
            break;
        case start_screen_st:
            if (startScreenCounter == START_SCREEN_COUNTER_MAX_VALUE) {
                currentState = blank_board_st;
                displayStartScreen(true); // erase the start screen message by rewriting it in black
            }
            break;
        case blank_board_st:
            if (display_isTouched()) { // if the player touches the LCD screen, move to adc_counter_running_st
                currentState = adc_counter_running_st;
                display_clearOldTouchData(); // clear the previous touch data
            }
            else if (playerStartCounter == PLAYER_START_COUNTER_MAX_VALUE)
                currentState = computer_move_st;
            break;
        case adc_counter_running_st:
            if (adcCounter == ADC_COUNTER_MAX_VALUE)
                currentState = check_valid_move_st;
            break;
        case check_valid_move_st:
            if (gameBoard.squares[nextMove.row][nextMove.column] == MINIMAX_EMPTY_SQUARE) // if the square selected by the player is empty, move to player_move_st
                currentState = player_move_st;
            else // if the square selected by the player is filled, move to waiting_for_player_st
                currentState = waiting_for_player_st;
            break;
        case player_move_st:
            if (minimax_isGameOver(minimax_computeBoardScore(&gameBoard, current_player_is_x))) // if the game is over, move to game_over_st
                currentState = game_over_st;
            else { // otherwise move to computer_move_st
                currentState = computer_move_st;
                current_player_is_x = !current_player_is_x;
            }
            break;
        case computer_move_st:
            if (minimax_isGameOver(minimax_computeBoardScore(&gameBoard, current_player_is_x))) // if the game is over, move to game_over_st
                currentState = game_over_st;
            else { // otherwise move to waiting_for_player_st
                currentState = waiting_for_player_st;
                current_player_is_x = !current_player_is_x;
            }
            break;
        case waiting_for_player_st:
            if (display_isTouched()) { // if the player touches the LCD screen, move to adc_counter_running_st
                currentState = adc_counter_running_st;
                display_clearOldTouchData(); // clear the old touch data
            }
            break;
        case game_over_st:
            if ((buttons_read() & BTN_0_MASK) == BTN_0_MASK) { // if button 0 is pressed reset the game
                currentState = blank_board_st;
                current_player_is_x = true;
                for (int8_t i = 0; i < MINIMAX_BOARD_ROWS; i++) { // for loop to move through each row
                    for (int8_t j = 0; j < MINIMAX_BOARD_COLUMNS; j++) { // for loop to move through each column
                        if (gameBoard.squares[i][j] == MINIMAX_X_SQUARE) { // if there is an X here, erase it from the display and remove it from the gameBoard
                            ticTacToeDisplay_drawX(i, j, true); // erase the X in this i,j position on the board
                            gameBoard.squares[i][j] = MINIMAX_EMPTY_SQUARE;
                        }
                        else if (gameBoard.squares[i][j] == MINIMAX_O_SQUARE) { // if there is an O here, erase it from the display and remove it from the gameBoard
                            ticTacToeDisplay_drawO(i, j, true); // erase the O in this i,j position on the board
                            gameBoard.squares[i][j] = MINIMAX_EMPTY_SQUARE;
                        }
                    }
                }
            }
            break;
        default:
            printf("ERROR\n"); // print an error message if the state machine is not in one of the defined states
            break;
    }

    // Perform the state actions next
    switch (currentState) {
        case init_st:
            break;
        case start_screen_st:
            displayStartScreen(false); // call the helper function to display the start screen
            minimax_initBoard(&gameBoard); // initialize the game board to all empty squares
            startScreenCounter++;
            break;
        case blank_board_st:
            ticTacToeDisplay_init(); // show a blank board on the display
            playerStartCounter++;
            break;
        case adc_counter_running_st:
            adcCounter++;
            break;
        case check_valid_move_st:
            adcCounter = 0;
            ticTacToeDisplay_touchScreenComputeBoardRowColumn(&(nextMove.row), &(nextMove.column)); // get the row and column touched by the player
            break;
        case player_move_st:
            adcCounter = 0;
            if (current_player_is_x) { // if the current player is X update the game with an X
                gameBoard.squares[nextMove.row][nextMove.column] = MINIMAX_X_SQUARE; // put X in the gameBoard in the spot of the next move
                ticTacToeDisplay_drawX(nextMove.row, nextMove.column, false); // draw the X on the display in the spot of the next move
            }
            else { // if the current player is O, updat the game with an O
                gameBoard.squares[nextMove.row][nextMove.column] = MINIMAX_O_SQUARE; //  put O in the gameBoard in the spot of the next move
                ticTacToeDisplay_drawO(nextMove.row, nextMove.column, false); // draw the O on the display in the spot of the next move
            }
            board_is_empty = false;
            break;
        case computer_move_st:
            if (board_is_empty) { // if the board is empty, rather than recursing through minimax, play in the top left square
                nextMove.row = TOP;
                nextMove.column = LFT;
            }
            else // if the board is not empty, recurse through minimax as usual
                minimax_computeNextMove(&gameBoard, current_player_is_x, &(nextMove.row), &(nextMove.column));
            if (current_player_is_x) { // if the current player is X update the game with an X
                gameBoard.squares[nextMove.row][nextMove.column] = MINIMAX_X_SQUARE; // put an W in the gameBoard in the spot of the next move
                ticTacToeDisplay_drawX(nextMove.row, nextMove.column, false); // draw the X on the display in the spot of the next move
            }
            else { // if the current player is O, updat the game with an O
                gameBoard.squares[nextMove.row][nextMove.column] = MINIMAX_O_SQUARE; //  put O in the gameBoard in the spot of the next move
                ticTacToeDisplay_drawO(nextMove.row, nextMove.column, false); // draw the O on the display in the spot of the next move
            }
            board_is_empty = false;
            break;
        case waiting_for_player_st:
            break;
        case game_over_st:
            playerStartCounter = 0;
            board_is_empty = true;
            // minimax_initBoard(&gameBoard); // reset the game board to all empty squares for next game
            break;
        default:
            printf("ERROR\n"); // print an error message if the state machine is not in one of the defined states
            break;
    }
}

// Initialize the tic-tac-toe conroller state machine
void ticTacToeControl_init() {
    currentState = init_st;
}