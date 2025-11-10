/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.inter = RXData; IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 SPI - 3-wire Master Incremented Data
 *
 * This example shows how SPI master talks to SPI slave using 3-wire mode.
 * Incrementing data is sent by the master starting at 0x01. Received data is
 * expected to be same as the previous transmission.  eUSCI RX ISR is used to
 * handle communication with the CPU, normally in LPM0. Because all execution
 * after LPM0 is in ISRs, initialization waits for DCO to stabilize against
 * ACLK.
 *
 * Note that in this example, EUSCIB0 is used for the SPI port. If the user
 * wants to use EUSCIA for SPI operation, they are able to with the same APIs
 * with the EUSCI_AX parameters.
 *
 * ACLK = ~32.768kHz, MCLK = SMCLK = DCO 3MHz
 *
 * Use with SPI Slave Data Echo code example.
 *
 *                MSP432P401
 *              -----------------
 *             |                 |
 *             |                 |
 *             |                 |
 *             |             P1.6|-> Data Out (UCB0SIMO)
 *             |                 |
 *             |             P1.7|<- Data In (UCB0SOMI)
 *             |                 |
 *             |             P1.5|-> Serial Clock Out (UCB0CLK)
 *******************************************************************************/
/* DriverLib Includes */
#include <driverlib.h>

/* Standard Includes */
#include <at86rf215.h>
#include <msp.h>
#include <stdio.h>
#include "spi_helper.h"
#include <regs.h>
#include <at86rf215Regs.h>


/* Statics */
static volatile uint8_t transmitData = 0x01, receiveData = 0x00;

eUSCI_SPI_MasterConfig spiMasterConfig = {
    EUSCI_B_SPI_CLOCKSOURCE_SMCLK,      // Use SMCLK
    3000000,                            // SMCLK = 3 MHz
    500000,                             // SPI clock = 500 kHz
    EUSCI_B_SPI_MSB_FIRST,              // MSB first
    EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // Phase
    EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH,                 // CPOL = 0
    EUSCI_B_SPI_3PIN
};


void AT86RF215Reset( void );
void ClockInit( void );
void GpioSetInterrupt( uint_fast8_t port, uint_fast16_t pin, uint_fast8_t irq_mode);
void FPGA_modulation(void);
void FPGAreset(void);


static void at86_fsk_900_tx_demo(void);
typedef enum
{
    CLK_MCLK = 0,
    CLK_ACLK,
    CLK_HSMCLK,
    CLK_SMCLK,
    CLK_BCLK
} Clock_Types_t;


void gpio_init() {
 GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0); //CS
 GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7); //RST -spi
 GPIO_setAsInputPin(GPIO_PORT_P2, GPIO_PIN3); //irq
 GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7); // RESETN --FPGA
 MAP_GPIO_setAsInputPinWithPullDownResistor(GPIO_PORT_P2, GPIO_PIN3);

}

int main(void)
 {
    /* Halting WDT  */
    WDTCTL = WDTPW | WDTHOLD;
    ClockInit();
    //

    volatile uint32_t i;
    gpio_init();
    AT86RF215Reset();
    GpioSetInterrupt(GPIO_PORT_P2, GPIO_PIN3, GPIO_LOW_TO_HIGH_TRANSITION);

    //![Simple SPI Example]
    /* Selecting P1.5 P1.6 and P1.7 in SPI mode */
     GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring SPI in 3wire master mode */
    SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig);

    /* Enable SPI module */
    SPI_enableModule(EUSCI_B0_BASE);

    /* Enabling interrupts */
    SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_SPI_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIB0);
    //EUSCI_B_SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);


    /* Polling to see if the TX buffer is ready */
    while (!(SPI_getInterruptStatus(EUSCI_B0_BASE,EUSCI_SPI_TRANSMIT_INTERRUPT)));

     uint8_t version = AT86RF215Read(REG_RF_VN );

     printf("version = %x\n"
             "",version);
                  //  fflush(stdout);
     while (version != 0x03)
     {
         GPIO_setAsOutputPin(
         GPIO_PORT_P2,
         GPIO_PIN2
         );
         delay_us(1000);
         version = AT86RF215Read(REG_RF_VN);
     }
      modem_state = AT86RF215_RF09; // for 09 command // modem is set here


      /* Below function is for testing using the internal modulation capability of AT86*/
      //  AT86RF215_TX_Alt01_Test();
      // AT86RF215SetCLKO(0x2); // 0x2 for 32 mhz
      //      uint8_t  frequency = AT86RF215Read(REG_RF_CLKO);
      //    printf("set frequency = %x\n", frequency );


      //resetting fpga

      GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
             /* Wait typical time of timer TR1. */
             delay_us(100);
             /* Set RESET pin to 0 */
      GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7); // fpga in reset mode --- so sending zeroes now



      AT86RF215TxSetIQ(910000000); //-------------------------> function to set for lvds data reception
     // AT86RF215TxSetIQ_old(920000000);
      delay_ms(1);
      //FPGAreset();
      AT86RF215Write(REG_RF09_CMD, RF_CMD_TX);
      /* FPGA firmware load */
        delay_us(10);

        GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7); // release the reset on the fpga now so that it sends clear data
       //FPGAreset();

        while (1) {
                     uint8_t val = AT86RF215Read(REG_RF_IQIFC1);
                                uint8_t bit7 = (val >> 7) & 0x01;

                                uint8_t flsync = AT86RF215Read(REG_RF_IQIFC2);
                                uint8_t sync = (flsync >> 7) & 0x01;   // <-- fixed

                               // IQIFC0.SF-> indicates synchronization failure (1= sync failed)
                                uint8_t meh =  AT86RF215Read(REG_RF_IQIFC0);
                                uint8_t no_sync = (meh >>6) & 0x01;


                                uint8_t irq = AT86RF215Read(REG_RF09_IRQS);
                                uint8_t irqfsf = (irq>>5 ) & 0x01;

                                uint8_t trxrdy = (irq>>1) & 0x01;

                                uint8_t txerr = (irq>>4) & 0x01;
                                printf("failsafe value = %d  and sync = %d and irq_fail_safe_interrupt = %d  and trxrdy = %d  and txerr = %d and failed_sync = %d \n", bit7, sync, irqfsf, trxrdy, txerr,no_sync);

                                delay_ms(50);   // slow down for UART
                 }


}


void ClockInit( void )
{

    MAP_CS_initClockSignal(CS_MCLK, CS_MODOSC_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_2);
    MAP_CS_initClockSignal(CS_BCLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    /*
     *  Getting all of the frequency values of the CLK sources using the
     * convenience functions */
}


void GpioSetInterrupt( uint_fast8_t port, uint_fast16_t pin, uint_fast8_t irq_mode) {
    if (irq_mode == GPIO_LOW_TO_HIGH_TRANSITION )
    {
        MAP_GPIO_setAsInputPinWithPullDownResistor(port, pin);
        MAP_GPIO_interruptEdgeSelect(port, pin, GPIO_LOW_TO_HIGH_TRANSITION);
    }
    else
    {
        MAP_GPIO_setAsInputPinWithPullUpResistor(port, pin);
        MAP_GPIO_interruptEdgeSelect(port, pin, GPIO_HIGH_TO_LOW_TRANSITION);
    }

    MAP_GPIO_clearInterruptFlag(port, pin);
    MAP_GPIO_enableInterrupt(port, pin);
    MAP_Interrupt_enableInterrupt(INT_PORT1 + (port-GPIO_PORT_P1));
}


void AT86RF215Reset( void )
{
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN7);
    /* Ensure control lines have correct levels */
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
    /* Wait typical time of timer TR1. */
    delay_us(300);
    /* Set RESET pin to 0 */
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7);
    /* Wait 10 us */
    delay_us(300);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7);
}


void FPGA_modulation(void){

    // modem state is set in the main function itself.

    //AT86RF215TxSetIQ(900000000);
    delay_ms(1);

    // doing the below in the main function
   // AT86RF215SetState(RF_CMD_TX);
}

void FPGAreset(void){
    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
       /* Wait typical time of timer TR1. */
       delay_us(100);
       /* Set RESET pin to 0 */
       GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
       /* Wait 10 us */
       delay_us(100);
       GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
      // delay_ms(100);

//    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);
//    delay_us(10000000);
//    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN7);
//    delay_us(10000000);
//    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN7);


}





