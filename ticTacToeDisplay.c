#include "ticTacToeDisplay.h"
#include "display.h"
#include "buttons.h"
#include "switches.h"

#include <utils.h>

#define TOP 0
#define MID 1
#define BOT 2
#define LFT 0
#define RGT 2
#define ONE_THIRD_DISPLAY_WIDTH 107
#define TWO_THIRDS_DISPLAY_WIDTH 213
#define ONE_THIRD_DISPLAY_HEIGHT 80
#define TWO_THIRDS_DISPLAY_HEIGHT 160
#define DISPLAY_WIDTH_START 0
#define DISPLAY_HEIGHT_START 0
#define COLUMN_0_X_LEFT 27
#define COLUMN_0_X_RIGHT 80
#define COLUMN_1_X_LEFT 134
#define COLUMN_1_X_RIGHT 186
#define COLUMN_2_X_LEFT 240
#define COLUMN_2_X_RIGHT 293
#define ROW_0_X_TOP 13
#define ROW_0_X_BTM 67
#define ROW_1_X_TOP 93
#define ROW_1_X_BTM 147
#define ROW_2_X_TOP 173
#define ROW_2_X_BTM 227
#define COLUMN_0_CENTER 53
#define COLUMN_1_CENTER 160
#define COLUMN_2_CENTER 267
#define ROW_0_CENTER 40
#define ROW_1_CENTER 120
#define ROW_2_CENTER 200
#define CIRCLE_RADIUS 27
#define BTN_0_MASK 0x0001
#define BTN_1_MASK 0x0002
#define SWITCH_0_MASK 0x0001
#define ADC_SETTLE_TIME 50

// Inits the tic-tac-toe display, draws the lines that form the board.
void ticTacToeDisplay_init() {
    ticTacToeDisplay_drawBoardLines(); // call the function to draw the four lines of the ticTacToe board
}

// Draws an X at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase ==
// false, draw the X as foreground.
void ticTacToeDisplay_drawX(uint8_t row, uint8_t column, bool erase) {
    int16_t color, x1, x2, y1, y2;
    if (erase)
        color = DISPLAY_BLACK;
    else
        color = DISPLAY_WHITE;
    if (column == LFT) { // if drawing in left column, set the x values accordingly
        x1 = COLUMN_0_X_LEFT;
        x2 = COLUMN_0_X_RIGHT;
    }
    else if (column == MID) { // if drawing in middle column, set the x values accordingly
        x1 = COLUMN_1_X_LEFT;
        x2 = COLUMN_1_X_RIGHT;
    }
    else { // if drawing in the right column, set the x values accordingly
        x1 = COLUMN_2_X_LEFT;
        x2 = COLUMN_2_X_RIGHT;
    }
    if (row == TOP) { // if drawing in the top row, set the y values accordingly
        y1 = ROW_0_X_TOP;
        y2 = ROW_0_X_BTM;
    }
    else if (row == MID) { // if drawing in the middle row, set the y values accordingly
        y1 = ROW_1_X_TOP;
        y2 = ROW_1_X_BTM;
    }
    else { // if drawing in the bottom row, set the y values accordingly
        y1 = ROW_2_X_TOP; 
        y2 = ROW_2_X_BTM;
    }
    display_drawLine(x1, y1, x2, y2, color); // draw the line from top left to bottom right using the color and positions obtained above
    display_drawLine(x1, y2, x2, y1, color); // draw the line from bottom left to top right using the color and positions obtained above
}

// Draws an O at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase ==
// false, draw the X as foreground.
void ticTacToeDisplay_drawO(uint8_t row, uint8_t column, bool erase) {
    int16_t color, x1, y1;
    if (erase)
        color = DISPLAY_BLACK;
    else
        color = DISPLAY_WHITE;
    if (column == LFT) { // if drawing in left column, set the x value accordingly
        x1 = COLUMN_0_CENTER;
    }
    else if (column == MID) { // if drawing in middle column, set the x value accordingly
        x1 = COLUMN_1_CENTER;
    }
    else { // if drawing in the right column, set the x value accordingly
        x1 = COLUMN_2_CENTER;
    }
    if (row == TOP) { // if drawing in the top row, set the y value accordingly
        y1 = ROW_0_CENTER;
    }
    else if (row == MID) { // if drawing in the middle row, set the y value accordingly
        y1 = ROW_1_CENTER;
    }
    else { // if drawing in the bottom row, set the y value accordingly
        y1 = ROW_2_CENTER; 
    }
    display_drawCircle(x1, y1, CIRCLE_RADIUS, color); // draw the circle using the color and position obtained above
}

// After a touch has been detected and after the proper delay, this sets the row
// and column arguments according to where the user touched the board.
void ticTacToeDisplay_touchScreenComputeBoardRowColumn(uint8_t *row, uint8_t *column) {
    int16_t x, y;
    uint8_t z;
    display_getTouchedPoint(&x, &y, &z); // get the coordinates of where the LCD screen was touched
    if (x < ONE_THIRD_DISPLAY_WIDTH)
        *column = LFT;
    else if (x < TWO_THIRDS_DISPLAY_WIDTH)
        *column = MID;
    else 
        *column = RGT;
    if (y < ONE_THIRD_DISPLAY_HEIGHT)
        *row = TOP;
    else if (y < TWO_THIRDS_DISPLAY_HEIGHT)
        *row = MID;
    else
        *row = BOT;
}

// Runs a test of the display. Does the following.
// Draws the board. Each time you touch one of the screen areas, the screen will
// paint an X or an O, depending on whether switch 0 (SW0) is slid up (O) or
// down (X). When BTN0 is pushed, the screen is cleared. The test terminates
// when BTN1 is pushed.
void ticTacToeDisplay_runTest() {
    uint8_t row, column;
    ticTacToeDisplay_init();
    while ((buttons_read() & BTN_1_MASK) != BTN_1_MASK) { // if button 1 is pressed, runTest() will end
        if ((buttons_read() & BTN_0_MASK) == BTN_0_MASK) { // if button 0 is pressed reset the screen
            display_fillScreen(DISPLAY_BLACK); // clear the whole screen by filling with black
            ticTacToeDisplay_drawBoardLines(); // redraw the four lines
        }
        if (display_isTouched()) { // if the board is touched update the display
            display_clearOldTouchData(); // clear old touch data
            utils_msDelay(ADC_SETTLE_TIME); // wait 50 ms for the ADC to settle
            ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column); // get the row and column of where was touched
            if ((switches_read() & SWITCH_0_MASK) == SWITCH_0_MASK) // if switch 0 is high, draw an O
                ticTacToeDisplay_drawO(row, column, false); // draw an O in the row and column where the LCD is touched
            else // if switch 0 is low, draw an X
                ticTacToeDisplay_drawX(row, column, false); // draw an X in the row and column where the LCD is touched
        }
    }  
}

// This will draw the four board lines.
void ticTacToeDisplay_drawBoardLines() {
    display_drawLine(DISPLAY_WIDTH_START, ONE_THIRD_DISPLAY_HEIGHT, DISPLAY_WIDTH, ONE_THIRD_DISPLAY_HEIGHT, DISPLAY_WHITE); // draw the upper horizontal line
    display_drawLine(DISPLAY_WIDTH_START, TWO_THIRDS_DISPLAY_HEIGHT, DISPLAY_WIDTH, TWO_THIRDS_DISPLAY_HEIGHT, DISPLAY_WHITE); // draw the lower horizontal line
    display_drawLine(ONE_THIRD_DISPLAY_WIDTH, DISPLAY_HEIGHT_START, ONE_THIRD_DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_WHITE); // draw the left vertical line
    display_drawLine(TWO_THIRDS_DISPLAY_WIDTH, DISPLAY_HEIGHT_START, TWO_THIRDS_DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_WHITE); // draw the right vertical line
}