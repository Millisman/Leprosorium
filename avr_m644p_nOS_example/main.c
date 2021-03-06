/*
 * nOS v0.1
 * Copyright (c) 2014 Jim Tremblay
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "nOS.h"

#define THREAD_STACK_SIZE       128

#include "pin_macros.h"

#define PinR A,0,H
#define PinG A,1,H
#define PinB A,2,H


static void Timer2Init(void);

void ThreadA(void *arg);
void ThreadB(void *arg);
void ThreadC(void *arg);

nOS_Sem semA;
nOS_Sem semB;
nOS_Sem semC;
nOS_Thread threadA;
nOS_Thread threadB;
nOS_Thread threadC;
nOS_Stack threadAStack[THREAD_STACK_SIZE];
nOS_Stack threadBStack[THREAD_STACK_SIZE];
nOS_Stack threadCStack[THREAD_STACK_SIZE];

void ThreadA(void *arg)
{
    volatile uint32_t cntr = 0;

    (void)arg;

    while(1)
    {
        ON(PinG);
//         if (tick == 255) ON(PinG);
//         if (tick == 128) OFF(PinR);
        nOS_SemTake(&semA, NOS_WAIT_INFINITE);

        nOS_Sleep(200);
        OFF(PinG);
        nOS_Sleep(200);

        cntr++;

    }
}

void ThreadB(void *arg)
{
    volatile uint32_t cntr = 0;
    cntr |= 0x80;

    (void)arg;

    while(1)
    {

        nOS_SemTake(&semB, NOS_WAIT_INFINITE);
        nOS_SemGive(&semA);
        cntr++;
        if (cntr % 5 == 0) ON(PinB);
        if (cntr % 7 == 0) OFF(PinB);
//         OFF(PinB);
    }
}

void ThreadC(void *arg)
{
    volatile uint32_t cntr = 0;
// // //     volatile uint32_t cntr = 0;
    (void)arg;

    while(1)
    {
        nOS_SemTake(&semC, NOS_WAIT_INFINITE);
        nOS_SemGive(&semB);
        cntr++;
    }
}


static uint8_t tick = 0;

NOS_ISR(TIMER2_COMPA_vect)
{

    nOS_Tick(tick);
    ++tick;
}

static void Timer2Init(void)
{
    OCR2A  = 249;           /* Set compare match value        */
    TCCR2A = 0b00000010;    /* Clear timer on compare match   */
    TCCR2B = 0b00000100;    /* Use prescaler of 64            */
    TIMSK2 = (1 << OCIE2A); /* Enable compare match interrupt */

    /* enable all interrupts */
    sei();
}



int main (void)
{
    DRIVER(PinR, OUT);	//as output
    DRIVER(PinG, OUT);		//as input
    DRIVER(PinB, OUT);	// with pullup


    volatile uint32_t cntr = 0;

    nOS_Init();
	
	nOS_ThreadSetName(NULL, "main");

    nOS_SemCreate(&semA, 0, 1);
    nOS_SemCreate(&semB, 0, 1);
    nOS_SemCreate(&semC, 0, 1);

    nOS_ThreadCreate(&threadA, ThreadA, (void*)300, threadAStack, THREAD_STACK_SIZE, NOS_CONFIG_HIGHEST_THREAD_PRIO,   NOS_THREAD_READY, "ThreadA");
    nOS_ThreadCreate(&threadB, ThreadB, (void*)200, threadBStack, THREAD_STACK_SIZE, NOS_CONFIG_HIGHEST_THREAD_PRIO-1, NOS_THREAD_READY, "ThreadB");
    nOS_ThreadCreate(&threadC, ThreadC, (void*)100, threadCStack, THREAD_STACK_SIZE, NOS_CONFIG_HIGHEST_THREAD_PRIO-2, NOS_THREAD_READY, "ThreadC");

    Timer2Init();
    nOS_Start();

    // nOS_Yield();

    while (1)
    {
        if (tick == 255) ON(PinR);
        if (tick == 128) OFF(PinR);
        nOS_SemGive(&semC);
        cntr++;
    }
}
