#ifndef _FOPS_H_
#define _FOPS_H_

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

/* We refuse to read strings larger that this */
#define LARGE_BUF_SZ	200
/* End of text on this platform */
#define KNOP_ETX	(3)
/* End of file, any negative should suffice */
#define KNOP_EOF	(-2)

/*
 * Tis fella has written rillicool wrapper fo da 'filp_open()', et al:
 * https://stackoverflow.com/questions/1184274/read-write-files-within-a-linux-kernel-module
 * Actually dat woz me who shoulda do 'at (shit on me). But, anyway, I gonna
 * use dat wrapper as was just mine, all mine. (Shit on me once more.)
 */

struct file *file_open(const char *path, int flags, int rights);

void file_close(struct file *file);

int file_readln(struct file *file, unsigned long long offset, unsigned char *data, unsigned int * size);

#endif /* _FOPS_H_ */
