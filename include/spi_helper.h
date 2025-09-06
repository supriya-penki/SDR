//
// Created by supriya on 8/10/25.
//

#ifndef SPI_HELPER_H
#define SPI_HELPER_H

#endif //SPI_HELPER_H
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
#ifndef _SYSTEM_SPI_H_
#define _SYSTEM_SPI_H_
#include <stdio.h>
#include <stdint.h>
#include <msp.h>
#include <driverlib.h>

/**#############################Variables and Types#############################**/


extern volatile unsigned char SPI_RXData_IQ;
extern volatile unsigned char SPI_RXData_LoRa;
extern volatile unsigned char SPI_RXData_FPGA;
extern volatile unsigned char SPI_RXData_Flash;

/* SPI object type definition */

/**#############################Clock#############################**/
#define CLK_FREQ_8M     1
#define CLK_FREQ_16M    2
#define CLK_FREQ_24M    3

#define CLK_FREQ        CLK_FREQ_8M

#if CLK_FREQ == CLK_FREQ_8M
#define CYCLES_mS      8000
#elif CLK_FREQ == CLK_FREQ_16M
#define CYCLES_mS      16000
#elif CLK_FREQ == CLK_FREQ_24M
#define CYCLES_mS      24000
#else
#define CYCLES_mS      8000
#endif

#define CYCLES_us   CYCLES_mS/1000
/**#############################Functions#############################**/
void SpiInit(void);

uint8_t SpiInOut_IQRadio(uint8_t outData);

uint8_t SpiInOut_LoRa( uint8_t outData);

uint8_t fpgaSpiInOut(uint8_t outData);

uint8_t flashSpiInOut(uint8_t outData);

void delay_ms(uint32_t msTime);
void delay_us(uint32_t usTime);

#endif
