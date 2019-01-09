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
 * Abstract: a sketch
 *
 */



#include <linux/init.h>

#include <linux/module.h>

#include <linux/kernel.h>

#include <linux/proc_fs.h>

#include <linux/uaccess.h>

#define BUFFER_SIZE 128

#define PROC_NAME "sketch_1"

#define MESSAGE "SKETCH_1_in_action\n"

ssize_t proc_read(struct file *file, char *buf, size_t count, loff_t *pos);

static struct file_operations proc_ops =
{
        .owner = THIS_MODULE,

        .read = proc_read,

}; /* struct file_operations proc_ops */

int proc_init(void)
{
        /* This is nothing but proc_create_data() with NULL as the last argument */
        proc_create(PROC_NAME, 0, NULL, &proc_ops);

        printk(KERN_INFO "/proc/%s created\n", PROC_NAME);

	return 0;

} /* int proc_init(void) */

void proc_exit(void)
{
        remove_proc_entry(PROC_NAME, NULL);

        printk( KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

/* Is called repeatedly until it returns 0, so there must be logic that ensures it ultimately returns 0 */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
int rv = 0;

char buffer[BUFFER_SIZE];

static int completed = 0;

        if (completed)
        {
                completed = 0;

                return 0;
        }

        completed = 1;

        rv = sprintf(buffer, "Hello World\n");

        /* Copies the contents of buffer to userspace usr_buf */
        copy_to_user(usr_buf, buffer, rv);

        return rv;
}



module_init( proc_init );

module_exit( proc_exit );

MODULE_LICENSE("GPL");

MODULE_DESCRIPTION("Hello Module");

MODULE_AUTHOR("SGG");
