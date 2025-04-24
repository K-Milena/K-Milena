/* ************************************************************************* */
/*                                                                           */
/* SYIR - Systemy Internetu Rzeczy LAB                                       */
/*                                                                           */
/* Autorzy:  Arkadiusz Luczyk; Jakub Jasinski; Marek NiewiÅski               */
/* Email  :  arkadiusz.luczyk@pw.edu.pl                                      */
/*           jakub.jasinski@pw.edu.pl                                        */
/*           marek.niewinski@pw.edu.pl                                       */
/*                                                                           */
/*     Do wyÅÄcznego uÅ¼ytku w ramach zajÄÄ SYIR                              */
/*     Do wylacznego uzytku w ramach zajec SYIR                              */
/*                                                                           */
/* ************************************************************************* */

#ifndef SYIRDEVICE_REC_H
#define SYIRDEVICE_REC_H

#define REC_BUF_SIZE 64  // rozmiar bufora cyklicznego

    extern void     syirDevice_INIT_rec (void);
    uint8_t syirDevice_GET_rec (uint8_t* buf, uint8_t N);


#endif