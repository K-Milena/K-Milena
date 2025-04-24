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

#ifndef SYIRDEVICE_LOG_H
#define SYIRDEVICE_LOG_H

    extern void     syirDevice_INIT_log (void);
    extern int8_t   syirDevice_SET_log(int8_t BSCALE, uint16_t  BSEL, uint8_t CLK2X);
    extern int8_t   syirDevice_SEND_log (char *buf, uint16_t num);

#endif