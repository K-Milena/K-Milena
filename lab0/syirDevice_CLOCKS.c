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

typedef struct syirDevice_StatusSystemClock {

    volatile systemClock_t tick;

} syirDevice_StatusSystemClock_t;

syirDevice_StatusSystemClock_t statusCLOCKS;


/* Ustawienie zrodla i podzialu czestotliwosci zegara urzadzenia */
int8_t syirDevice_SET_deviceClock(syirDevice_DeviceClockSource_t ckSrc, syirDevice_DeviceClockDivision_t clkDivision) {

    uint16_t    timeout = 0;

    // Sprawdzanie warunkow bezpieczenstaw danych wejsciowych
    switch (ckSrc) {
        case SYIRDEVICE_CLOCK_INTERNAL_2MHz:
        case SYIRDEVICE_CLOCK_INTERNAL_32MHz:
        case SYIRDEVICE_CLOCK_INTERNAL_32kHz:
        case SYIRDEVICE_CLOCK_EXTERNAL_16MHz: 
            break;

        default: 
            return -1;
    }
    switch (clkDivision) {
        case SYIRDEVICE_CLOCK_DIV_1 :
        case SYIRDEVICE_CLOCK_DIV_2 :
        case SYIRDEVICE_CLOCK_DIV_4 :
        case SYIRDEVICE_CLOCK_DIV_8 :
        case SYIRDEVICE_CLOCK_DIV_16 :
        case SYIRDEVICE_CLOCK_DIV_32 :
        case SYIRDEVICE_CLOCK_DIV_64 :
        case SYIRDEVICE_CLOCK_DIV_128 :
        case SYIRDEVICE_CLOCK_DIV_256 :
        case SYIRDEVICE_CLOCK_DIV_512 : 
            break;

        default: 
            return -1;
    }

        timeout = 1;
        cli();
        {
            switch (ckSrc) {
                case SYIRDEVICE_CLOCK_EXTERNAL_16MHz: {
                    // Ustawienia parametrow zewnetrznego oscylatora - freq_range | sturtup_time
                    OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
                    break;
                }
                default:
                    break;
            }
            // Selekcja zadanego zegara    
            OSC.CTRL = OSC.CTRL | (1 << ckSrc);
            // Oczekiwanie az zadany zegar wstanie
            while (!(OSC.STATUS & (1 << ckSrc)))
            {
                timeout += 1;
                if (timeout == 0)
                    break;
            }
            if (timeout != 0) {
            // Faktyczna zmiana zegara i podzielnika zegara  
                CCP         = CCP_IOREG_gc;
                CLK.CTRL    = ckSrc;
                CCP         = CCP_IOREG_gc;
                CLK.PSCTRL  = clkDivision;
            }
            PMIC.CTRL       = PMIC_LOLVLEN_bm | PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm; // All INT levels active
        } 
        sei();
    
    if (timeout == 0) 
        return -1;
    else
        return 1;
}

int8_t syirDevice_SET_systemClock (uint16_t period, syirDevice_SystemClockDivision_t ckDiv) {  
    
    statusCLOCKS.tick = 0;

    if (period == 0)
        return -1;

    switch (ckDiv) {
        case SYSTEM_CLOCK_DIV_1: 
        case SYSTEM_CLOCK_DIV_2: 
        case SYSTEM_CLOCK_DIV_4: 
        case SYSTEM_CLOCK_DIV_8: 
        case SYSTEM_CLOCK_DIV_64: 
        case SYSTEM_CLOCK_DIV_256: 
        case SYSTEM_CLOCK_DIV_1024: break;
        default: 
            return -1;
    }

    cli();
    {
        TCC0.CTRLA      = 0;
        TCC0.CTRLB      = 0;
        TCC0.CTRLC      = 0;
        TCC0.CTRLD      = 0;
        TCC0.CTRLE      = 0;
        TCC0.CNT        = 0;
        TCC0.PERBUF     = period;
        TCC0.CTRLA      = ckDiv;
        TCC0.INTCTRLA   = 1; // system clock prioryty Low
    }
    sei();
    return 1;   
}

int8_t syirDevice_CHECK_systemClock(systemClock_t *prev){
    uint16_t tick = statusCLOCKS.tick;
    if (tick == *prev)
        return 0;
    
    *prev = tick;
    return 1;  
    
}

int8_t syirDevice_CHECK_N_systemClock(uint16_t N, systemClock_t *prev){
    uint16_t tick = statusCLOCKS.tick;
    uint16_t k = 0;
    uint16_t old = *prev;

    if (tick > old)
        k = tick - old;

    if (tick < old)
        k = (0xFFFF - old) + tick;

    if (k < N ) 
        return 0;
    
    *prev = tick;
    return 1;    
    
}

//SYSTEM CLOCK interrupt service routine
ISR(TCC0_OVF_vect)
{
   statusCLOCKS.tick--;
}