#ifndef _STUCT_H_
#define _STUCT_H_

/*
 * Copyright (c) 2021 EDDR konstantin.mauch@ All rights reserved
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

/* TODO: add comment */
#undef max

/* TODO: add comment */
#undef min

/* TODO: add comment */
#define max(x,y)	((x>y)?x:y)

/* TODO: add comment */
#define min(x,y)	((x<y)?x:y)


/* TODO: add comment */
typedef struct _QuasiFloatType
{
	/* Integer part of quasi-float */
	int integer;

	/* Fractal part of quasi-float */
	int fraction;

}  QuasiFloatType, *pQuasiFloatType;



/* List of D+(d.IN) and D-(d.OUT) values with relative time points */
typedef struct _TimepointType
{
	/* String to descibe this tm. point */
	char * pcMarquee;

	/* 0 - 256 for ADxx for 'white' */
	unsigned short ushRawXval;

	/* 0 - 256 for ADxx for 'green' */
	unsigned short ushRawYval;

	/* Average qudratic for 'white' */
	unsigned short ushQuadAvgXval;

	/* Average qudratic for 'green' */
	unsigned short ushQuadAvgYval;	

	/* D- , green wire of USB wire quartet */
	QuasiFloatType qfltXval;

	/* D+ , white wire of USB wire quartet */
	QuasiFloatType qfltYval;

	/* CSV's time stamp */
	QuasiFloatType qfltAbsTime;

	/* Next time point in the chain */
	struct _TimepointType * pNext;

	/* Previous time point */
	struct _TimepointType * pPrev;

} TimepointType, *pTimepointType;


int _ProcessPoints(const char * caller, pTimepointType pPointChainPar, int iPortIndex);

void _ProcessPoint(const char * caller, pTimepointType pTimepoint, int iPortIdx);

void _DeletePoints(const char * caller, pTimepointType * ppThisPointChain);

int _PreProcessPoints(const char * caller, pTimepointType pPointChainPar);

int _EnrollPoint(const char * caller, pTimepointType * ppThisPointChain, 
	QuasiFloatType * pqfltTm, QuasiFloatType * pqfltX, QuasiFloatType * pqfltY, 
	char * pcMrq);
	
#define EnrollPoint(x, y, s, t, u) _EnrollPoint(__func__, (x), (y), (s), (t), (u))

#define ProcessPoints(x, y) _ProcessPoints(__func__, (x), (y))

#define PreProcessPoints(x) _PreProcessPoints(__func__, (x))

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
	#define ProcessPoint(x, y) _ProcessPoint(__func__, (x), (y))
#pragma GCC diagnostic pop


#define DeletePoints(x) _DeletePoints(__func__, (x))

#endif /* _STUCT_H_ */
