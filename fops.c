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
 
#include <linux/fs.h>

#include <asm/segment.h>

#include <asm/uaccess.h>

#include <linux/buffer_head.h>

#include "fops.h"
 
/* Open the file: relative path for k.modules and absolute - for thread(s) tsrtaed by it */
struct file *file_open(const char *path, int flags, int rights) 
{
struct file *filp = NULL;

mm_segment_t oldfs;

int err = 0;

    oldfs = get_fs();

    set_fs(get_ds());

    filp = filp_open(path, flags, rights);

    set_fs(oldfs);

    if (IS_ERR(filp))
    {
        err = PTR_ERR(filp);

        return NULL;
    }

    return filp;
    
} /* struct file *file_open(const char *path, int flags, int rights)  */

void file_close(struct file *file) 
{
    filp_close(file, NULL);
    
} /* void file_close(struct file *file) */

/* Raed a portion of file, return '0' or negative if can't */
static int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
mm_segment_t oldfs;

int ret;

    oldfs = get_fs();

    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);

    return ret;
    
} /* int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) */

/* Read a potion of bytes from binary file, put a trailing '\0' once newline is encountered, thus make a text string of it; return butes in this strings ot KNOP_EOF - otherwise */
int file_readln(struct file *file, unsigned long long offset, unsigned char *data, unsigned int * size) 
{
char * _data;

int iRet = 0, iFrRet;

	/* Exclude fro consideration all negatives including -EBADF, -EINVAL, -EFAULT, -EINVAL */
	if ( 0 > (iFrRet = file_read(file, offset, _data = data, LARGE_BUF_SZ) ) )

		/* Some ill-fated read or such */
		return KNOP_EOF;
	
	while (  (   ('\n' != *_data) && ( KNOP_ETX != *_data)       ) && ( iRet++ < LARGE_BUF_SZ ) ) 

		_data++; 

	/* There's no text in this file left to read, even trailg '\0' is ain't not there */
	if ((KNOP_ETX == *_data) || (0 == iFrRet) )  

		/* EOF as it is */
		return KNOP_EOF;
	
	data[*size = iRet] = '\0';

	return iRet;
    
} /* int file_readln(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) */
