/**
  System Interrupts Generated Driver File 

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.c

  @Summary:
    This is the generated manager file for the MPLAB(c) Code Configurator device.  This manager
    configures the pins direction, initial state, analog setting.
    The peripheral pin select, PPS, configuration is also handled by this manager.

  @Description:
    This source file provides implementations for MPLAB(c) Code Configurator interrupts.
    Generation Information : 
        Product Revision  :  MPLAB(c) Code Configurator - 4.15.1
        Device            :  dsPIC33EP128GP502
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.30
        MPLAB             :  MPLAB X 3.45

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

    Microchip licenses to you the right to use, modify, copy and distribute
    Software only when embedded on a Microchip microcontroller or digital signal
    controller that is integrated into your product or third party product
    (pursuant to the sublicense terms in the accompanying license agreement).

    You should refer to the license agreement accompanying this Software for
    additional information regarding your rights and obligations.

    SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
    EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
    MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
    IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
    CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
    OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
    INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
    CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
    SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
    (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

*/


/**
    Section: Includes
*/
#include <xc.h>
#include "pin_manager.h"

/**
    void PIN_MANAGER_Initialize(void)
*/
void PIN_MANAGER_Initialize(void)
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0010;
    LATB = 0x0000;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x000F;
    TRISB = 0xFF2F;

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPDA = 0x0000;
    CNPDB = 0x0000;
    CNPUA = 0x0000;
    CNPUB = 0x0000;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0000;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSELA = 0x0013;
    ANSELB = 0x0003;

    /****************************************************************************
     * Set the PPS
     ***************************************************************************/
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS

    RPINR22bits.SDI2R = 0x0025;   //RB5->SPI2:SDI2;
    RPOR2bits.RP38R = 0x0008;   //RB6->SPI2:SDO2;
    RPOR0bits.RP20R = 0x0001;   //RA4->UART1:U1TX;
    RPOR2bits.RP39R = 0x000A;   //RB7->SPI2:SS2OUT;
    RPOR1bits.RP36R = 0x0009;   //RB4->SPI2:SCK2OUT;

    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS

}

