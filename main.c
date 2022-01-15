/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "display.h"

#define X_START 0
#define Y_START 0
#define X_MIDDLE 160
#define Y_MIDDLE 120
#define CIRLCE_RADIUS 27
#define CIRCLE_FROM_EDGE 96
#define TRIANGLE_FROM_EDGE 40
#define TRIANGLE_HEIGHT 56
#define TRINAGLE_WIDTH 50

#include <stdio.h>
int main() {
  display_init(); // Must init all of the software and underlying hardware for
                  // LCD.
  display_fillScreen(DISPLAY_BLACK);     // Blank the screen.
  display_drawLine(X_START, Y_START, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_YELLOW);     //Draw the line from top left to bottom right
  display_drawLine(DISPLAY_WIDTH, Y_START, X_START, DISPLAY_HEIGHT, DISPLAY_YELLOW);     //Draw the line from top right to bottom left
  display_drawCircle(CIRCLE_FROM_EDGE, Y_MIDDLE, CIRLCE_RADIUS, DISPLAY_RED);     //Draw the open circle
  display_fillCircle((DISPLAY_WIDTH - CIRCLE_FROM_EDGE), Y_MIDDLE, CIRLCE_RADIUS, DISPLAY_RED);     //Draw the filled circle
  display_drawTriangle((X_MIDDLE - (TRINAGLE_WIDTH / 2)), (DISPLAY_HEIGHT - TRIANGLE_FROM_EDGE), (X_MIDDLE + (TRINAGLE_WIDTH / 2)), (DISPLAY_HEIGHT - TRIANGLE_FROM_EDGE), X_MIDDLE, (DISPLAY_HEIGHT - (TRIANGLE_FROM_EDGE + TRIANGLE_HEIGHT)), DISPLAY_YELLOW);     //Draw the open triangle
  display_fillTriangle((X_MIDDLE - (TRINAGLE_WIDTH / 2)), TRIANGLE_FROM_EDGE, (X_MIDDLE + (TRINAGLE_WIDTH / 2)), TRIANGLE_FROM_EDGE, X_MIDDLE, (TRIANGLE_FROM_EDGE + TRIANGLE_HEIGHT), DISPLAY_YELLOW);     //Draw the filled triangle

  return 0;
}

void isr_function() {
  // Empty for now.
}