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
/* stdout */
#include <stdio.h>

/* strlen() */
#include <string.h>

/* calloc() */
#include <stdlib.h>

/* struct timeval, gettimeofday() */
#include <sys/time.h>

/* sqrt() */
#include <math.h>

#else

/* kfree(), ... */
#include <linux/slab.h>

/* gettimeofday(), ... */
#include <linux/time.h>

unsigned short sqrt(unsigned short z) {return 0;}

#endif /* (0) */

/* Data structure type definition */
#include "datastruct.h"

/* Errcode definitions */
#include "modsim.h"

#if (0)
#define max(x,y)	((x>y)?x:y)
#define min(x,y)	((x<y)?x:y)
#endif /* (0) */

/* Multiplier to get decimal data for ADxx converter from volatge value */
#define CONV_MAX_SCALE 255

/* MAX voltage to be emitted by converter within currennt platform */
#define VDD_VOLTAGE 	5


int ProcessPoint( pTimepointType pTimepoint ) { return 0; }


/* Time measurement variable to define begin of time scale */
struct timeval starttimePROC;

/* First time, will be subtracted in HAL */
#if (0)
float fFIRST;
#else
QuasiFloatType fFIRST;
#endif /* (0) */

/* Attach 3 floats to tail of dynamic structure 'pTimepointType' */
#if (0)
int _EnrollPoint(const char * caller, pTimepointType * ppThisPointChain, 
	float * pfltTm, float * pfltX, float * pfltY, 
	char * pcMrq)
#else
int _EnrollPoint(const char * caller, pTimepointType * ppThisPointChain, 
	QuasiFloatType * pfltTm, QuasiFloatType * pfltX, QuasiFloatType * pfltY, 
	char * pcMrq)
#endif /* (0) */
{
pTimepointType pChild, pTempPointChain;

	if (NULL == *ppThisPointChain)
	{
#if (0)
		/* only one chain, for beginning */
		*ppThisPointChain = (pTimepointType) calloc ( 1, sizeof (TimepointType) );
#else
                *ppThisPointChain = (pTimepointType) kcalloc ( 1, sizeof (TimepointType), GFP_USER);
#endif /* (0) */

		/* check if successful */
		if (NULL == *ppThisPointChain)
		{
#if (0)
			printk("[%s] %s:%s : ERROR: can't allocate memory for first element. %f: [X(%f),Y(%f)]  \n",
			*pfltTm, *pfltX, *pfltY);
#endif /* (0) */

			return P_ERROR_MEM;
		}

#if (0)
		(*ppThisPointChain)->qfltXval = *pfltX;
		(*ppThisPointChain)->qfltYval = *pfltY;
		(*ppThisPointChain)->qfltAbsTime = *pfltTm;

		/* Remove parasitic values (mostly they're same by modul, and different by sign) */
		if ( (*ppThisPointChain)->qfltXval < 0.0) (*ppThisPointChain)->qfltXval *= (-1);
		if ( (*ppThisPointChain)->qfltYval < 0.0) (*ppThisPointChain)->qfltYval *= (-1);
#endif /* (0) */

#if (0)
		/* 0.01 becomes 1, and placed on new scale */
		(*ppThisPointChain)->ushRawXval = (*ppThisPointChain)->qfltXval * CONV_MAX_SCALE / VDD_VOLTAGE;
		(*ppThisPointChain)->ushRawYval = (*ppThisPointChain)->qfltYval * CONV_MAX_SCALE / VDD_VOLTAGE;


		/* Don't let the values to be higher than highest value allowed for converter */
		(*ppThisPointChain)->ushRawXval = min(CONV_MAX_SCALE, (*ppThisPointChain)->ushRawXval);
		(*ppThisPointChain)->ushRawYval = min(CONV_MAX_SCALE, (*ppThisPointChain)->ushRawYval);
#endif /* (0) */

#if (0)
		(*ppThisPointChain)->pcMarquee = calloc (1, strlen (pcMrq) +1 );
#else
		(*ppThisPointChain)->pcMarquee = kcalloc (1, strlen (pcMrq) +1, GFP_USER);
#endif /* (0) */
		strcpy( (*ppThisPointChain)->pcMarquee, pcMrq);

		// TODO: rework
		fFIRST = (*ppThisPointChain)->qfltAbsTime;


#if defined(DEBUG_DATA)
		printk("FIRST <%f> <%f>[%d] <%f>[%d] <%s> \n",
			(*ppThisPointChain)->qfltAbsTime,
			(*ppThisPointChain)->qfltXval, (*ppThisPointChain)->ushRawXval,
			(*ppThisPointChain)->qfltYval, (*ppThisPointChain)->ushRawYval,
			(*ppThisPointChain)->pcMarquee
		);
#endif /* (DEBUG_DATA) */

		/* No first el't */
		(*ppThisPointChain)->pPrev = NULL;
	}
	else
	{
		/* point with first temporary element to head of chain */
		pChild = *ppThisPointChain;

#if (0)
		pTempPointChain = (pTimepointType) calloc (1, sizeof (TimepointType) );
#else
		pTempPointChain = (pTimepointType) kcalloc (1, sizeof (TimepointType), GFP_USER);
#endif /* (0) */

		if (NULL == pTempPointChain)
		{
#if (0)
			printk("ERROR: can't allocate memory for next element. %f: [X(%f),Y(%f)]  \n",
			*pfltTm, *pfltX, *pfltY);
#else

#endif /* (0) */

			return P_ERROR_MEM;
		}

#if (0)
		pTempPointChain->qfltXval = *pfltX;
		pTempPointChain->qfltYval = *pfltY;
		pTempPointChain->qfltAbsTime = *pfltTm;

		/* Remove parasitic values (mostly they're same by modul, and different by sign) */
		if ( pTempPointChain->qfltXval < 0.0) pTempPointChain->qfltXval *= (-1);
		if ( pTempPointChain->qfltYval < 0.0) pTempPointChain->qfltYval *= (-1);
#endif /* (0) */



#if (0)

		/* 0.01 becomes 1, and placed on new scale */
		pTempPointChain->ushRawXval = pTempPointChain->qfltXval * CONV_MAX_SCALE / VDD_VOLTAGE;
		pTempPointChain->ushRawYval = pTempPointChain->qfltYval * CONV_MAX_SCALE / VDD_VOLTAGE;


		/* Don't let the values to be higher than highest value allowed for converter */
		pTempPointChain->ushRawXval = min(CONV_MAX_SCALE, pTempPointChain->ushRawXval);
		pTempPointChain->ushRawYval = min(CONV_MAX_SCALE, pTempPointChain->ushRawYval);
#endif /* (0) */


#if (0)
		pTempPointChain->pcMarquee = calloc (1, strlen (pcMrq) +1 );
#else
		pTempPointChain->pcMarquee = kcalloc (1, strlen (pcMrq) +1, GFP_USER);
#endif /* (0) */
		strcpy( pTempPointChain->pcMarquee, pcMrq);


#if defined(DEBUG_DATA)
		printk("NEXT <%f> <%f>[%d] <%f>[%d] <%s> \n",
			pTempPointChain->qfltAbsTime,
			pTempPointChain->qfltXval, pTempPointChain->ushRawXval,
			pTempPointChain->qfltYval, pTempPointChain->ushRawYval,
			pTempPointChain->pcMarquee
		);
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


// TODO: _2_DEL . TESTING REALDATA CURVES on OSCILL. remove!
struct timeval starttime_A,  endtime_A;// TODO: _2_DEL . TESTING REALDATA CURVES on OSCILL. remove!


/* Process data stored in dynamic structure pointed by 'pPointChainPar' */
int _ProcessPoints(const char * caller, pTimepointType pPointChainPar)
{
pTimepointType pPointChain = pPointChainPar;

#if DEBUG_DATA_
//pTimepointType pPointChainRESERV__2DEL = pPointChainPar;// TODO: _2_DEL . TESTING REALDATA CURVES on OSCILL. remove!
FLOAT qfltAbsTime;
#endif /* (DEBUG_DATA) */

double timeusePROC;

	/* Take initial time. Current time values will be taken in 'ProcRealAndRel()' */
#if (0)
	gettimeofday(&starttimePROC, 0);
#else
	do_gettimeofday(&starttimePROC);
#endif /* (0) */

#if (0)
gettimeofday(&starttime_A, 0);
#else
do_gettimeofday(&starttime_A);
#endif /* (0) */
	/* Process each entry of chain */
	while (NULL != pPointChain)
	{
#if DEBUG_DATA_
		printk("<%f> <%f> <%f> <%s> \n",
			pPointChain->qfltAbsTime,
			pPointChain->qfltXval,
			pPointChain->qfltYval,
			pPointChain->pcMarquee
		);
#endif /* (DEBUG_DATA) */

		unsigned short ushPrevXval, ushPrevYval, ushNextXval, ushNextYval;
		
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

		/* Compute the average quadratic X as QUAD_AVG=SQRT(Prev*Prev + Curr*Curr + Next*Next )*/
		pPointChain->ushQuadAvgXval = sqrt(ushPrevXval*ushPrevXval +
			pPointChain->ushRawXval*pPointChain->ushRawXval +
			ushNextXval*ushNextXval);

		/* Compute the average quadratic Y as QUAD_AVG=SQRT(Prev*Prev + Curr*Curr + Next*Next )*/
		pPointChain->ushQuadAvgYval = sqrt(ushPrevYval*ushPrevYval +
			pPointChain->ushRawYval*pPointChain->ushRawYval +
			ushNextYval*ushNextYval);

		/* Realtime and relative-time values */
		ProcessPoint(pPointChain);

		/* Go to next record of chain */
		pPointChain = pPointChain->pNext;
	}
#if (0)
gettimeofday(&endtime_A, 0);
#else
do_gettimeofday(&endtime_A);
#endif



#if (0)
FLOAT elapsedtime_A = 1000000*(endtime_A.tv_sec - starttime_A.tv_sec - fFIRST) 	+ endtime_A.tv_usec - starttime_A.tv_usec;
printf(">>> time elapsed : %f <<<\n", elapsedtime_A);
#else

#endif /* (0) */


	return P_SUCCESS;

} /* int _ProcessPoints(const char * caller, pTimepointType pPointChainPar) */

/* Free memory occupied by '*ppThisPointChain' */
void _DeletePoints(const char * caller, pTimepointType * ppThisPointChain)
{
pTimepointType pChild, pThisPointChain = *ppThisPointChain;

	/* Walk through entire list and delete each chain */
	while (NULL != pThisPointChain)
	{
		/* if space to keep item's name is allocated */
		if (pThisPointChain->pcMarquee)
		
		    /* then release this space */
#if (0)
		    free(pThisPointChain->pcMarquee);
#else
		    kfree(pThisPointChain->pcMarquee);
#endif /* (0) */

		/* preserve a pointer to next record */		    
		pChild = pThisPointChain->pNext;
		
		/* free space occupied by current record */
#if (0)
		free (pThisPointChain);
#else
		kfree (pThisPointChain);
#endif /* (0) */
		
		/* Go to next record */
		pThisPointChain = pChild;
	}

	/* Dispose first element of chain */
	*ppThisPointChain = NULL;

} /* void _DeletePoints(const char * caller, pTimepointType * ppThisPointChain) */
