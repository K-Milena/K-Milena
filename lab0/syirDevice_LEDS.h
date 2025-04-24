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

#ifndef SYIRDEVICE_LEDS_H
#define SYIRDEVICE_LEDS_H

typedef enum {
    GREEN   = (1<<1),
    ORANGE  = (1<<0),
} syirDevice_Leds_t;

#define syirDevice_ON_led(led)      PORTF.DIR |=  led; PORTF.OUT |=  led
#define syirDevice_OFF_led(led)     PORTF.DIR &= ~led; PORTF.OUT &= ~led
#define syirDevice_TOGGLE_led(led)  PORTF.DIR |=  led; PORTF.OUT ^=  led

#endif