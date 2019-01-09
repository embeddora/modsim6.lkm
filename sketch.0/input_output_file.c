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
 * Abstract:
 *
 */

/* For all modules */
#include <linux/module.h>

/* KERN_INFO */
#include <linux/kernel.h>

/* A prototype of filp()  */
#include <linux/fs.h>

/* Segment descriptors */
#include <linux/uaccess.h>


int init_module(void)
{
struct file *f;

char buf[128];

mm_segment_t fs;

        memset(buf, 0, 128);

        f = filp_open("/etc/_trpz", O_RDONLY, 0);

        if(f == NULL)

                printk(KERN_ALERT "Can't open a file from kernel module\n");
        else
        {
                /* Get current segment descriptor */
                fs = get_fs();

                /* Set segment descriptor associated to kernel space */
                set_fs(get_ds());

                /* Read a file */
                f->f_op->read(f, buf, 128, &f->f_pos);

                /* Restore segment descriptor */
                set_fs(fs);

                /* See what was read from a file */
                printk(KERN_INFO "buf:%s\n", buf);
        }

        filp_close(f,NULL);

        return 0;
}

void cleanup_module(void)
{
        printk(KERN_INFO "Unloaded\n");
}
