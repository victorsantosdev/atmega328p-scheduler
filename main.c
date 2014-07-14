/*
 * main.cpp
 *
 *  Created on: Apr 6, 2014
 *      Author: victor
 */
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include "ulthread.h"
#include "util/delay.h"
#include "uart.h"
#include "avr/interrupt.h"
#include "LS_defines.h"
/* define CPU frequency in Mhz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* 9600 baud */
#define UART_BAUD_RATE 9600
#define UNUSED(x) (void)(x)

static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

/*************************************************************************
Function: uartInit
Purpose: initialize uart in 9600 8-N-1 standard and sets serial to stdout
 **************************************************************************/
void uartInit(void) { /* testado: OK */
    /*
     * Initialize UART library, pass baudrate and AVR cpu clock
     * with the macro
     * UART_BAUD_SELECT() (normal speed mode )
     * or
     * UART_BAUD_SELECT_DOUBLE_SPEED() ( double speed mode)
     */
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );

    // setup our stdio stream
    stdout = &mystdout;
}

void thr_run(void * args) {
    volatile int i = 10;
    int * a = (int*)args;
    //se quiser usar o valor de a tem que deferenciar ex. *a
    while (i--) {
        printf("Thread %d: running...\n", getid());
        printf("Thread arg= %d\n", *a);
        printf("Iteration number= %d\n", i);
        PORTB = 127 & *a;
        _delay_ms(250);
        cpl_bit(PORTB, PB4);
        _delay_ms(250);
        /*free cpu to run another thread with yield method()*/
        yield();
    }
    finish(*a);
}

int main(void) {
    uartInit();
    sei();
    printf("AVR Scheduler\n");
#if 0
    printf("PortB test.\n");
    DDRB = 0b11111111;
    PORTB = 5;
    _delay_ms(500);
#endif 
    PORTB = 0;
    printf("Initializing Scheduler...\n");
    init_scheduler();
    printf("Done\n");
    int res_a, res_b, res_c;
    int arg_a = 5;
    int arg_b = 6;
    int arg_c = 7;
    int tid_a = create(&thr_run, THREAD_PRIORITY_NORMAL, MINIMAL_STACK_SIZE, &arg_a);
    printf("Thread A created\n");
    int tid_b = create(&thr_run, THREAD_PRIORITY_NORMAL, MINIMAL_STACK_SIZE, &arg_b);
    printf("Thread B created\n");
    int tid_c = create(&thr_run, THREAD_PRIORITY_ABOVE_NORMAL, MINIMAL_STACK_SIZE, &arg_c);
    printf("Thread C created\n");
    //UNUSED(tid_a);
    //UNUSED(tid_b);
    //UNUSED(tid_c);
    join(tid_a, &res_a);
    join(tid_b, &res_b);
    join(tid_c, &res_c);

    while(1) {
        printf("While(1):Main\n");
        _delay_ms(250);
        PORTB = 0;
    }
}



