/* ************************************************************************* */
/*                                                                           */
/* SYIR - Systemy Internetu Rzeczy LAB                                       */
/*                                                                           */
/* Autorzy:  Arkadiusz Luczyk; Jakub Jasinski; Marek NiewiÅ„ski               */
/* Email  :  arkadiusz.luczyk@pw.edu.pl                                      */
/*           jakub.jasinski@pw.edu.pl                                        */
/*           marek.niewinski@pw.edu.pl                                       */
/*                                                                           */
/*     Do wyÅ‚Ä…cznego uÅ¼ytku w ramach zajÄ™Ä‡ SYIR                              */
/*     Do wylacznego uzytku w ramach zajec SYIR                              */
/*                                                                           */
/* ************************************************************************* */

#include <syirDevice.h>  // Nag?ówek zawieraj?cy deklaracje funkcji dla urz?dzenia SYIR
#include <string.h>      // Nag?ówek dla operacji na ?a?cuchach znaków
#include <stdlib.h>      // Nag?ówek dla funkcji ogólnych, np. alokacji pami?ci
#include <stdio.h>       // Nag?ówek dla funkcji I/O, np. do logowania

// Ustawienia zegara urz?dzenia SYIR: cz?stotliwo?? 32 MHz
#define SYIRDEVICE_CLOCK_SOURCE     SYIRDEVICE_CLOCK_INTERNAL_32MHz
#define SYIRDEVICE_CLOCK_DIVISION   SYIRDEVICE_CLOCK_DIV_1

// Ustawienia zegara systemowego: ~26 Hz
#define SYSTEM_CLOCK_PERIOD         1200
#define SYSTEM_CLOCK_DIVISION       SYSTEM_CLOCK_DIV_1024

// Ustawienia logowania przez UART: pr?dko?? 57600 baud
#define UARTLOG_BSCALE              -6
#define UARTLOG_BSEL                2158
#define UARTLOG_CLOCK_DIVISION      0

int main(void) {
    
    char        log_buf [1024];  // Bufor do przechowywania logów
    uint16_t    log_len = 0;     // D?ugo?? aktualnie zapisanego logu

    char rec_buf[REC_BUF_SIZE];  // Bufor do przechowywania odebranych danych - cykliczny
    uint8_t rx_len;              // D?ugo?? odebranych danych
    
    systemClock_t led_ticks;     // Zmienna do zliczania cykli zegara dla LED
    systemClock_t log_ticks;     // Zmienna do zliczania cykli zegara dla logów
    
    syirDevice_OFF_ports();      // Wy??czanie portów urz?dzenia SYIR przed konfiguracj?

    // Ustawienie zegara urz?dzenia SYIR
    if (syirDevice_SET_deviceClock(SYIRDEVICE_CLOCK_SOURCE, SYIRDEVICE_CLOCK_DIVISION) < 0) 
    {
        syirDevice_ON_led(GREEN);  // W??czenie diody LED na zielono w przypadku b??du
        while (1);  // Zatrzymanie programu w niesko?czonej p?tli
    }
    
    // Ustawienie zegara systemowego
    if (syirDevice_SET_systemClock(SYSTEM_CLOCK_PERIOD, SYSTEM_CLOCK_DIVISION) < 0) 
    {
        syirDevice_ON_led(ORANGE);  // W??czenie diody LED na pomara?czowo w przypadku b??du
        while (1);  // Zatrzymanie programu w niesko?czonej p?tli
    }

    /* Ustawienie logowania przez UART z pr?dko?ci? 57600 baud */
    if (!syirDevice_SET_log(UARTLOG_BSCALE, UARTLOG_BSEL, UARTLOG_CLOCK_DIVISION)) 
    {
        syirDevice_ON_led(GREEN);  // W??czenie zielonej diody LED w przypadku b??du
        syirDevice_ON_led(ORANGE); // W??czenie pomara?czowej diody LED w przypadku b??du
        while (1);  // Zatrzymanie programu w niesko?czonej p?tli
    }

    syirDevice_INIT_log();  // Inicjalizacja logowania przez UART
    syirDevice_INIT_rec();  // Inicjalizacja odbioru danych przez USARTC0

    syirDevice_ON_led(GREEN);  // W??czenie zielonej diody LED, aby wskaza?, ?e system dzia?a
    syirDevice_OFF_led(ORANGE); // Wy??czenie pomara?czowej diody LED

    // Przesy?anie komunikatu powitalnego przez UART
    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "--------------------------------------------------------- \r\n");
    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "SYIR Device: ECHO mode\r\n");
    log_len += snprintf(log_buf+log_len, sizeof(log_buf)-log_len,   "Waiting for the UART string to echo > \r\n");
    while (syirDevice_SEND_log(log_buf, log_len) == 0);  // Wysy?anie logu przez UART
    log_len = 0;

    while (1){
        
        rx_len = syirDevice_GET_rec((uint8_t*)rec_buf, sizeof(rec_buf));  // Odbiór danych przez USART do bufora- ile znaków przysz?o z UART i zosta?o w?a?nie odebrane w tej iteracji p?tli.
        if (rx_len > 0) 
        {
            // Je?li s? dane w buforze, przesy?amy je z powrotem przez UART
            for (uint8_t i = 0; i < rx_len; ++i) {
                log_len += snprintf(log_buf + log_len, sizeof(log_buf) - log_len, "%c", rec_buf[i]);
            }
            while (syirDevice_SEND_log(log_buf, log_len) == 0);  // Wysy?anie danych przez UART
            log_len = 0;  // Zerowanie d?ugo?ci logu po wys?aniu
        }
        
        // Co 10 cykli zegara systemowego, prze??czamy LED
        if (syirDevice_CHECK_N_systemClock(10, &led_ticks)) {
            syirDevice_TOGGLE_led(GREEN);  // Zmiana stanu diody LED (zielona)
            syirDevice_TOGGLE_led(ORANGE); // Zmiana stanu diody LED (pomara?czowa)
        }

    }

    /* Nigdy nie powinien znale?? si? w tym miejscu */
    syirDevice_OFF_led(GREEN);  // Wy??czenie zielonej diody LED w przypadku krytycznego b??du
    syirDevice_OFF_led(ORANGE); // Wy??czenie pomara?czowej diody LED w przypadku krytycznego b??du
    while (1);  // Zatrzymanie programu w niesko?czonej p?tli
    return -1;  // Zwrócenie -1 w przypadku nieoczekiwanego zako?czenia programu
}
