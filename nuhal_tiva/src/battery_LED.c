/// Adapted from led example
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "inc/hw_types.h"

//minimum battery volt% for each color. Red if below YELLOW_THRESH.
//will inform values with testing
#define YELLOW_THRESH 0.95
#define GREEN_THRESH 0.98

//The TIVA ADC only reads up to 3.3V, assume the battery voltage 
//has been somehow scaled from 12V to 3.3V
#define ADC_V_REF 3.3 //reference voltage
#define ADC_SEQ_NO 0 //sequencer number
#define ADC_PRIORITY 0 //priority

//Number of times per second to check voltage
#define READS_PER_SEC 2


// interrupt for Timer0A
void Timer0AISR(void)
{
    //on interrupt, check the value of pin E3, determine the color required
    //Start the ADC conversion
    TimerIntClear(TIMER0_BASE, TIMER_A);
    ADCProcessorTrigger(ADC0_BASE, ADC_SEQ_NO);

    //wait for conversion to finish
    while(!ADCIntStatus(ADC0_BASE, ADC_SEQ_NO, false)){
        ;
    }

    //clear ADC interrupt
    ADCIntClear(ADC0_BASE, ADC_SEQ_NO);

    //read ADC
    int32_t adc_val;
    ADCSequenceDataGet(ADC0_BASE, ADC_SEQ_NO, &adc_val);
    float voltage_reading = (adc_val * ADC_V_REF) / 4096;

    if(voltage_reading > ADC_V_REF * GREEN_THRESH){
        //green
        GPIOPinWrite(GPIO_PORTD_BASE,
                    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
                    GPIO_PIN_1);
    }
    else if(voltage_reading > ADC_V_REF * YELLOW_THRESH){
        //yellow (blue for now)
        GPIOPinWrite(GPIO_PORTD_BASE,
                    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 
                    GPIO_PIN_2);
    }
    else{
        //red
        GPIOPinWrite(GPIO_PORTD_BASE,
                    GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 
                    GPIO_PIN_3);
    }
}

int main(void)
{
    // Setup clock frequency to 80MHz
    SysCtlClockSet(
        SYSCTL_OSC_MAIN        // Use the main oscillator
        | SYSCTL_XTAL_16MHZ   // which is a 16 MHz crystal
        | SYSCTL_USE_PLL      // Feed PLL which results in 400 MHz
        | SYSCTL_SYSDIV_2_5   // (400 MHz /2) /2.5) = 80 MHz
        );

    // Enable the ports D, E 
    // Enable ADC 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // wait for peripherals  to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)
    | !SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)
    | !SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)) 
    {
        ; 
    }

    //Set port E3 to be ADC input, D1, D2, D3 to be outputs
    //E3 is ADC input channel 0
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

    //Sample sequencer 0, highest priority
    ADCSequenceConfigure(ADC0_BASE, ADC_SEQ_NO, ADC_TRIGGER_PROCESSOR, ADC_PRIORITY);

    //E3 is ADC channel 0
    ADCSequenceStepConfigure(ADC0_BASE, ADC_SEQ_NO, 0, ADC_CTL_CH0 | ADC_CTL_IE | ADC_CTL_END);

    //Enable the sample sequencer
    ADCSequenceEnable(ADC0_BASE, ADC_SEQ_NO);
    ADCIntClear(ADC0_BASE, ADC_SEQ_NO);

    // Enable timer0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0))
    {
        ; //wait for the timer module to be ready
    }

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC_UP);
    TimerClockSourceSet(TIMER0_BASE, TIMER_CLOCK_SYSTEM);
    TimerLoadSet(TIMER0_BASE, TIMER_A, (int)(80000000/READS_PER_SEC));

    // Enable the interrupt on the timer
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    // Enable timer interrupt on the nvic
    IntEnable(INT_TIMER0A);
 
    // Enable the timer itself
    TimerEnable(TIMER0_BASE, TIMER_A);

    // enable interrupts
    IntMasterEnable();

    //reset the LED
    // GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0); //pin off
    // delay(10000);

    for(;;)
    {
        ; // loop forever
    }
}
