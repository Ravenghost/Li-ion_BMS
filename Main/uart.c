#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"


/*
   constants and macros
 */

// size of RX/TX buffers
#define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)
#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)

#define UART0_RECEIVE_INTERRUPT   USART_RX_vect
#define UART0_TRANSMIT_INTERRUPT  USART_UDRE_vect
#define UART0_STATUS      UCSR0A
#define UART0_CONTROL     UCSR0B
#define UART0_CONTROLC    UCSR0C
#define UART0_DATA        UDR0
#define UART0_UDRIE       UDRIE0
#define UART0_UBRRL       UBRR0L
#define UART0_UBRRH       UBRR0H
#define UART0_BIT_U2X     U2X0
#define UART0_BIT_RXCIE   RXCIE0
#define UART0_BIT_RXEN    RXEN0
#define UART0_BIT_TXEN    TXEN0
#define UART0_BIT_UCSZ0   UCSZ00
#define UART0_BIT_UCSZ1   UCSZ01



/*
   module global variables
 */
static volatile uint8_t UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile uint8_t UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile uint8_t UART_TxHead;
static volatile uint8_t UART_TxTail;
static volatile uint8_t UART_RxHead;
static volatile uint8_t UART_RxTail;
static volatile uint8_t UART_LastRxError;




ISR (UART0_RECEIVE_INTERRUPT)	
/*
   UART Receive Complete interrupt
   called when the UART has received a character
*/
{
    uint8_t tmphead;
    uint8_t data;
    uint8_t usr;
    uint8_t lastRxError;
 
 
    // read UART status register and UART data register
    usr  = UART0_STATUS;
    data = UART0_DATA;
    
    // get FEn (Frame Error) DORn (Data OverRun) UPEn (USART Parity Error) bits
#if defined(FE) && defined(DOR) && defined(UPE)
    lastRxError = usr & (_BV(FE)|_BV(DOR)|_BV(UPE) );
#elif defined(FE0) && defined(DOR0) && defined(UPE0)
    lastRxError = usr & (_BV(FE0)|_BV(DOR0)|_BV(UPE0) );
#elif defined(FE1) && defined(DOR1) && defined(UPE1)
    lastRxError = usr & (_BV(FE1)|_BV(DOR1)|_BV(UPE1) );
#elif defined(FE) && defined(DOR)
    lastRxError = usr & (_BV(FE)|_BV(DOR) );
#endif

    // calculate buffer index
    tmphead = (UART_RxHead + 1) & UART_RX_BUFFER_MASK;
    
    if (tmphead == UART_RxTail)
	{
        // error: receive buffer overflow
        lastRxError = UART_BUFFER_OVERFLOW >> 8;
    }
	else
	{
        // store new index
        UART_RxHead = tmphead;
        // store received data in buffer
        UART_RxBuf[tmphead] = data;
    }
    UART_LastRxError |= lastRxError;  
}


ISR(UART0_TRANSMIT_INTERRUPT)
/*
   UART Data Register Empty interrupt
   called when the UART is ready to transmit the next byte
*/
{
    uint8_t tmptail;

    
    if (UART_TxHead != UART_TxTail) 
	{
        // calculate and store new buffer index
        tmptail = (UART_TxTail + 1) & UART_TX_BUFFER_MASK;
        UART_TxTail = tmptail;
        // get one byte from buffer and write it to UART
        UART0_DATA = UART_TxBuf[tmptail];  // start transmission
    }
	else
	{
        // tx buffer empty, disable UDRE interrupt
        UART0_CONTROL &= ~_BV(UART0_UDRIE);
    }
}


/*
  initialize UART and set baudrate
  baudrate using macro UART_BAUD_SELECT()
*/
void uart_init(uint16_t baudrate)
{
    UART_TxHead = 0;
    UART_TxTail = 0;
    UART_RxHead = 0;
    UART_RxTail = 0;


    // Set baud rate
    if (baudrate & 0x8000)
    {
        #if UART0_BIT_U2X
        UART0_STATUS = (1<<UART0_BIT_U2X);  //Enable 2x speed 
        #endif
    } 
    #if defined(UART0_UBRRH)
    UART0_UBRRH = (uint8_t)((baudrate>>8) & 0x80) ;
    #endif    
    UART0_UBRRL = (uint8_t)(baudrate & 0x00FF);
      
    // Enable USART receiver and transmitter and receive complete interrupt
    UART0_CONTROL = _BV(UART0_BIT_RXCIE)|(1<<UART0_BIT_RXEN)|(1<<UART0_BIT_TXEN);
    
    // Set frame format = asynchronous, 8data, no parity, 1stop bit
    #ifdef UART0_CONTROLC
    #ifdef UART0_BIT_URSEL
    UART0_CONTROLC = (1<<UART0_BIT_URSEL)|(1<<UART0_BIT_UCSZ1)|(1<<UART0_BIT_UCSZ0);
    #else
    UART0_CONTROLC = (1<<UART0_BIT_UCSZ1)|(1<<UART0_BIT_UCSZ0);
    #endif 
    #endif

}


/*
   return byte from ringbuffer  
   lower byte =  received byte from ringbuffer
   higher byte = last receive error
*/
uint16_t uart_getc(void)
{    
    uint8_t tmptail;
    uint8_t data;
    uint8_t lastRxError;


    if (UART_RxHead == UART_RxTail)
	{
        return UART_NO_DATA;   // no data available
    }
    
    // calculate buffer index
    tmptail = (UART_RxTail + 1) & UART_RX_BUFFER_MASK;
    
    // get data from receive buffer
    data = UART_RxBuf[tmptail];
    lastRxError = UART_LastRxError;
    
    // store buffer index
    UART_RxTail = tmptail; 
    
    UART_LastRxError = 0;
    return (lastRxError<<8) + data;

}


/*
   Write byte to ringbuffer for transmitting via UART
   Input = byte to be transmitted
*/
void uart_putc(uint8_t data)
{
    uint8_t tmphead;

    
    tmphead  = (UART_TxHead + 1) & UART_TX_BUFFER_MASK;
	
    // wait for free space in buffer
    while (tmphead == UART_TxTail);
    
	UART_TxBuf[tmphead] = data;
	UART_TxHead = tmphead;

    // enable UDRE interrupt
    UART0_CONTROL    |= _BV(UART0_UDRIE);

}


/*
   Transmit string to UART
   Input = string to be transmitted
*/
void uart_puts(const char *s)
{
    while (*s) uart_putc(*s++);

}


/*
   Transmit string from program memory to UART
   Input = program memory string to be transmitted
*/
void uart_puts_p(const char *progmem_s)
{
    register char c;
    
    while ( (c = pgm_read_byte(progmem_s++)) ) uart_putc(c);

}