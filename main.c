/*****************************************************************************
* Eastlogic Digital Lab.
*
* File              : main.c
* Compiler          : IAR EWAAVR ver.5.3
* Revision          : Rev 1.0
* Date              : Date: 10-08-2021
* Author            : Reznikov Michael
*
* Support mail      : eastlogic@yandex.ru
*
* Device            : AtMega32 and modifications, Fclk = 8 MHz
* AppNote           : no
*
* Description       : V7-38 UART PC Link Interface Module
****************************************************************************/
#include "iom32.h"
#include "intrinsics.h"
#include "global_def.h"       // Ãëîáàëüíûå ìàêðîîïðåäåëåíèÿ ïðîåêòà

/*
Tint  -> PD2 (INT0)
R1    -> PA0
R2    -> PA1
R3    -> PA2
Sign  -> PA3
TXD   -> PD1
*/

void UART_send(unsigned char symb)
{
  while (!(UCSRA & (1<<UDRE)));
  UDR = symb;
}


unsigned long Tint_us = 0;  // Measured integration time, us
unsigned char byte;


void main(void)
{
// -------- Ports init -------------------------------------------------
DDRD  |= 1<<PD1;      // PD1 -> TXD line

DDRA &= ~0x0F;        // PA3...PA0 -> floating inputs
PORTA &= ~0x0F;

DDRD &= ~(1<<PD2);    // PD2 (INT0) -> floating input
PORTA &= ~(1<<PD2);    
// -------- External interrupt init ------------------------------------
MCUCR |= (1<<ISC00);    // INT0 logic change generates interrupt request
GICR  |= (1<<INT0);     // INT0 Interrupt Request Enable
    
// -------- Timer init -------------------------------------------------
TIMSK |= 1<<TOIE1;      // Timer 1 Overflow Enable
// -------- Uart init --------------------------------------------------
UBRRL = 51;             // 9600 baud, 8 bits, 1 stop bit, no parity ctrl
UCSRB |= 1<<TXEN;
  
    asm("sei");

    while(1)
    {       
            // Sending of result ASCII package 
            if (Tint_us)                  // Non-zero value -> measured time is ready
            {
              
            UART_send('S');               // Start byte

            // Measurement Range definition            
            byte = (~(PINA & 0x07) & 0x07) + 0x30;                     
            UART_send(byte);
            
            byte = PINA & 1<<3;             // Sign definition
            if (byte) UART_send('+');
            else      UART_send('-');
            
            // Tint send in ASCII
            UART_send((Tint_us/100000)%10+0x30);
            UART_send((Tint_us/10000)%10+0x30);
            UART_send((Tint_us/1000)%10+0x30);
            UART_send((Tint_us/100)%10+0x30);
            UART_send((Tint_us/10)%10+0x30);
            UART_send(Tint_us%10+0x30);
        
            UART_send(0x0d);              // Carriage Return
            UART_send(0x0a);              // Line Feed
          
            Tint_us = 0;                  // Clear, as time value has been sent
            }    
    }
  
}


unsigned char Tim1OvfNumber = 0;   // Number of Timer 1 Overflows


// --------- External (INT0) Interrupt Handler -------------------------
// --------- Requested by INT0 pin logic level change ------------------
#pragma vector=INT0_vect
__interrupt void INT0_OVF(void)
{
unsigned char p_state;
  
p_state = PIND;

    if (!(p_state & (1<<PD2)))  // Low logic level on INT0
    {                           // (falling edge detected)
                                // Begin of measurement interval        

    TIM1_START;                 // Timer 1 start    
    }
    else                        // High logic level on INT0
    {                           // (rising edge detected)
                                // End of measurement interval    
    TIM1_STOP;
    
      Tint_us = 0xFFFF;
      Tint_us *= Tim1OvfNumber;
      Tint_us += TCNT1;
      Tint_us *= TIM1_DIV;
      Tint_us /= ClkFreqMHz;
      
    // Clear Timer 1 and number of overflows    
    TCNT1 = 0;
    Tim1OvfNumber = 0;
    }

}
//----------------------------------------------------------------------




// --------- Timer 1 Overflow Interrupt Handler ------------------------
#pragma vector=TIMER1_OVF_vect
__interrupt void TIMER1_OVF(void)
{

Tim1OvfNumber++;   

}
//----------------------------------------------------------------------

