#include "buttons.h"
#include "display.h"
#include "xil_io.h"
#include "xparameters.h"

#define NO_OFFSET 0x00
#define TRI_STATE_OFFSET 0x04
#define TRI_STATE_OFF 0xFFFFFFFF
#define ALL_ON 0xF
#define X_START 0
#define Y_START 0
#define X_MIDDLE 160
#define RECT_WIDTH 80
#define RECT_HEIGHT 120
#define CURSOR_Y 50
#define CURSOR_X 18
#define TEXT_SIZE 2

// Helper function to read GPIO registers.
// Argument "int32_t offset" is the offset from the
// provided buttons register address.
// Returns the values of the buttons or other register
// based on the offset.
int32_t buttons_readGpioRegister(int32_t offset) {
  //   Just use the low-level Xilinx call.
  return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

// Initializes the button driver software and hardware. Returns one of the
// defined status values in buttons.h .
int32_t buttons_init() {
  Xil_Out32((XPAR_PUSH_BUTTONS_BASEADDR + TRI_STATE_OFFSET),
            TRI_STATE_OFF); // set GPIO pins to function as input by writing 1s
                            // to the tri-state

  if (buttons_readGpioRegister(TRI_STATE_OFFSET) ==
      TRI_STATE_OFF) // check if the value we read from the tri-state is the
                     // same as what we wrote
    return BUTTONS_INIT_STATUS_OK;
  else
    return BUTTONS_INIT_STATUS_FAIL;
}

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
int32_t buttons_read() {
  return buttons_readGpioRegister(
      NO_OFFSET); // use helper function to read GPIO register for buttons
}

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will until all 4 pushbuttons are
// simultaneously pressed.
void buttons_runTest() {

  buttons_init();                    // initialize the buttons
  display_fillScreen(DISPLAY_BLACK); // clear the screen

  while (1) { // run the function until all four buttons are pressed
    if ((buttons_read() & ALL_ON) ==
        ALL_ON) { // check if all four buttons are pressed
      display_fillScreen(DISPLAY_BLACK); // clear the screen
      break; // return from function if all four buttons are pressed
    }

    if ((buttons_read() & BUTTONS_BTN0_MASK) ==
        BUTTONS_BTN0_MASK) { // check if button 0 is pressed
      display_fillRect((DISPLAY_WIDTH - RECT_WIDTH), Y_START, RECT_WIDTH,
                       RECT_HEIGHT,
                       DISPLAY_YELLOW); // show first rectangle on LCD

      display_setCursor((DISPLAY_WIDTH - RECT_WIDTH + CURSOR_X),
                        CURSOR_Y);         // left of the first rectangle
      display_setTextColor(DISPLAY_BLACK); // set the text black
      display_setTextSize(TEXT_SIZE);      // make the text larger
      display_println("BTN0");             // prints the string to the LCD.
    } else
      display_fillRect((DISPLAY_WIDTH - RECT_WIDTH), Y_START, RECT_WIDTH,
                       RECT_HEIGHT,
                       DISPLAY_BLACK); // erase first square on the LCD

    if ((buttons_read() & BUTTONS_BTN1_MASK) ==
        BUTTONS_BTN1_MASK) { // check if button 1 is pressed
      display_fillRect(X_MIDDLE, Y_START, RECT_WIDTH, RECT_HEIGHT,
                       DISPLAY_GREEN); // show second square on LCD

      display_setCursor((X_MIDDLE + CURSOR_X),
                        CURSOR_Y);         // left of the second rectangle
      display_setTextColor(DISPLAY_BLACK); // set the text black
      display_setTextSize(TEXT_SIZE);      // make the text larger
      display_println("BTN1");             // prints the string to the LCD.
    } else
      display_fillRect(X_MIDDLE, Y_START, RECT_WIDTH, RECT_HEIGHT,
                       DISPLAY_BLACK); // erase second square on the LCD

    if ((buttons_read() & BUTTONS_BTN2_MASK) ==
        BUTTONS_BTN2_MASK) { // check if button 2 is pressed
      display_fillRect(RECT_WIDTH, Y_START, RECT_WIDTH, RECT_HEIGHT,
                       DISPLAY_RED); // show third sqaure on LCD

      display_setCursor((RECT_WIDTH + CURSOR_X),
                        CURSOR_Y);         // left of the third rectangle
      display_setTextColor(DISPLAY_WHITE); // set the text black
      display_setTextSize(TEXT_SIZE);      // make the text larger
      display_println("BTN2");             // prints the string to the LCD.
    } else
      display_fillRect(RECT_WIDTH, Y_START, RECT_WIDTH, RECT_HEIGHT,
                       DISPLAY_BLACK); // erase third square on LCD

    if ((buttons_read() & BUTTONS_BTN3_MASK) ==
        BUTTONS_BTN3_MASK) { // check if button 3 is pressed
      display_fillRect(X_START, Y_START, RECT_WIDTH, RECT_HEIGHT,
                       DISPLAY_BLUE); // show fourth squre on LCD

      display_setCursor((X_START + CURSOR_X),
                        CURSOR_Y);         // left of the fourth rectangle
      display_setTextColor(DISPLAY_WHITE); // set the text black
      display_setTextSize(TEXT_SIZE);      // make the text larger
      display_println("BTN3");             // prints the string to the LCD.
    } else
      display_fillRect(X_START, Y_START, RECT_WIDTH, RECT_HEIGHT,
                       DISPLAY_BLACK); // erase fourth square on the LCD
  }

  return;
}