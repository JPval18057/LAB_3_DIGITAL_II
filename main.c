/*
 * File:   main.c
 * Author: Juan Pablo Valenzuela
 * LABORATORIO 2
 * Created on january 28th 2021, 07:28 PM
 */

//**************************************************************
// Include libraries
#include <xc.h>
#include "adc_lib.h"
//**************************************************************


// Configuration bits
//**************************************************************
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//**************************************************************
//Function prototypes
void show_numbers(void);
void read_value(void);
//**************************************************************

//GLOBAL VARIABLES
unsigned int interrupts=0;
// SCREEN VARIABLES (7 SEGMENTS)
unsigned char screen[16] = {0b00111111,0b00000110,0b01011011,0b01001111,0b01100110,0b01101101,0b01111101,0b00000111,0b11111111,0b01101111,0b01110111,0b01111100,0b00111001,0b01011110,0b01111001,0b01110001};
// ADC MECHANICS
int ref_value = 128;
int ADC_value = 0;
// COUNTER SYSTEM
unsigned char counter = 0; //counter value
unsigned char nibble_1=0; //1st nibble
unsigned char nibble_2=0; //2nd nibble
//PORTB DEBOUNCING
unsigned char boton1 = 0;
unsigned char boton2 = 0;
//**************************************************************


//Interrupt subroutine
void __interrupt() ISR(void) {
    //CHECK THE TIMER FLAG
    if(INTCONbits.T0IF==1){
        INTCONbits.T0IF = 0; //SHUT DOWN THE FLAG
        interrupts++; //EVERY 25 INTERRUPTS ARE 100MS
        if(interrupts==250){ //EVERY 1000ms it resets
            interrupts=0;//RESET THE INTERRUPT COUNTER
        }
    }
    //CHECK THE PORTB FLAG
    if (INTCONbits.RBIF==1){
        INTCONbits.RBIF = 0;//TURN OFF THE FLAG
        if(PORTBbits.RB0==1){
            boton1 = 1;
        }
        if (PORTBbits.RB1==1){
            boton2 = 1;
        }
        
    }
    //CHECK THE ADC FLAG
    if (PIR1bits.ADIF==1){
        PIR1bits.ADIF = 0; //SHUT DOWN THE ADC FLAG
        ADC_value = ADRESH; //STORE THE CONVERSION VALUE INTO VARIABLE
    }
}

//**************************************************************
//MAIN CODE CONFIGURATION
//**************************************************************
void setup(void) {
    /*
     I will put the pot on RA0/AN0
     The increment button on RB0 PULLDOWN
     The decrement button on RB1 PULLDOWN
     The 7 segment on PORTC
     Reference counter, PORTD
     alarm in RB7
     RB5 AND RB6 CONTROL THE SCREENS
     */
    //CONFIGURING I/O
    TRISB = 0b00011111; //PORTB AS INPUT except RB7, RB6 AND RB5 (OUTPUT)
    TRISA = 255; //PORTA AS AN INPUT
    ANSEL = 1; //PORTA PIN 0 AS AN ANALOG INPUT, THE REST AS A DIGITAL INPUT
    ANSELH = 0; //THE REST OF THE CHANNELS ARE DIGITAL
    TRISC = 0; //PORTC AS OUTPUT
    TRISD = 0; //PORTD AS OUTPUT
    
    //PORTS RESET
    PORTC = 0;
    PORTD = 0;
    PORTBbits.RB7 = 0; //turn off the alarm
    //PORTDbits.RD0 = 1;
    
    //CONFIGURING OSCILLATOR
    OSCCON = 0b01110101; //I USE THE INTERNAL OSC @ 8MHz
    //TIMER0 CONFIG
    OPTION_REG = 0b10010100; //CONFIGURE THE PRESCALER AT 32 WITH INT CLOCK
    TMR0 = 6; 
    /*
     USING 8MHZ YOU NEED TO SET THE TMR0 REG AT 6
     * THEN YOU NEED A PRESCALER OF 32
     * YOUR PERIOD IS 4MS (EACH CYCLE)
     * FOR 100MS TIMER YOU NEED 25 CYCLES
     */
    //CONFIGURING Interrupts
    ei(); //activate all the interrupts
    INTCONbits.T0IE = 1; //TURN ON THE TIMER0 INTERRUPT
    INTCONbits.T0IF = 0; //SHUT DOWN THE INTERRUPT FLAG
    //PORTB INTERRUPTS
    INTCONbits.RBIF = 0; //TURN OFF THE PORTB FLAG
    INTCONbits.RBIE = 1; //TURN ON THE PORTB INTERRUPT
    IOCBbits.IOCB0 = 1; //TURN ON RB0 INTERRUPTS
    IOCBbits.IOCB1 = 1; //TURN ON RB1 INTERRUPTS
    
    ADC_config(); //Configuration of ADC
    //BY DEFAULT THE SCREEN 1 IS THE ONE THAT TURNS ON
    PORTBbits.RB5 = 1; //turn on screen 1
    PORTBbits.RB6 = 0; //TURN OFF SCREEN 2
    return;
}
//**************************************************************
// MAIN CODE
//**************************************************************
void main(void) {
    setup();    
    while(1){ //This is the main code loop
//        PORTBbits.RB5 = 1;
//        PORTBbits.RB6 = 1;
        if (boton1==1){ //this activates when the button is pressed and has just released
            if (PORTBbits.RB0==0){
                counter++;
                boton1 = 0;
            }
        }
        if (boton2==1){ //when the button is pressed and releases, that instant this code will execute
            if (PORTBbits.RB1==0){ 
                counter--;
                boton2 = 0;
            }
        }
        //DISPLAY THE COUNTER IN PORTD
        PORTD = counter;
        //Read the value of analog voltage every 100ms
        if (interrupts%10==0){ //en los múltiplos de 10 se va a leer (cada 40ms)
            read_value();
        }        
        show_numbers();
        //Comparing if the reference value was suprassed
        if (counter<=ADC_value){ //If the reference is equal or less than the ADC value then the alarm gets on
            PORTBbits.RB7 = 1;
        } else {
            PORTBbits.RB7 = 0;
        }
        }    
    return;
}

//**************************************************************
//SUBROUTINE TO SHOW THE NUMBERS
//**************************************************************
void show_numbers(void){
    //First we extract nibble 1
    nibble_1 = ADC_value & 0b00001111;
    //Then we extract nibble 2
    nibble_2 = (ADC_value>>4)&0b00001111;
    //Finally we show the number on screen    

    if (interrupts%2==0){
        PORTC = screen[nibble_2];
        PORTBbits.RB5 = 1; //turn on screen 1
        PORTBbits.RB6 = 0; //TURN OFF SCREEN 2
        
    }
    if (interrupts%2==1){ 
        PORTC = screen[nibble_1];
        PORTBbits.RB5 = 0; //turn off screen 1
        PORTBbits.RB6 = 1; //TURN ONN SCREEN 2
        
    }
    
}
//**************************************************************


/*
 -------------------------------------------------------------------------------
 PORTC NUMBER CODING
 * 0: 0b00111111 *completo
 * 1: 0b00000110 *completo
 * 2: 0b01011011 *completo
 * 3: 0b01001111 *completo
 * 4: 0b01100110 *completo
 * 5: 0b01101101 *completo
 * 6: 0b01111101 //es opcional si se usa aca *listo
 * 7: 0b00000111 *completo
 * 8: 0b11111111 *completo
 * 9: 0b01101111 *completo
 * A: 0b01110111 *completo
 * B: 0b01111100 *completo 
 * C: 0b00111001 *completo
 * D: 0b01011110 *completo
 * E: 0b01111001 *completo
 * F: 0b01110001 *completo
 
 
 
 -------------------------------------------------------------------------------
 */

