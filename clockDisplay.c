#include <stdint.h>
#include <stdio.h>
#include <utils.h>

#include "clockDisplay.h"
#include "display.h"

#define HOURS_MAXIMUM 12
#define SEC_MIN_MAXIMUM 59
#define HOURS_MINIMUM 1
#define SEC_MIN_MINIMUM 0
#define HALF_DISPLAY_WIDTH 160
#define HALF_DISPLAY_HEIGHT 120
#define NUM_OF_CHARS 8
#define HALF_NUM_OF_CHARS 4
#define QUARTER_NUM_OF_CHARS 2
#define CURSOR_START_OFFSET 2
#define TRIANGLE_OFFSET_1 2
#define TRIANGLE_OFFSET_2 3
#define SEC_MIN_TEST_LIMIT 30
#define TIME_RUN_TEST_LIMIT 100
#define LONG_TIME_DELAY 1000
#define SHORT_TIME_DELAY 100

// global variables?
uint8_t currentDisplayTime[9];
uint8_t newTime[9];
uint8_t hours;
uint8_t minutes;
uint8_t seconds;

// Helper function that calculates the cursor position base don text size
// and calls display_setCursor to reset it to the start position
void resetCursor() {
    uint8_t cursorStartX = HALF_DISPLAY_WIDTH - (HALF_NUM_OF_CHARS * CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH);
    uint8_t cursorStartY = HALF_DISPLAY_HEIGHT - (DISPLAY_CHAR_HEIGHT * CLOCKDISPLAY_TEXT_SIZE / CURSOR_START_OFFSET);
    display_setCursor(cursorStartX, cursorStartY); // set the cursor to start where the leftmost number will go
}

// Called only once - performs any necessary inits.
// This is a good place to draw the triangles and any other
// parts of the clock display that will never change.
void clockDisplay_init() {

    resetCursor(); // set the cursor to where the leftmost character will go
    display_setTextColor(DISPLAY_YELLOW); // set the text color to yellow
    display_setTextSize(CLOCKDISPLAY_TEXT_SIZE); // set the text size

    uint16_t triangleHeight = (QUARTER_NUM_OF_CHARS * CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH * TRIANGLE_OFFSET_1 / TRIANGLE_OFFSET_2);
    uint16_t leftTriangleX0 = HALF_DISPLAY_WIDTH - (HALF_NUM_OF_CHARS * CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH);
    uint16_t leftTriangleX1 = HALF_DISPLAY_WIDTH - (QUARTER_NUM_OF_CHARS * CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH);
    uint16_t leftTrangleX2 = leftTriangleX0 + (CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH);
    uint16_t middleTriangleX0 = HALF_DISPLAY_WIDTH - (CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH);
    uint16_t middleTriangleX1 = HALF_DISPLAY_WIDTH + (CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH);
    uint16_t rightTriangleX0 = HALF_DISPLAY_WIDTH + (QUARTER_NUM_OF_CHARS * CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH);
    uint16_t rightTriangleX1 = HALF_DISPLAY_WIDTH + (HALF_NUM_OF_CHARS * CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH);
    uint16_t rightTriangleX2 = rightTriangleX0 + (CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH);
    uint16_t upperTriangleY0 = HALF_DISPLAY_HEIGHT + (CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_HEIGHT * TRIANGLE_OFFSET_1 / TRIANGLE_OFFSET_2);
    uint16_t upperTriangleY2 = upperTriangleY0 + triangleHeight;
    uint16_t lowerTriangleY0 = HALF_DISPLAY_HEIGHT - (CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_HEIGHT * TRIANGLE_OFFSET_1 / TRIANGLE_OFFSET_2);
    uint16_t lowerTriangleY2 = lowerTriangleY0 - triangleHeight;

    display_fillTriangle(leftTriangleX0, upperTriangleY0, leftTriangleX1, upperTriangleY0, leftTrangleX2, upperTriangleY2, DISPLAY_RED); // draw the upper left triangle
    display_fillTriangle(middleTriangleX0, upperTriangleY0, middleTriangleX1, upperTriangleY0, HALF_DISPLAY_WIDTH, upperTriangleY2, DISPLAY_RED); // draw the upper middle triangle
    display_fillTriangle(rightTriangleX0, upperTriangleY0, rightTriangleX1, upperTriangleY0, rightTriangleX2, upperTriangleY2, DISPLAY_RED); // draw the upper right triangle
    display_fillTriangle(leftTriangleX0, lowerTriangleY0, leftTriangleX1, lowerTriangleY0, leftTrangleX2, lowerTriangleY2, DISPLAY_RED); // draw the lower left triangle
    display_fillTriangle(middleTriangleX0, lowerTriangleY0, middleTriangleX1, lowerTriangleY0, HALF_DISPLAY_WIDTH, lowerTriangleY2, DISPLAY_RED); // draw the lower middle triangle
    display_fillTriangle(rightTriangleX0, lowerTriangleY0, rightTriangleX1, lowerTriangleY0, rightTriangleX2, lowerTriangleY2, DISPLAY_RED); // draw the lower right triangle

    display_print("  :  :  "); // print the starting value for the clock
    resetCursor(); // set the cursor to where the leftmost character will go
    hours = HOURS_MAXIMUM;
    minutes = seconds = SEC_MIN_MAXIMUM;
    sprintf(currentDisplayTime, "%2hd %02hd %02hd", hours, minutes, seconds);
    display_print(currentDisplayTime);
}

// Updates the time display with latest time, making sure to update only those
// digits that have changed since the last update. if forceUpdateAll is true,
// update all digits.
void clockDisplay_updateTimeDisplay(bool forceUpdateAll) {
    resetCursor(); // set the cursor to where the leftmost character will go
    sprintf(newTime, "%2hd %02hd %02hd", hours, minutes, seconds);

    if (forceUpdateAll) { // rewrite all characters if forceUpdateAll is true
        display_setTextColor(DISPLAY_BLACK); // set the text color to black
        display_println(currentDisplayTime); // print the old time in black to erase it

        for (uint8_t i = 0; i < NUM_OF_CHARS; i++) {
            currentDisplayTime[i] = newTime[i];
        }

        resetCursor(); // set the cursor to where the leftmost character will go
        display_setTextColor(DISPLAY_YELLOW); // set the text color to yellow
        display_print(currentDisplayTime); // print the new time in yellow
    }
    else { // rewrite only the characters that didn't change if forceUpdateAll is false

        for (uint8_t i = 0; i < NUM_OF_CHARS; i++) { // move through each index of the arrays one at a time

            if (currentDisplayTime[i] == newTime[i]) { // if the character doesn't need to be updated
                display_print(" "); // print a space (nothing) to move the cursor along on
            }
            else { // if there is a new character to be updated
                display_setTextColor(DISPLAY_BLACK); // set the text color to black
                display_printChar(currentDisplayTime[i]); // print the old character in black to erase it
                resetCursor(); // set the cursor back to where the leftmost character will go

                for (uint8_t j = 0; j < i; j++)
                    display_print(" "); // print a space for every character that doesn't change to move the cursor back to where we just erased
                
                display_setTextColor(DISPLAY_YELLOW); // set the text color back to yellow
                currentDisplayTime[i] = newTime[i];
                display_printChar(currentDisplayTime[i]); // print the new character in yellow
            }
        }
    }
}

// Reads the touched coordinates and performs the increment or decrement,
// depending upon the touched region.
void clockDisplay_performIncDec() {
    ;
}

// Advances the time forward by 1 second and updates the display.
void clockDisplay_advanceTimeOneSecond() {
    if (seconds == SEC_MIN_MAXIMUM) { // check if we need to roll over seconds
        if (minutes == SEC_MIN_MAXIMUM) { // check if we need to roll over minutes
            if (hours == HOURS_MAXIMUM) { // check if we need to roll over hours
                hours = HOURS_MINIMUM;
                minutes = seconds = SEC_MIN_MINIMUM;
            }
            else {
                hours++;
                minutes = seconds = SEC_MIN_MINIMUM;
            }
        }
        else {
            minutes++;
            seconds = SEC_MIN_MINIMUM;
        }
    }
    else {
        seconds++;
    }
    clockDisplay_updateTimeDisplay(false); // update only the characters that have changed on the display
}

// Run a test of clock-display functions.
void clockDisplay_runTest() {
    clockDisplay_init(); // inititalize the clock 
    utils_msDelay(LONG_TIME_DELAY); // wait for one second

    for (int8_t i = HOURS_MINIMUM; i <= HOURS_MAXIMUM; i++) { // increment hours from 1 up to 12
        hours = i;
        utils_msDelay(100); // wait for 100ms
        clockDisplay_updateTimeDisplay(false); // update only the characters that have changed on the display
    }
    for (int8_t i = HOURS_MAXIMUM; i >= HOURS_MINIMUM; i--) { // increment hours from 12 down to 1
        hours = i;
        utils_msDelay(100); // wait for 100ms
        clockDisplay_updateTimeDisplay(false); // update only the characters that have changed on the display
    }
    for (int8_t i = SEC_MIN_MINIMUM; i <= SEC_MIN_TEST_LIMIT; i++) { // increment minutes from 0 up to 30
        minutes = i;
        utils_msDelay(100); // wait for 100ms
        clockDisplay_updateTimeDisplay(false); // update only the characters that have changed on the display
    }
    for (int8_t i = SEC_MIN_TEST_LIMIT; i >= SEC_MIN_MINIMUM; i--) { // increment minutes from 30 down to 0
        minutes = i;
        utils_msDelay(100); // wait for 100ms
        clockDisplay_updateTimeDisplay(false); // update only the characters that have changed on the display
    }
    for (int8_t i = SEC_MIN_MINIMUM; i <= SEC_MIN_TEST_LIMIT; i++) { // increment seconds from 0 up to 30
        seconds = i;
        utils_msDelay(100); // wait for 100ms
        clockDisplay_updateTimeDisplay(false); // update only the characters that have changed on the display
    }
    for (int8_t i = SEC_MIN_TEST_LIMIT; i >= SEC_MIN_MINIMUM; i--) { // increment seconds from 30 down to 0
        seconds = i;
        utils_msDelay(100); // wait for 100ms
        clockDisplay_updateTimeDisplay(false); // update only the characters that have changed on the display
    }
    for (int8_t i = 0; i < TIME_RUN_TEST_LIMIT; i++) { // iterate 100 times with a 100ms delay each iteration to run for 10s
        clockDisplay_advanceTimeOneSecond(); // advance the clock by one second
        utils_msDelay(100); // wait for 100ms
    }
}