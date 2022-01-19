/*
 * Copyright (c) 2021 EDDR  konstantin.mauch@ All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in the
 *          documentation and/or other materials provided with the distribution.
 *        * Neither the name of The Linux Foundation nor
 *          the names of its contributors may be used to endorse or promote
 *          products derived from this software without specific prior written
 *          permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.    IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Abstract: an implementation of hardware abstraction layer stuff.
 */

/* HW_ALL_PORTS ,et al */
#include "knop.h"

/* NULL */
#include <linux/kernel.h>

/* Required for the GPIO functions */
#include <linux/gpio.h> 

/* <_1_SCLK_HI_W> et al */
#include "hal.h"

/* Length of data word at AD53xx converter. TODO: move away */
#define AD5300_DATA_LEN		16

/* Amount of bits (within data word) to be ignored while passing 8-bit data value to AD53xx converter. TODO: move away */
#define AD5300_DONTCARE_LEN		4

/* Longest command line ever required by this app. */
#define MEDIUM_SIZE			1024

/* GPIOs connected to SCKL with 'white' wire */
GPIOTarget SCLK_i_W[HW_ALL_PORTS];
/* GPIOs connected to MOSI with 'white' wire */
GPIOTarget MOSI_i_W[HW_ALL_PORTS];
/* GPIOs connected to SYNC with 'white' wire */
GPIOTarget SYNC_i_W[HW_ALL_PORTS];
/* GPIOs connected to SCKL with 'green' wire */
GPIOTarget SCLK_i_G[HW_ALL_PORTS];
/* GPIOs connected to MOSI with 'green' wire */
GPIOTarget MOSI_i_G[HW_ALL_PORTS];
/* GPIOs connected to SYNC with 'green' wire */
GPIOTarget SYNC_i_G[HW_ALL_PORTS];

/* Command line buffer */
char pcCmdBuffer[MEDIUM_SIZE];

/*!
 * @brief		Well I burned this Port-30 on P9 slopt cause I wanted to re-solder
 			the ~SYN pin of AD5300BRMZ while the was power on ande working device,
 			now it seems like I touched GND and ~SYN on the working device while
 			re-soldering. So I've substituted it in the hardware and here. Also
 			I'm about toface a lack of working GPIO ports cause one by one the
 			go away, no return. But the problem will be gone as soon as I take
 			a new template board, so I don't care much about this lacking-ports
 			problem. 
 */
char * GPIOs[] = {

	 /* P9, left side: 30-->3  */
	 "3", "31", "48" , "60", "50", "51", "49", "20", 

	 /* P9, right side */
	 "115", "15", "14" , "112", "5", "5", "5",  "5",

	 /* P8, left side */ 
	 "5", "5", "5", "5", "5", "5", "5",

	 /* P8, right side */ 
	 "5", "5", "5", "5", "5", "5", "5" 

}; /* char * GPIOs[] */

/*!
 * @brief		Make GPIO port iPort a) to appear in the system; b) to become an output port
 *			
 *
 * \param[in] 		.
 *
 * return		.
 */
void OpenGPIO(int iPort)
{
	/* gpioLED is 60 by default, request it */
	gpio_request(iPort, "sysfs");

	/* Set the GPIO to operate in output mode */
	gpio_direction_output(iPort, /* ledOn */ true );
}

/*!
 * @brief		Park the port lines down
 *			
 *
 * \param[in] 		.
 *
 * return		.
 */
void CloseGPIO(int iPort)
{
	/* Turn the LED off, indicates device was unloaded */
	gpio_set_value(iPort, 0);

	/* Unexport GPIO */
	gpio_unexport(iPort);

	/* Free the LED GPIO */
	gpio_free(iPort);
}

/*!
 * @brief		
 *
 * \param[in] 		.
 *
 * return		.
 */
void OnGPIO(/* FILE * fcPortFile */ GPIOTarget iPort)
{
		gpio_set_value(iPort, 1);
}

/*!
 * @brief		
 *
 * \param[in] 		.
 *
 * return		.
 */
void OffGPIO(/* FILE * fcPortFile */ GPIOTarget iPort)
{
		gpio_set_value(iPort, 0);
}

/*!
 * @brief		
 *
 * \param[in] 		.
 * \param[in] 		.
 *
 * return		None
 */
void _i_AD5300_Write_W(unsigned char data, int iIdx) 
{
unsigned short tmp;

unsigned char iCnt;

	tmp = data << AD5300_DONTCARE_LEN;

	_i_AD5300_ACT_W( SYNC_i_W [iIdx] );

	for (iCnt = 0; iCnt < AD5300_DATA_LEN; iCnt++)
	{
		_i_SCLK_HI_W( SCLK_i_W[iIdx] );

		(tmp & (unsigned short)( 1U << (15 - iCnt) ) ) ? (_i_MOSI_HI_W( MOSI_i_W[iIdx] )) : (_i_MOSI_LO_W( MOSI_i_W[iIdx] ));

		_i_SCLK_LO_W( SCLK_i_W[iIdx] );
	}

	_i_AD5300_DEACT_W ( SYNC_i_W [iIdx] );

} /* void _i_AD5300_Write_W(unsigned short data, int iIdx) */

/*!
 * @brief		
 *
 * \param[in] 		.
 * \param[in] 		.
 *
 * return		None
 */
void _i_AD5300_Write_G(unsigned char data, int iIdx) 
{
unsigned short tmp;

unsigned char iCnt;

	tmp = data << AD5300_DONTCARE_LEN;

	_i_AD5300_ACT_G ( SYNC_i_G [iIdx] );

	for (iCnt = 0; iCnt < AD5300_DATA_LEN; iCnt++)
	{
		_i_SCLK_HI_G( SCLK_i_G[iIdx] );

		(tmp & (unsigned short)( 1U << (15 - iCnt) ) ) ? (_i_MOSI_HI_G( MOSI_i_G[iIdx] )) : (_i_MOSI_LO_G( MOSI_i_G[iIdx] ));

		_i_SCLK_LO_G( SCLK_i_G[iIdx] );
	}

	_i_AD5300_DEACT_G ( SYNC_i_G [iIdx] );

} /* void _i_AD5300_Write_G(unsigned short data, int iIdx) */
