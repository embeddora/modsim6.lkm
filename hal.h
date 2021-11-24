#ifndef _HAL_H_
#define _HAL_H_

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

typedef int GPIOTarget;


/* Toggle down SCKL on 'white' converter */
#define _i_SCLK_LO_W(x)		OffGPIO(x)
/* Toggle up SCKL on 'white' converter */
#define _i_SCLK_HI_W(x)		OnGPIO(x)
/* Toggle down MOSI on 'white' converter */
#define _i_MOSI_LO_W(x)		OffGPIO(x)
/* Toggle up MOSI on 'white' converter */
#define _i_MOSI_HI_W(x)		OnGPIO(x)
/* Activate 'white' converter */
#define _i_AD5300_ACT_W(x) 		OffGPIO(x)
/* Deactivate 'white' converter */
#define _i_AD5300_DEACT_W(x) 		OnGPIO(x)

/* Toggle down SCKL on 'green' converter */
#define _i_SCLK_LO_G(x)		OffGPIO(x)
/* Toggle up SCKL on 'green' converter */
#define _i_SCLK_HI_G(x)		OnGPIO(x)
/* Toggle down MOSI on 'green' converter */
#define _i_MOSI_LO_G(x)		OffGPIO(x)
/* Toggle up MOSI on 'green' converter */
#define _i_MOSI_HI_G(x)		OnGPIO(x)


/* Activate 'green' converter */
#define _i_AD5300_ACT_G(x) 		OffGPIO(x)
/* Deactivate 'green' converter */
#define _i_AD5300_DEACT_G(x) 		OnGPIO(x)


void _i_AD5300_Write_W(unsigned char data, int iIdx);
void _i_AD5300_Write_G(unsigned char data, int iIdx);

void OpenGPIO(int iPort);
void CloseGPIO(int iPort);

#endif /* _HAL_H_ */
