#include "switches.h"
#include "leds.h"
#include "xil_io.h"
#include "xparameters.h"

#define NO_OFFSET 0x00
#define TRI_STATE_OFFSET 0x04
#define TRI_STATE_OFF 0xFFFFFFFF
#define ALL_ON 0xF

// Helper function to read GPIO registers.
// Argument "int32_t offset" is the offset from the
// provided switches register address.
// Returns the values of the switches or other register
// based on the offset.
int32_t switches_readGpioRegister(int32_t offset) {
  return Xil_In32(
      XPAR_SLIDE_SWITCHES_BASEADDR +
      offset); // uses the provided Xilinx function to read the switch values
}

// Initializes the SWITCHES driver software and hardware.
// Returns one of the STATUS values defined in switches.h
int32_t switches_init() {
  Xil_Out32((XPAR_SLIDE_SWITCHES_BASEADDR + TRI_STATE_OFFSET),
            TRI_STATE_OFF); // set GPIO pins to function as input by writing 1's
                            // to the tri-state

  if (switches_readGpioRegister(TRI_STATE_OFFSET) ==
      TRI_STATE_OFF) // check if the value we read from the tri-state is the
                     // same as what we wrote
    return SWITCHES_INIT_STATUS_OK;
  else
    return SWITCHES_INIT_STATUS_FAIL;
}

// Returns the current value of all 4 switches as the lower 4 bits of the
// returned value. bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
int32_t switches_read() {
  return switches_readGpioRegister(
      NO_OFFSET); // use helper function to read GPIO register for buttons
}

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.
void switches_runTest() {
  uint8_t ledValue = 0x00;
  switches_init();

  while (1) { // runs the function until all switches are turned on
    if ((switches_read() & ALL_ON) ==
        ALL_ON) {          // check if all four switches are on
      leds_write(~ALL_ON); // turn off all four LEDs
      break;               // return from function if all four switches are on
    }

    if ((switches_read() & SWITCHES_SW0_MASK) ==
        SWITCHES_SW0_MASK) // check if switch 0 is on
      ledValue = (ledValue | SWITCHES_SW0_MASK);
    else
      ledValue = (ledValue & ~SWITCHES_SW0_MASK);

    if ((switches_read() & SWITCHES_SW1_MASK) ==
        SWITCHES_SW1_MASK) // check if switch 1 is on
      ledValue = (ledValue | SWITCHES_SW1_MASK);
    else
      ledValue = (ledValue & ~SWITCHES_SW1_MASK);

    if ((switches_read() & SWITCHES_SW2_MASK) ==
        SWITCHES_SW2_MASK) // check if switch 2 is on
      ledValue = (ledValue | SWITCHES_SW2_MASK);
    else
      ledValue = (ledValue & ~SWITCHES_SW2_MASK);

    if ((switches_read() & SWITCHES_SW3_MASK) ==
        SWITCHES_SW3_MASK) // check if switch 3 is on
      ledValue = (ledValue | SWITCHES_SW3_MASK);
    else
      ledValue = (ledValue & ~SWITCHES_SW3_MASK);

    leds_write(ledValue); // turn on or off LEDs according to new ledValue
  }

  return;
}
