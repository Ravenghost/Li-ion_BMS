#ifndef UART_H
#define UART_H

#include <avr/pgmspace.h>

/*
   constants and macros
*/


/* UART Baudrate Expression
   xtalCpu  system clock in Mhz      
   baudRate baudrate in bps     
 */
#define UART_BAUD_SELECT(baudRate,xtalCpu)  ( ((xtalCpu) + 8UL * (baudRate)) / (16UL * (baudRate)) -1UL)

/* UART Baudrate Expression for ATmega double speed mode
   xtalCpu  system clock in Mhz        
   baudRate baudrate in bps   
 */
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) ( ((((xtalCpu) + 4UL * (baudRate)) / (8UL * (baudRate)) -1UL)) | 0x8000)

/*
   brief  Size of the circular receive buffer, must be power of 2
 */
#ifndef UART_RX_BUFFER_SIZE
#define UART_RX_BUFFER_SIZE 32
#endif

/*
   Size of the circular transmit buffer, must be power of 2 
 */
#ifndef UART_TX_BUFFER_SIZE
#define UART_TX_BUFFER_SIZE 32
#endif

/* 
   high byte error return code of uart_getc()
*/
#define UART_FRAME_ERROR      0x1000              // Framing Error by UART
#define UART_OVERRUN_ERROR    0x0800              // Overrun condition by UART
#define UART_PARITY_ERROR     0x0400              // Parity Error by UART
#define UART_BUFFER_OVERFLOW  0x0200              // Receive ringbuffer overflow
#define UART_NO_DATA          0x0100              // No receive data available


/*
   function prototypes
*/

/*
   Initialize UART and set baudrate 
   Input = baudrate Specify baudrate using macro UART_BAUD_SELECT()
*/
extern void uart_init(uint16_t baudrate);


/*
   Get received byte from ringbuffer

  Returns in the lower byte the received character and in the 
  higher byte the last receive error.
  UART_NO_DATA is returned when no data is available.
 
   return  lower byte:  received byte from ringbuffer
   return  higher byte: last receive status
           -  0 successfully received data from UART
           -  UART_NO_DATA           
			  no receive data available
           -  UART_BUFFER_OVERFLOW   
			  Receive ringbuffer overflow.
              We are not reading the receive buffer fast enough, 
              one or more received character have been dropped 
           -  UART_OVERRUN_ERROR     
              Overrun condition by UART.
              A character already present in the UART UDR register was 
              not read by the interrupt handler before the next character arrived,
              one or more received characters have been dropped.
           -  UART_FRAME_ERROR       
              Framing Error by UART
 */
extern uint16_t uart_getc(void);


/*
   Put byte to ringbuffer for transmitting via UART
   Input = data byte to be transmitted
 */
extern void uart_putc(uint8_t data);


/*
   Put string to ringbuffer for transmitting via UART
   
   The string is buffered by the uart library in a circular buffer
   and one character at a time is transmitted to the UART using interrupts.
   Blocks if it can not write the whole string into the circular buffer.
  
   Input = s string to be transmitted
 */
extern void uart_puts(const char *s);


/*
   Put string from program memory to ringbuffer for transmitting via UART.
 
   The string is buffered by the uart library in a circular buffer
   and one character at a time is transmitted to the UART using interrupts.
   Blocks if it can not write the whole string into the circular buffer.
 
   Input = s program memory string to be transmitted
   see      uart_puts_P
 */
extern void uart_puts_p(const char *s);

/*
   Macro to automatically put a string constant into program memory
 */
#define uart_puts_P(__s)       uart_puts_p(PSTR(__s))

#endif // UART_H 

