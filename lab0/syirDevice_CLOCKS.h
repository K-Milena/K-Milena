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

#ifndef SYIRDEVICE_CLOCKS_H
#define SYIRDEVICE_CLOCKS_H

/* Stale wyboru zrodla sygnalu zegara urzadzenia */
typedef enum {
    SYIRDEVICE_CLOCK_INTERNAL_32kHz  = CLK_SCLKSEL_RC32K_gc,
    SYIRDEVICE_CLOCK_INTERNAL_2MHz   = CLK_SCLKSEL_RC2M_gc,
    SYIRDEVICE_CLOCK_INTERNAL_32MHz  = CLK_SCLKSEL_RC32M_gc,
    SYIRDEVICE_CLOCK_EXTERNAL_16MHz  = CLK_SCLKSEL_XOSC_gc,
} syirDevice_DeviceClockSource_t; 

/* Stale podzialu sygnalu zegara urzadzenia */
typedef enum {
    SYIRDEVICE_CLOCK_DIV_1    = CLK_PSADIV_1_gc,
    SYIRDEVICE_CLOCK_DIV_2    = CLK_PSADIV_2_gc,
    SYIRDEVICE_CLOCK_DIV_4    = CLK_PSADIV_4_gc,
    SYIRDEVICE_CLOCK_DIV_8    = CLK_PSADIV_8_gc,
    SYIRDEVICE_CLOCK_DIV_16   = CLK_PSADIV_16_gc,
    SYIRDEVICE_CLOCK_DIV_32   = CLK_PSADIV_32_gc,
    SYIRDEVICE_CLOCK_DIV_64   = CLK_PSADIV_64_gc,
    SYIRDEVICE_CLOCK_DIV_128  = CLK_PSADIV_128_gc,
    SYIRDEVICE_CLOCK_DIV_256  = CLK_PSADIV_256_gc,
    SYIRDEVICE_CLOCK_DIV_512  = CLK_PSADIV_512_gc,
} syirDevice_DeviceClockDivision_t;

typedef enum {

    SYSTEM_CLOCK_DIV_1    = 1,
    SYSTEM_CLOCK_DIV_2    = 2,
    SYSTEM_CLOCK_DIV_4    = 3,
    SYSTEM_CLOCK_DIV_8    = 4,
    SYSTEM_CLOCK_DIV_64   = 5,
    SYSTEM_CLOCK_DIV_256  = 6,
    SYSTEM_CLOCK_DIV_1024 = 7,

} syirDevice_SystemClockDivision_t;


    typedef uint16_t systemClock_t;

    extern int8_t syirDevice_SET_deviceClock      (syirDevice_DeviceClockSource_t ckSrc, syirDevice_DeviceClockDivision_t clkDivision);
    extern int8_t syirDevice_SET_systemClock      (uint16_t period, syirDevice_SystemClockDivision_t ckDiv);
    extern int8_t syirDevice_CHECK_systemClock    (systemClock_t *prev);
    extern int8_t syirDevice_CHECK_N_systemClock  (uint16_t N, systemClock_t *prev);


#endif