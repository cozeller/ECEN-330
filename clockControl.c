#include "clockControl.h"

#include <display.h>

#define ADC_COUNTER_MAX_VALUE 5 // this value should be 40 or 50ms divided by the period of the tick function, currently assuming 10ms period

// States of the clockControl state machine
enum clockControl_st_t {
	init_st,                 // Start here, transition out of this state on the first tick.
	//never_touched_st,        // Wait here until the first touch - clock is disabled until set.
	waiting_for_touch_st,    // waiting for touch, clock is enabled and running.
	adc_counter_running_st,     // waiting for the touch-controller ADC to settle.
	auto_counter_running_st,   // waiting for the auto-update delay to expire
	rate_counter_running_st,   // waiting for the rate-timer to expire to know when to perform the auto inc/dec.
	rate_counter_expired_st,   // when the rate-timer expires, perform the inc/dec function.
	add_second_to_clock_st   // add a second to the clock time and reset the ms counter.
};

static enum clockControl_st_t currentState;

// Standard tick function.
void clockControl_tick() {

    uint8_t adcCounter;

    // Perform state update first.
    // mealy actions go here as well
  switch(currentState) {
    case init_st:
        currentState = waiting_for_touch_st;
        break;
    case waiting_for_touch_st:
        if (display_isTouched()) {
            currentState = adc_counter_running_st;
            display_clearOldTouchData();
        }
      break;
    case adc_counter_running_st:
        if (display_isTouched() && (adcCounter == ADC_COUNTER_MAX_VALUE)) {
            currentState = auto_counter_running_st;
        }
        else if (!display_isTouched() && (adcCounter == ADC_COUNTER_MAX_VALUE)) {
            currentState = waiting_for_touch_st;
        }
      break;
    case auto_counter_running_st:
      break;
    case rate_counter_running_st:
      break;
    case rate_counter_expired_st:
      break;
    case add_second_to_clock_st:
      break;
    default:
      // print an error message here.
      break;
  }
  
  // Perform state action next.
  switch(currentState) {
    case init_st:
        break;
    case waiting_for_touch_st:
        adcCounter = 0;
        break;
    case adc_counter_running_st:
        adcCounter++;
        break;
    case auto_counter_running_st:
        break;
    case rate_counter_running_st:
        break;
    case rate_counter_expired_st:
        break;
    default:
        // print an error message here.
        break;
  }  
}

// Call this before you call clockControl_tick().
void clockControl_init() {
    currentState = init_st;
}