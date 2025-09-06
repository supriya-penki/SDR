//
// Created by supriya on 8/10/25.
//
/*
 _____  _                _____ ______ ______
|_   _|(_)              /  ___||  _  \| ___ \
  | |   _  _ __   _   _ \ `--. | | | || |_/ /
  | |  | || '_ \ | | | | `--. \| | | ||    /
  | |  | || | | || |_| |/\__/ /| |/ / | |\ \
  \_/  |_||_| |_| \__, |\____/ |___/  \_| \_|
                   __/ |
                  |___/
Description: Implements the generic SPI driver

License: see LICENSE.TXT file include in the project

Maintainer: Mehrdad Hessar, Ali Najafi
*/
#include <spi_helper.h>
#include <msp.h>
#include <stdio.h>
#include <stdint.h>

volatile unsigned char SPI_RXData_IQ;
volatile unsigned char SPI_RXData_LoRa;
volatile unsigned char SPI_RXData_FPGA;
volatile unsigned char SPI_RXData_Flash;

/* Statics */
static volatile uint8_t RXData = 0;

/**#############################Functions#############################**/


uint8_t SpiInOut_IQRadio(uint8_t outData) {
    /* SPI send data */

	EUSCI_B_CMSIS(EUSCI_B0_BASE)->TXBUF = outData;

   __no_operation();

   __enable_irq();
   PCM_gotoLPM0();
    return SPI_RXData_IQ;
}





//******************************************************************************
//
//This is the EUSCI_B0 interrupt vector service routine.
//
//******************************************************************************
void EUSCIB0_IRQHandler(void)
{
    uint32_t status = EUSCI_B_CMSIS(EUSCI_B0_BASE)->IFG &
            ((EUSCI_SPI_TRANSMIT_INTERRUPT | EUSCI_SPI_RECEIVE_INTERRUPT)
                    & EUSCI_B_CMSIS(EUSCI_B0_BASE)->IE);

    EUSCI_B_CMSIS(EUSCI_B0_BASE)->IFG &= ~status;

    while(!(EUSCI_B_CMSIS(EUSCI_B0_BASE)->IFG & EUSCI_B_SPI_TRANSMIT_INTERRUPT));

    SPI_RXData_IQ = EUSCI_B_CMSIS(EUSCI_B0_BASE)->RXBUF;

    /* exit from LPM0 */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
}


void delay_ms(uint32_t msTime)
{
    //    unsigned long cycles;
    //    if (CLK_FREQ == CLK_FREQ_8M)
    //        cycles = 8000;
    //    else if (CLK_FREQ == CLK_FREQ_16M)
    //        cycles = 16000;
    //    else if (CLK_FREQ == CLK_FREQ_24M)
    //        cycles = 24000;
    //    else
    //        cycles = 8000;

    int i;
    for (i=0; i<msTime; i++)
        __delay_cycles(CYCLES_mS);
}

void delay_us(uint32_t usTime)
{
    int i;
    for (i=0; i<usTime; i++)
        __delay_cycles(CYCLES_us);
}



