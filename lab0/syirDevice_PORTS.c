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

/* Wylaczenie wszystkich portow */
void syirDevice_OFF_ports(void) 
{
    PORT_t *PORT[7] = {&PORTA,&PORTB,&PORTC,&PORTD,&PORTE,&PORTF,&PORTR};
	uint8_t port_no,pin_no;
	
    {
        for (port_no=0;port_no<7;port_no++)
        {
            PORT[port_no]->OUT      = 0b00000000;
            PORT[port_no]->DIR      = 0b00000000;

            for (pin_no=0;pin_no<8;pin_no++)
                *(&PORT[port_no]->PIN0CTRL+pin_no) = PORT_OPC_TOTEM_gc;
        }
    }
	return;
}

