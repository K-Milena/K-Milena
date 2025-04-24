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

#include <avr/interrupt.h>  // Za??czenie nag?ówka dla przerwa?
#include "syirDevice.h"

#define REC_BUF_MASK (REC_BUF_SIZE - 1) // Maskowanie bufora; zak?ada, ze REC_BUF_SIZE jest pot?g? dwójki. U?ywamy tego do optymalizacji operacji w buforze

// Zmienna bufora cyklicznego do przechowywania odebranych danych
static volatile char    rec_buffer[REC_BUF_SIZE]; 
// Wska?niki do ?ledzenia miejsca zapisu (rec_head) i odczytu (rec_tail) w buforze
static volatile uint8_t rec_head = 0; // wskazuje miejsce do zapisania kolejnego odebranego bajtu
static volatile uint8_t rec_tail = 0;

// Struktura do przechowywania statusu urz?dzenia
typedef struct syirDevice_StatusRec {
    volatile uint8_t    state;   // Zmienna stanu urz?dzenia (np. status przerwania)
    char                buf;     // Zmienna pomocnicza dla bufora
} syirDevice_StatusRec_t;

syirDevice_StatusRec_t statusRec;  // Tworzenie structa typu syirDevice_StatusRec_t

// Funkcja inicjalizuj?ca odbiór danych przez USARTC0
void syirDevice_INIT_rec(void)
{
    rec_head = 0;  // Ustawienie wska?nika na pocz?tek bufora
    rec_tail = 0;  // Ustawienie wska?nika na pocz?tek bufora

    cli();  // Wy??czenie przerwa?, aby zainicjowa? sprz?t bez zak?óce?
    {
        PORTC.DIRCLR = (1 << 2);  // Ustawienie pinu 2 portu C jako wej?cie (dla RX)
        PORTC.OUTCLR = (1 << 2);  // Wy??czenie wyj?cia na tym pinie (je?li jest u?ywane)

        /* Wlaczenie portu C bitu 2 do odbioru  */
        PORTC.DIRSET &= 0b11111011;
        PORTC.DIR    &= 0b11111011;
        PORTC.OUT    &= 0b11111011;
        
        /* USARTC0 - interfejs USART kontrolujacy wyjscia/wejscia portu C*/
        USARTC0.CTRLB |= (1 << 4);           // Ustawienie USARTU do odbioru - RX
        USARTC0.CTRLC  = (0b11 << 0);        // Konfiguracja formatu ramki: 8 bitów, brak parzysto?ci, 1 bit stopu (8N1)
        USARTC0.CTRLA |= (0b10 << 4);        // Ustawienie przerwania dla odbioru z poziomem priorytetu MEDIUM
    }
    sei();  // W??czenie przerwa?, kontynuacja pracy programu
}

// Funkcja do odczytu danych z bufora cyklicznego
uint8_t syirDevice_GET_rec(uint8_t* buf, uint8_t N)
{
    if (!buf || N == 0) return 0;  // Je?li wska?nik jest NULL lub liczba pró?b o dane to 0, zwró? 0

    uint8_t count = 0;

    cli();  // Wy??czenie przerwa? przed dost?pem do zmiennych wspó?dzielonych
    // Odczyt danych z bufora do podanego bufora docelowego
    while ((rec_head != rec_tail) && (count < N)) {  
        buf[count++] = rec_buffer[rec_tail];  // Odczytanie danych
        rec_tail = (rec_tail + 1) & REC_BUF_MASK;  // Inkrementacja wska?nika 'tail' z uwzgl?dnieniem bufora cyklicznego
    }
    sei();  // Ponowne w??czenie przerwa?

    return count;  // Zwrócenie liczby odczytanych bajtów
}

/* Procedura obs?ugi przerwania - odbiór danych przez USARTC0 */
ISR(USARTC0_RXC_vect)
{
    uint8_t next_head = (rec_head + 1) & REC_BUF_MASK;  // Obliczenie kolejnego wska?nika "head" (przy uwzgl?dnieniu bufora cyklicznego)

    // Je?li bufor nie jest pe?ny, zapisujemy nowy znak
    if (next_head != rec_tail) {
        rec_buffer[rec_head] = USARTC0.DATA;  // Zapisanie odebranego znaku do bufora
        rec_head = next_head;  // Aktualizacja wska?nika "head"
    } else {
        volatile char dummy = USARTC0.DATA;  // Odczytujemy dane w przypadku przepe?nienia bufora (mo?na je tak?e logowa?)
    }
}
