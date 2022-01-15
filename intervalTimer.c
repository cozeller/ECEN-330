#include "intervalTimer.h"
#include "xparameters.h"
#include "xil_io.h"

#define CLEAR_REGISTER 0x0
#define TCSR0_OFFSET 0x0
#define TCSR1_OFFSET 0x10
#define TLR0_OFFSET 0x04
#define TRL1_OFFSET 0x14
#define TCR0_OFFSET 0x08
#define TCR1_OFFSET 0x18
#define CASC_MASK 0x0800
#define UDT0_MASK 0x0002
#define ENT0_MASK 0x0080
#define LOAD_MASK 0x0020
#define TIMER_WIDTH 32
#define TIMER_NUMBER_INVALID 0

// Helper function to read from registers.
// The argument baseAddress gives base address and the 
// argument offset gives the offset for the register to read.
// The value stored in the register given by the address 
// and offset is returned.
int32_t intervalTimer_readRegister(int32_t baseAddress, int32_t offset) {
    return Xil_In32(baseAddress + offset); // uses the provided Xilinx function to read the register
}

// Helper function to write to registers.
// The argument baseAddress gives base address and the 
// argument offset gives the offset for the register to write to.
void intervalTimer_writeRegister(int32_t baseAddress, int32_t offset, int32_t value) {
    Xil_Out32((baseAddress + offset), value); // uses the provided Xilinx function to write to the register
}

// Helper function to get the base address for one of the three timers.
// The integer returned is the base address for whichever timer number was given as the argument.
// The timerNumber argument specifies which timer's address will be returned.
// If the argument given is not one of the three timers, returns 0.
uint32_t intervalTimer_getBaseAddress(uint32_t timerNumber) {
    uint32_t baseAddress;

    if (timerNumber == INTERVAL_TIMER_TIMER_0)
        baseAddress = XPAR_AXI_TIMER_0_BASEADDR;
    else if (timerNumber == INTERVAL_TIMER_TIMER_1)
        baseAddress = XPAR_AXI_TIMER_1_BASEADDR;
    else if (timerNumber == INTERVAL_TIMER_TIMER_2)
        baseAddress = XPAR_AXI_TIMER_2_BASEADDR;
    else
        baseAddress = TIMER_NUMBER_INVALID;

    return baseAddress;
}

// You must initialize the timers before you use them the first time.
// It is generally only called once but should not cause an error if it
// is called multiple times.
// timerNumber indicates which timer should be initialized.
// returns INTERVAL_TIMER_STATUS_OK if successful, INTERVAL_TIMER_STATUS_FAIL otherwise.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber) {
    uint32_t baseAddress = intervalTimer_getBaseAddress(timerNumber); // get the base address based on the timerNumber argument
    if (baseAddress == 0) // if the timerNumber argument was invalid, return INTERVAL_TIMER_STATUS_FAIL
        return INTERVAL_TIMER_STATUS_FAIL;

    intervalTimer_writeRegister(baseAddress, TCSR0_OFFSET, CLEAR_REGISTER); // write 0 to the TCSR0 register
    intervalTimer_writeRegister(baseAddress, TCSR1_OFFSET, CLEAR_REGISTER); // write 0 to the TCSR1 register
    intervalTimer_writeRegister(baseAddress, TCSR0_OFFSET, ((intervalTimer_readRegister(baseAddress, TCSR0_OFFSET) | CASC_MASK) & ~UDT0_MASK)); // set the CASC bit and clear the UCT0 bit in the TCSR0 register by reading the value and using a mask to set the 11th bit to 1 and 1st bit to 0
    
    if ((intervalTimer_readRegister(baseAddress, TCSR0_OFFSET) == CASC_MASK) && (intervalTimer_readRegister(baseAddress, TCSR1_OFFSET) == CLEAR_REGISTER)) // read the TCSR0 and TCSR1 registers to check for the values we wrote to them
        return INTERVAL_TIMER_STATUS_OK;
    else 
        return INTERVAL_TIMER_STATUS_FAIL;
}

// This is a convenience function that initializes all interval timers.
// Simply calls intervalTimer_init() on all timers.
// returns INTERVAL_TIMER_STATUS_OK if successful, INTERVAL_TIMER_STATUS_FAIL otherwise.
intervalTimer_status_t intervalTimer_initAll() {
    if (intervalTimer_init(INTERVAL_TIMER_TIMER_0) && intervalTimer_init(INTERVAL_TIMER_TIMER_1) && intervalTimer_init(INTERVAL_TIMER_TIMER_2)) // initialize all three timers and check that each status returns as OK
        return INTERVAL_TIMER_STATUS_OK;
    else
        return INTERVAL_TIMER_STATUS_FAIL;
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
void intervalTimer_start(uint32_t timerNumber) {
    uint32_t baseAddress = intervalTimer_getBaseAddress(timerNumber); // get the base address based on the timerNumber argument

    intervalTimer_writeRegister(baseAddress, TCSR0_OFFSET, (intervalTimer_readRegister(baseAddress, TCSR0_OFFSET) | ENT0_MASK)); // sets the ENT0 bit in the TCSR0 register by reading the value and using a mask to set the 7th bit to 1
    intervalTimer_writeRegister(baseAddress, TCSR0_OFFSET, (intervalTimer_readRegister(baseAddress, TCSR0_OFFSET) & ~LOAD_MASK)); // clears the LOAD0 bit in the TCSR0 register by reading the value and using a mask to set the 5th bit to 0
    intervalTimer_writeRegister(baseAddress, TCSR1_OFFSET, (intervalTimer_readRegister(baseAddress, TCSR1_OFFSET) & ~LOAD_MASK)); // clears the LOAD1 bit in the TCSR1 register by reading the value and using a mask to set the 5th bit to 0
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.
void intervalTimer_stop(uint32_t timerNumber) {
    uint32_t baseAddress = intervalTimer_getBaseAddress(timerNumber); // get the base address based on the timerNumber argument

    intervalTimer_writeRegister(baseAddress, TCSR0_OFFSET, (intervalTimer_readRegister(baseAddress, TCSR0_OFFSET) & ~ENT0_MASK)); // clears the ENT0 bit in the TCSR0 register by reading the value and using a mask to set the 7th bit to 0
}

// This function is called whenever you want to reuse an interval timer.
// For example, say the interval timer has been used in the past, the user
// will call intervalTimer_reset() prior to calling intervalTimer_start().
// timerNumber indicates which timer should reset.
void intervalTimer_reset(uint32_t timerNumber) {
    uint32_t baseAddress = intervalTimer_getBaseAddress(timerNumber); // get the base address based on the timerNumber argument

    intervalTimer_writeRegister(baseAddress, TLR0_OFFSET, CLEAR_REGISTER); // writes 0 to the TLR0 register
    intervalTimer_writeRegister(baseAddress, TCSR0_OFFSET, (intervalTimer_readRegister(baseAddress, TCSR0_OFFSET) | LOAD_MASK)); // sets the LOAD0 bit in the TCSR0 register by reading the value and using a mask to set the 5th bit to 1
    intervalTimer_writeRegister(baseAddress, TRL1_OFFSET, CLEAR_REGISTER); // writes 1 to the TLR1 register
    intervalTimer_writeRegister(baseAddress, TCSR1_OFFSET, (intervalTimer_readRegister(baseAddress, TCSR1_OFFSET) | LOAD_MASK)); // sets the LOAD1 bit in the TCSR1 register by reading the value and using a mask to set the 5th bit to 1
}

// Convenience function for intervalTimer_reset().
// Simply calls intervalTimer_reset() on all timers.
void intervalTimer_resetAll(){
    intervalTimer_reset(INTERVAL_TIMER_TIMER_0); // reset timer 0
    intervalTimer_reset(INTERVAL_TIMER_TIMER_1); // reset timer 1
    intervalTimer_reset(INTERVAL_TIMER_TIMER_2); // reset timer 2
}

// Runs a test on a single timer as indicated by the timerNumber argument.
// Returns INTERVAL_TIMER_STATUS_OK if successful, something else otherwise.
// This function is not in use, so it automatically returns OK.
intervalTimer_status_t intervalTimer_test(uint32_t timerNumber) {
    return INTERVAL_TIMER_STATUS_OK; // this function is not in use, so it automatically returns OK
}

// Convenience function that invokes test on all interval timers.
// Returns INTERVAL_TIMER_STATUS_OK if successful, INTERVAL_TIMER_STATUS_FAIL otherwise.
intervalTimer_status_t intervalTimer_testAll() {
    if (intervalTimer_test(INTERVAL_TIMER_TIMER_0) && intervalTimer_test(INTERVAL_TIMER_TIMER_1) && intervalTimer_test(INTERVAL_TIMER_TIMER_2)) // tests all three timers and checks that each status returns as OK
        return INTERVAL_TIMER_STATUS_OK;
    else
        return INTERVAL_TIMER_STATUS_FAIL;
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber) {
    uint32_t baseAddress = intervalTimer_getBaseAddress(timerNumber); // get the base address based on the timerNumber argument

    uint64_t timerAllBits = intervalTimer_readRegister(baseAddress, TCR1_OFFSET); // store the value of the upper 32 bits in timerAllBits
    timerAllBits = ((timerAllBits << TIMER_WIDTH) | intervalTimer_readRegister(baseAddress, TCR0_OFFSET)); // shift the upper 32 bits into the proper position and read the lower 32 bits
    double timerDuration =  (double) timerAllBits / XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ; // divide the value of the 64 bit counter by the frequency to get seconds
    return timerDuration;
}