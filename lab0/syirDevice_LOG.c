/* ************************************************************************* */
/*                                                                           */
/* SYIR - Systemy Internetu Rzeczy LAB                                       */
/*                                                                           */
/* Autorzy:  Arkadiusz Luczyk; Jakub Jasinski; Marek Niewiński               */
/* Email  :  arkadiusz.luczyk@pw.edu.pl                                      */
/*           jakub.jasinski@pw.edu.pl                                        */
/*           marek.niewinski@pw.edu.pl                                       */
/*                                                                           */
/*     Do wyłącznego użytku w ramach zajęć SYIR                              */
/*     Do wylacznego uzytku w ramach zajec SYIR                              */
/*                                                                           */
/* ************************************************************************* */

#include "syirDevice.h"

typedef struct syirDevice_StatusLog {
    volatile uint8_t   state;
             char    * end;
             char    * ptr;
} syirDevice_StatusLog_t;

syirDevice_StatusLog_t statusLog;

void syirDevice_INIT_log (void)
{
    statusLog.state = 0;
    statusLog.ptr   = NULL;
    statusLog.end   = NULL;
    cli();
    {
        /* Wlaczenie portu C bitu 3 do nadawania  */
        PORTC.DIRSET |= (1<<3);
        PORTC.DIR    |= (1<<3);
        PORTC.OUT    |= (1<<3);
        
        /* USARTC0 - interfejs USART kontrolujacy wyjscia/wejscia portu C*/
        USARTC0.CTRLB    |= (1<<3);                 // Ustawienie USARTU do nadawania
        USARTC0.CTRLC     = (0b11 << 0);            // 8-bits data length, no parity, one stop bit
        USARTC0.CTRLA    |= (0b10 << 1);            // TX interrupt level = LOW
    }
    sei();
}

int8_t syirDevice_SET_log(int8_t BSCALE, uint16_t  BSEL, uint8_t CLK2X) {
    
    USART_t *PORT = &USARTC0;

    if (BSCALE < -7)
        return 0;
    if (BSCALE > 7)
        return 0;
    if (BSEL > 4096)
        return 0;
    if (CLK2X > 1)
        return 0;
    
    cli();
    {
        if (CLK2X > 0)
            PORT->CTRLB |= (1 << 0);
        else
            PORT->CTRLB &= 0b11111011;
            
        PORT->BAUDCTRLB = ((BSCALE << 4) | (uint8_t)(BSEL >> 8));
        PORT->BAUDCTRLA = (uint8_t)(BSEL);
    }
    sei();
    return 1;
}

int8_t syirDevice_SEND_log (char *buf, uint16_t num) {
    
    switch (statusLog.state) {
        case 0: {
            if (num == 0)
                return 1;
            if (buf == NULL)
                return 1;
            if (*buf == 0)
                return 1;
           
            cli();
            {
                statusLog.state = 1; 
                USARTC0.DATA    = *buf;
            }
            sei();
            statusLog.ptr   = buf + 1;
            statusLog.end   = buf + num;
            return 0;
        }
        case 2: {
            statusLog.state = 0; 
            if (statusLog.ptr == statusLog.end)
                return 1;
            if (*statusLog.ptr == 0)
                return 1;
            
            cli();
            {
                statusLog.state = 1; 
                USARTC0.DATA    = *statusLog.ptr;
            }
            sei();           
            statusLog.ptr++;
            return 0;
        }
        default: 
            return 0;
    }
    return 0;
}

/* Procedura obslugi przerwania - USARTC nadawanie */
ISR(USARTC0_TXC_vect)
{
    statusLog.state++;
}

