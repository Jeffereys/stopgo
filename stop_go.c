// Stop Go C Example (Bitbanding)
// Jason Losh
// https://imgur.com/a/ZnIM0q6
//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// Red LED:
//   PF1 drives an NPN transistor that powers the red LED
// Green LED:
//   PF3 drives an NPN transistor that powers the green LED
// Pushbutton:
//   SW1 pulls pin PF4 low (internal pull-up is used)

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "clock.h"
#include "tm4c123gh6pm.h"
#include "wait.h"

// Bitband aliases
//Port D
#define RED_LED         (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 0*4)))
#define GREEN_LED       (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 1*4)))
#define YELLOW_LED      (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 2*4)))
#define BLUE_LED        (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 3*4)))

//Port E
#define PUSH_BUTTON_1  (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 1*4)))
#define PUSH_BUTTON_2  (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 2*4)))


// PortD masks
#define RED_LED_MASK    1
#define GREEN_LED_MASK  2
#define YELLOW_LED_MASK 4
#define BLUE_LED_MASK   8

//PortE MASKS
#define PUSH_BUTTON_1_MASK 2
#define PUSH_BUTTON_2_MASK 4


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Blocking function that returns only when SW1 is pressed
void waitPb_1_Press(void)
{
    while(PUSH_BUTTON_1);
}
void waitPb_2_Press(void)
{
    while(!PUSH_BUTTON_2);
}

// Initialize Hardware
void initHw(void)
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();
    // Enable clocks
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3 | SYSCTL_RCGCGPIO_R4;
    _delay_cycles(3);                                                                       // required Delay
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Configure LED and pushbutton pins
    GPIO_PORTD_DIR_R |= GREEN_LED_MASK | RED_LED_MASK | YELLOW_LED_MASK | BLUE_LED_MASK;                        // bits 1 and 3 are outputs, other pins are inputs
    GPIO_PORTE_DIR_R &= ~PUSH_BUTTON_1_MASK | ~PUSH_BUTTON_2_MASK ;                                    // bit 4 is an input

    GPIO_PORTD_DR2R_R |= GREEN_LED_MASK | RED_LED_MASK| YELLOW_LED_MASK| BLUE_LED_MASK;     // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTD_DEN_R  |= GREEN_LED_MASK | RED_LED_MASK| YELLOW_LED_MASK| BLUE_LED_MASK;     // enable LEDs

    GPIO_PORTE_DEN_R  |= PUSH_BUTTON_1_MASK | PUSH_BUTTON_2_MASK;                            //enable push buttons

    GPIO_PORTE_PUR_R    |= PUSH_BUTTON_1_MASK;                                               // enable internal pull-up for push button
    GPIO_PORTE_PDR_R    |= PUSH_BUTTON_2_MASK;                                               // enable internal pull-DOWN for push button
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------


int main(void)
{
    // Initialize hardware
    initHw();

    //Turn off ALL LEDS
    RED_LED = 0;
    YELLOW_LED = 1;
    //BLUE_LED = 0;
    GREEN_LED = 0;

    //ENable RED_LED
    RED_LED     = 1;
    YELLOW_LED  = 1;
    //BLUE_LED    = 0;
    GREEN_LED   = 0;

    //WAIT for push button 2
    waitPb_2_Press();

    //TURN off red TURN ON GREEN
    RED_LED     = 0;
    GREEN_LED   = 1;

    //wait 1 second
    waitMicrosecond(1000000);

    //turn on BLUE
    BLUE_LED = 1;

    //WAIT push button 1
    waitPb_1_Press();

    //wait half a second
    waitMicrosecond(1000000/2);

    // Toggle YELLOW LED every second
    while(true)
    {
        YELLOW_LED ^= 1;
        waitMicrosecond(1000000/2);
    }

}
