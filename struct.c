/*
 * Copyright (c) 2021 Embeddora LLC konstantin.mauch@embeddora.com All rights reserved
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
 * Abstract: we all but me are made of stars.
 *                         ___
 *                        ',_`""\        .---,
 *                           \   :-""``/`    |
 *                            `;'     //`\   /
 *                            /   __     |   ('.
 *                           |_ ./O)\     \  `) \
 *                          _/-.    `      `"`  |`.
 *                      .-=; `                  /  ;.
 *                     /o o \   ,_,           .'  '  ;
 *                     L._._;_.-'           .'   '   ;'
 *                       `'-.`           .-'        ,
 *                           `.        .'       ..;'
 *                             '-._. -'     ..-'
 *                              ./, \ ...--'
 *                            ,',\../',
 *                          ," ` /. .  ;
 *                          :   ; :  : :
 *                          `.,'  `,   ;
 *                           "     :   :
 *                                  ",'
 */

#include <linux/module.h>
#include <linux/kernel.h>

/* kmalloc() , kfree(), GFP_KERNEL */
#include <linux/slab.h>

/* struct _QuasiFloatType, struct _TimepointType, etc */
#include "struct.h"

/* _i_AD5300_Write_W(..), etc */
#include "hal.h"

/* Multiplier to get decimal data for ADxx converter from volatge value */
#define CONV_MAX_SCALE 255

/* MAX voltage to be emitted by converter within currennt platform */
#define VDD_VOLTAGE 	5

/* Time measurement variable to define begin of time scale */
struct timeval starttimePROC;

/* First time, will be subtracted in HAL */
QuasiFloatType qfltFIRST;

/* The memory for next time point can't be allocated */
#define P_ERROR_MEM		(-80)

/* The memory for next time point can't be allocated */
#define P_SUCCESS		(0)

/* The memory allocated alright, or time point processed well */
#define DEBUG_DATA		1

/*!
 * @brief		Normalize and send point value to the hardware
 *			
 *
 * \param[in] 		.
 * \param[in] 		.
 * \param[out] 	.
 *
 * return		.
 */
void _ProcessPoint(const char * caller, pTimepointType pTimepoint, int iPortIdx)
{
#if 0
#if defined(DEBUG_DATA)
	printk("[%s] %s:%s : <%d:%d:> <%d:%d> <%d:%d> <%s> \n",
		__FILE__, caller, __func__,
		pTimepoint->qfltAbsTime.integer, pTimepoint->qfltAbsTime.fraction,
		pTimepoint->qfltXval.integer, pTimepoint->qfltXval.fraction,
		pTimepoint->qfltYval.integer, pTimepoint->qfltYval.fraction, 
		pTimepoint->pcMarquee);
#endif /* (DEBUG_DATA) */
#endif /* (0) */

	/* TODO: move this smothing out of here, say to 'PreprocessPoints(..)' */
	/* Realtime and soft-time values */

	//.if (pTimepoint->ushQuadAvgYval > 255) pTimepoint->ushQuadAvgYval = 255;  
	//.if (pTimepoint->ushQuadAvgXval > 255) pTimepoint->ushQuadAvgXval = 255;

	/* Put current value on 'green' wire */
	_i_AD5300_Write_G(pTimepoint->ushQuadAvgXval, iPortIdx);

	/* Put current value on 'white' wire */
	_i_AD5300_Write_W(pTimepoint->ushQuadAvgYval, iPortIdx);
}

/*!
 * @brief		Attach 3 quiasi-floats to tail of dynamic structure 'pTimepointType'
 *			
 *
 * \param[in] 		.
 * \param[in] 		.
 * \param[out] 	.
 *
 * return		.
 */
int _EnrollPoint(const char * caller,
	pTimepointType * ppThisPointChain, 
	QuasiFloatType * pqfltTm, QuasiFloatType * pqfltX, QuasiFloatType * pqfltY, 
	char * pcMrq)
{
pTimepointType pChild, pTempPointChain;

	if (NULL == *ppThisPointChain)
	{
		/* only one chain, for beginning */
		*ppThisPointChain = (pTimepointType) kmalloc ( sizeof (TimepointType), GFP_KERNEL );
		memset (*ppThisPointChain, 0, sizeof (TimepointType) );

		/* check if successful */
		if (NULL == *ppThisPointChain)
		{
			printk("[%s] %s:%s: ERROR: can't allocate memory for FIRST element. %d.%d: [X(%d.%d),Y(%d.%d)]  \n",
			__FILE__, caller, __func__,
			pqfltTm->integer, pqfltTm->fraction,   pqfltX->integer, pqfltX->fraction,    pqfltY->integer, pqfltY->fraction );

			return P_ERROR_MEM;
		}

		memcpy ( &((*ppThisPointChain)->qfltXval),    pqfltX,  sizeof (struct _QuasiFloatType) ) ;
		memcpy ( &((*ppThisPointChain)->qfltYval),    pqfltY,  sizeof (struct _QuasiFloatType) );
		memcpy ( &((*ppThisPointChain)->qfltAbsTime), pqfltTm, sizeof (struct _QuasiFloatType) );


		/* Remove parasitic values (mostly they're same by modul, and different by sign) */
		if ( (*ppThisPointChain)->qfltXval.integer < 0) (*ppThisPointChain)->qfltXval.integer *= (-1);
		if ( (*ppThisPointChain)->qfltYval.integer < 0) (*ppThisPointChain)->qfltYval.integer *= (-1);

		/* 0.01 becomes 1, and placed on new scale */
		(*ppThisPointChain)->ushRawXval = (*ppThisPointChain)->qfltXval.integer * CONV_MAX_SCALE / VDD_VOLTAGE;
		(*ppThisPointChain)->ushRawYval = (*ppThisPointChain)->qfltYval.integer * CONV_MAX_SCALE / VDD_VOLTAGE;
#if (0)
		// TODO: heed the XXXX.fraction part of q-float
#endif /* 0 */


		/* Don't let the values to be higher than highest value allowed for converter */
		(*ppThisPointChain)->ushRawXval = min(CONV_MAX_SCALE, (*ppThisPointChain)->ushRawXval);
		(*ppThisPointChain)->ushRawYval = min(CONV_MAX_SCALE, (*ppThisPointChain)->ushRawYval);


		(*ppThisPointChain)->pcMarquee = kmalloc ( strlen (pcMrq) +1, GFP_KERNEL );
		strcpy( (*ppThisPointChain)->pcMarquee, pcMrq);

		/* TODO: add cmt */
		memcpy ( &qfltFIRST, pqfltTm, sizeof (struct _QuasiFloatType) );

#if defined(_DEBUG_DATA)
		printk("[%s] %s:%s: FIRST <%d:%d>    <%d:%d>[%d] <%d:%d>[%d]     <%s> \n",
			__FILE__, caller, __func__,
			(*ppThisPointChain)->qfltAbsTime.integer, (*ppThisPointChain)->qfltAbsTime.fraction,
			(*ppThisPointChain)->qfltXval.integer, (*ppThisPointChain)->qfltXval.fraction,        (*ppThisPointChain)->ushRawXval,
			(*ppThisPointChain)->qfltYval.integer, (*ppThisPointChain)->qfltXval.fraction,        (*ppThisPointChain)->ushRawYval,
			(*ppThisPointChain)->pcMarquee);
#endif /* (DEBUG_DATA) */

		/* No element to precede a first one */
		(*ppThisPointChain)->pPrev = NULL;
	}
	else
	{
		/* point with first temporary element to head of chain */
		pChild = *ppThisPointChain;

		pTempPointChain = (pTimepointType) kmalloc ( sizeof (TimepointType), GFP_KERNEL );
		memset (pTempPointChain, 0, sizeof (TimepointType) );

		if (NULL == pTempPointChain)
		{
			printk("[%s] %s:%s: ERROR: can't allocate memory for NEXT element. %d.%d: [X(%d.%d),Y(%d.%d)]  \n",
			__FILE__, caller, __func__,
			pqfltTm->integer, pqfltTm->fraction,   pqfltX->integer, pqfltX->fraction,    pqfltY->integer, pqfltY->fraction );

			return P_ERROR_MEM;
		}

		memcpy ( & (pTempPointChain->qfltXval),    pqfltX,  sizeof (struct _QuasiFloatType) );
		memcpy ( & (pTempPointChain->qfltYval),    pqfltY,  sizeof (struct _QuasiFloatType) );
		memcpy ( & (pTempPointChain->qfltAbsTime), pqfltTm, sizeof (struct _QuasiFloatType) );

		/* Remove parasitic values (mostly they're same by modul, and different by sign) */
		if ( pTempPointChain->qfltXval.integer < 0) pTempPointChain->qfltXval.integer *= (-1);
		if ( pTempPointChain->qfltYval.integer < 0) pTempPointChain->qfltYval.integer *= (-1);

		/* 0.01 becomes 1, and placed on new scale */
		pTempPointChain->ushRawXval = pTempPointChain->qfltXval.integer * CONV_MAX_SCALE / VDD_VOLTAGE;
		pTempPointChain->ushRawYval = pTempPointChain->qfltYval.integer * CONV_MAX_SCALE / VDD_VOLTAGE;

		/* Don't let the values to be higher than highest value allowed for converter */
		pTempPointChain->ushRawXval = min(CONV_MAX_SCALE, pTempPointChain->ushRawXval);
		pTempPointChain->ushRawYval = min(CONV_MAX_SCALE, pTempPointChain->ushRawYval);
#if (0)
		// TODO: heed the XXXX.fraction part of q-float
#endif /* 0 */

		pTempPointChain->pcMarquee = kmalloc ( strlen (pcMrq) +1, GFP_KERNEL );
		strcpy( pTempPointChain->pcMarquee, pcMrq);

#if defined(_DEBUG_DATA)
		printk("[%s] %s:%s : NEXT <%d:%d>          <%d:%d> [%d] <%d:%d> [%d] <%s> \n",
			__FILE__, caller, __func__,
			pTempPointChain->qfltAbsTime.integer, pTempPointChain->qfltAbsTime.fraction, 
			pTempPointChain->qfltXval.integer, pTempPointChain->qfltXval.fraction,    pTempPointChain->ushRawXval,
			pTempPointChain->qfltYval.integer, pTempPointChain->qfltYval.fraction,    pTempPointChain->ushRawYval,
			pTempPointChain->pcMarquee);
#endif /* (DEBUG_DATA) */

		/* Skip everything, except last entry */
		while ( (NULL != pChild) && (NULL != pChild->pNext ) )
		{
			/* . . */
			pChild = pChild->pNext;
		}

		/* Point onto previous el't */
		pTempPointChain->pPrev = pChild;

		/* Next chunk was created allright (we know it at this moment), so we attach a new chain entry to the end of existing chain */
		pChild->pNext = pTempPointChain;
	}

	return P_SUCCESS;

} /* int _EnrollPoint( ... ) */


/*!
 * @brief		Preprocess data stored in dynamic structure pointed by 'pPointChainPar'
 *			
 *
 * \param[in] 		.
 * \param[in] 		.
 * \param[out] 	.
 *
 * return		.
 */
int _PreProcessPoints(const char * caller, pTimepointType pPointChainPar)
{
pTimepointType pPointChain = pPointChainPar;

	/* Process each entry of chain */
	while (NULL != pPointChain)
	{
	unsigned short ushPrevXval, ushPrevYval, ushNextXval, ushNextYval;
	
#if defined(_DEBUG_DATA)
		printk("[%s] %s:%s : <%d:%d:> <%d:%d> <%d:%d> <%s> \n",
			__FILE__, caller, __func__,
			pPointChain->qfltAbsTime.integer, pPointChain->qfltAbsTime.fraction,
			pPointChain->qfltXval.integer, pPointChain->qfltXval.fraction,
			pPointChain->qfltYval.integer, pPointChain->qfltYval.fraction, 
			pPointChain->pcMarquee);
#endif /* (DEBUG_DATA) */

		/* Pick up previous X,Y (e.g. 'white','green') value into <ushPrevXval>,<ushPrevYval> */  
		if (NULL != pPointChain->pPrev)
		{
			ushPrevXval = pPointChain->pPrev->ushRawXval;

			ushPrevYval = pPointChain->pPrev->ushRawYval;
		}
		/* use current one if impossible to get previous one */
		else
		{
			ushPrevXval = pPointChain->ushRawXval;

			ushPrevYval = pPointChain->ushRawYval;
		}

		/* Pick up next X,Y (e.g. 'white','green') value into <ushPrevXval>,<ushPrevYval>  */ 
		if (NULL != pPointChain->pNext)
		{
			ushNextXval = pPointChain->pNext->ushRawXval;

			ushNextYval = pPointChain->pNext->ushRawYval;
		}
		/* use current one if impossible to get next one */
		else
		{
			ushNextXval = pPointChain->ushRawXval;

			ushNextYval = pPointChain->ushRawYval;
		}

		/* Compute the arithm. mean */
		pPointChain->ushQuadAvgXval =	(ushPrevXval +
				pPointChain->ushRawXval +
						ushNextXval) / 3;

		/* Compute the arithm. mean */
		pPointChain->ushQuadAvgYval =	(ushPrevYval +
				pPointChain->ushRawYval +
						ushNextYval) / 3;

		/* Go to next record of chain */
		pPointChain = pPointChain->pNext;
	}

	return P_SUCCESS;

} /* int _ProcessPoints(const char * caller, pTimepointType pPointChainPar) */

/*!
 * @brief		Free memory occupied by '*ppThisPointChain'
 *
 * \param[in] 		.
 * \param[in] 		.
 * \param[out] 	.
 *
 * return		.
 */
void _DeletePoints(const char * caller, pTimepointType * ppThisPointChain)
{
pTimepointType pChild, pThisPointChain = *ppThisPointChain;

	/* Walk through entire list and delete each chain */
	while (NULL != pThisPointChain)
	{
		/* if space to keep item's name is allocated */
		if (pThisPointChain->pcMarquee)
		
		    /* then release this space */
		    kfree(pThisPointChain->pcMarquee);

		/* preserve a pointer to next record */		    
		pChild = pThisPointChain->pNext;
		
		/* free space occupied by current record */
		kfree (pThisPointChain);
		
		/* Go to next record */
		pThisPointChain = pChild;
	}

	/* Dispose first element of chain */
	*ppThisPointChain = NULL;

} /* void _DeletePoints(const char * caller, pTimepointType * ppThisPointChain) */
