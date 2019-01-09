/*
 * Copyright (c) 2019 kmauch@     All rights reserved
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
 * Abstract: a driver to test input and output of SITARA platform
 * 
 * Note: includes <linux/init.h>, <linux/kernel.h> not needed on 4.15.0-43-generic
 *
 */

#if (0)
/* stdout, NULL */
#include <stdio.h>

/* sprintf() */
#include <string.h>
#endif /* (0) */

/* <_1_SCLK_HI_W> et al */
#include "beagle.h"


/* Length of data word at AD53xx converter. TODO: move away */
#define AD5300_DATA_LEN		16

/* Amount of bits (within data word) to be ignored while passing 8-bit data value to AD53xx converter. TODO: move away */
#define AD5300_DONTCARE_LEN	4


/* Longest command line ever required by this app. */
#define MEDIUM_SIZE 1024

/* Command line buffer */
char pcCmdBuffer[MEDIUM_SIZE];


char * GPIOs[] = {
	 /* P9, left side */
	 /*"30"*/ "3", "31", "48" , "60", "50", "51", "5", "5", 

	 /* P9, right side */
	 "5", "5", "5" , "5", "5", "5", "5",  "5",

	 /* P8, left side */ 
	 "5", "5", "5", "5", "5", "5", "5",

	 /* P8, right side */ 
	 "5", "5", "5", "5", "5", "5", "5" 
}; /* char * GPIOs[] */

/* Array of pointers to GPIO files */
FILE * GPIO_VALUE_FILES[30];

void common_print(char *format, ...)
{
#if (0)
    va_list args;

    va_start(args, format);

    vprintf(format, args);

    va_end(args);
#else
#endif /* (0) */

}

/* Make GPIO port <pcPortStr>: a) to appear in the system; b) to become output port; */
static void OpenGPIO(char * pcPortStr)
{
#if (0)
	if  (NULL != pcPortStr)
	{
		/* Command prepare */
		sprintf (pcCmdBuffer, "echo %s > /sys/class/gpio/export", pcPortStr);

		/* Run command in <sh> shell */
		system (pcCmdBuffer);

		/* Wait for driver create filesystem components </sys/class/gpio/gpio%s/> */
		usleep(1000);

		/* Command prepare */
		sprintf (pcCmdBuffer, "echo out > /sys/class/gpio/gpio%s/direction", pcPortStr);

		/* Run command in <sh> shell */
		system (pcCmdBuffer);

		/* Wait for driver stabilize tartget GPIO port */
		usleep(1000);
	}
	else
		common_printf("[%s] [%s] empty Port ID \n",__FILE__, __func__ );
#else
#endif /* (0) */
}

/* Toggle GPIO port <pcPortStr> ON */
#if defined(SH_FOPS)
void OnGPIO(char * pcPortStr)
#else
void OnGPIO(FILE * fcPortFile)
#endif /* (0) */
{
#if defined(SH_FOPS)
	if  (NULL != pcPortStr)
#else
	if  (NULL != fcPortFile)
#endif /* (0) */
	{
#if defined(SH_FOPS)
		/* Command prepare */		
		sprintf (pcCmdBuffer, "echo 1 > /sys/class/gpio/gpio%s/value", pcPortStr);

		/* Run command in <sh> shell */
		system (pcCmdBuffer);
#else
		fwrite("1", 1,1, fcPortFile);
		fflush(fcPortFile);
#endif /* (0) */

		/* Time ballast */
//.		usleep(1);
	}
	else
		common_printf("[%s] [%s] empty Port ID \n",__FILE__, __func__ );
}

/* Toggle GPIO port <pcPortStr> OFF */
#if defined(SH_FOPS)
void OffGPIO(char * pcPortStr)
#else
void OffGPIO(FILE * fcPortFile)
#endif /* (0) */
{
#if defined(SH_FOPS)
	if  (NULL != pcPortStr)
#else
	if  (NULL != fcPortFile)
#endif /* (0) */
	{
#if defined(SH_FOPS)
		/* Command prepare */
		sprintf (pcCmdBuffer, "echo 0 > /sys/class/gpio/gpio%s/value", pcPortStr);

		/* Run command in <sh> shell */
		system (pcCmdBuffer);
#else
		fwrite("0", 1,1, fcPortFile);
		fflush(fcPortFile);
#endif /* (0) */

		/* Time ballast */
//.		usleep(2);
	}
	else
		common_printf("[%s] [%s] empty Port ID \n",__FILE__, __func__ );
}

void _i_AD5300_Write_W(unsigned char data, int iIdx) 
{
unsigned short tmp;

unsigned char iCnt;

	tmp = data << AD5300_DONTCARE_LEN;

	_i_AD5300_ACT_W ( SYNC_i_W [iIdx] );

	for (iCnt = 0; iCnt < AD5300_DATA_LEN; iCnt++)
	{
		_i_SCLK_HI_W( SCLK_i_W[iIdx] );

		(tmp & (unsigned short)( 1U << (15 - iCnt) ) ) ? (_i_MOSI_HI_W( MOSI_i_W[iIdx] )) : (_i_MOSI_LO_W( MOSI_i_W[iIdx] ));

		_i_SCLK_LO_W( SCLK_i_W[iIdx] );
	}

	_i_AD5300_DEACT_W ( SYNC_i_W [iIdx] );

} /* void _i_AD5300_Write_W(unsigned short data, int iIdx) */

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


void AD5300_Init() 
{
int iIdx, iPdx;

	memset( (void*) GPIO_VALUE_FILES, 0, sizeof (GPIO_VALUE_FILES) );

	/* Initialize GPIO ports */
	for (iIdx = 0; iIdx < sizeof(GPIOs)/sizeof(GPIOs[0]);iIdx++ )
	{
//.		common_printf("[%s] [%s] opening GPIO %s \n",__FILE__, __func__ , GPIOs[iIdx] );

		OpenGPIO( GPIOs[iIdx] );	
	}
	common_printf("[%s] [%s] opened all GPIO ports \n",__FILE__, __func__ );

#if !defined(SH_FOPS)

	/* Open GPIO value files and store file poniters in array <GPIO_VALUE_FILES> */
	for (iIdx = 0; iIdx < sizeof(GPIO_VALUE_FILES)/sizeof(GPIO_VALUE_FILES[0]);iIdx++ )
	{
//.		common_printf("[%s] [%s] opening GPIO value file for GPIO %s \n",__FILE__, __func__ , GPIOs[iIdx] );

		/* Filename prepare */
		sprintf (pcCmdBuffer, "/sys/class/gpio/gpio%s/value", GPIOs[iIdx]);

		/* Try to open value file <pcCmdBuffer> */
		if ( NULL == (GPIO_VALUE_FILES[iIdx] = fopen (pcCmdBuffer, "wb+") ) )
		{
			common_printf("[%s] [%s] can't open GPIO value file <%s> \n", __FILE__, __func__ , pcCmdBuffer);

			continue;
		}

	} /* for (iIdx = 0; ... */

	/* GPIOs connected to SCKL with 'white' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		SCLK_i_W[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx];

	/* GPIOs connected to MOSI with 'white' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		MOSI_i_W[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx + 1];

	/* GPIOs connected to SYNC with 'white' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		SYNC_i_W[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx + 2];

	/* GPIOs connected to SCKL with 'green' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		SCLK_i_G[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx + 3];

	/* GPIOs connected to MOSI with 'green' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		MOSI_i_G[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx + 4];

	/* GPIOs connected to SYNC with 'green' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		SYNC_i_G[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx + 5];

#endif /* !defined(SH_FOPS) */

} /* void AD5300_Init()  */


void AD5300_Test() 
{
int iIdx;

	while (1) 
	{
#if defined(SH_FOPS)
		/* Cyclically toggle GPIO ports ON */
		for (iIdx = 0; iIdx < sizeof(GPIOs)/sizeof(GPIOs[0]);iIdx++ )
		{
//.			common_printf("[%s] [%s] toggling GPIO %s ON\n",__FILE__, __func__ , GPIOs[iIdx] );
			OnGPIO( GPIOs[iIdx] );
		}

		/* Cyclically toggle GPIO ports OFF */
		for (iIdx = 0; iIdx < sizeof(GPIOs)/sizeof(GPIOs[0]);iIdx++ )
		{
//.			common_printf("[%s] [%s] toggling GPIO %s OFF\n",__FILE__, __func__ , GPIOs[iIdx] );
			OffGPIO( GPIOs[iIdx] );
		}
#else		

#if 0
		/* For each CPE port except last one (which is going to be USB 3.0, and conseq. requires special handling) */
		for (iIdx = 0; iIdx < NUM_PORTS - 1 /* skip USB 3.0 Port */;iIdx++ )

		{
			/* Test */
			_i_AD5300_Write_W(0xAA, iIdx); /* CH1: blue oscilloscope beam */
			_i_AD5300_Write_G(0x88, iIdx); /* CH2: yellow oscilloscope beam */
		}
#else

		_i_AD5300_Write_W( 0 , 0);
			_i_AD5300_Write_G(0xFF, 0);
		_i_AD5300_Write_W(0x11, 0);
			_i_AD5300_Write_G( 0, 0);
		_i_AD5300_Write_W(0xAA, 0);
			_i_AD5300_Write_G( 0x11, 0);
		_i_AD5300_Write_W(0xFF, 0);
			_i_AD5300_Write_G( 0xAA, 0);

#endif /* (0) */


#endif /* defined(SH_FOPS) */

	} /* while (1) */

} /* void AD5300_Test()  */

/* Closing opened port */
void AD5300_Deinit() 
{
int iIdx;

#if !defined(SH_FOPS)

	/* Open GPIO value files and store file poniters in array <GPIO_VALUE_FILES> */
	for (iIdx = 0; iIdx < sizeof(GPIO_VALUE_FILES)/sizeof(GPIO_VALUE_FILES[0]);iIdx++ )
	{
//.		common_printf("[%s] [%s] closing GPIO value file for GPIO %s \n",__FILE__, __func__ , GPIOs[iIdx] );

		/* Try to close value file <pcCmdBuffer> */
		if ( 0 != fclose ( GPIO_VALUE_FILES[iIdx] ) )
		{
			common_printf("[%s] [%s] can't close GPIO value file  <%s>. Nevertheless proceeding with next one\n", __FILE__, __func__ , pcCmdBuffer);
		}

	}/* for (iIdx = 0; ... */

#endif
}/* void AD5300_Deinit() */
