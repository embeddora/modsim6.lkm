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
 * Abstract: main routine of MODSIM-6 kernel driver.
 */
 
/* The 'MODULE_LICENSE(..)' and alike */
#include <linux/module.h>

/* The 'kmalloc()', 'kfree(..)', etc */
#include <linux/slab.h>

/* Using kobjects for the sysfs bindings */
#include <linux/kobject.h>

/* Using kthreads for the flashing functionality */
#include <linux/kthread.h> 

/* The 'msleep()' prototype */
#include <linux/delay.h>

/* The 'NAME_MAX' macro */
#include <linux/limits.h>

/* The 'O_RDWR', 'O_LARGEFILE' macros  */
#include <linux/fs.h>

/* The 'HW_ALL_PORTS' macro */
#include "knop.h"

/* The 'EnrollPoint(..)', 'PreProcessPoints(..)', 'DeletePoints(..)' prototypes  */
#include "struct.h"

/* Prototypes of 'file_open()', 'file_readln()', 'file_close()' */
#include "fops.h"

/* The 'OpenGPIO(..)', 'CloseGPIO(..)' prototypes */
#include "hal.h"

/* An option to advertise ourselves inside dmesg-output */
#define __func__		"[KNOP-W1]"

/* Enable/disable an output of intermediate valuesinto 'dmesg'*/
#define DEBUG_DATA		1

#define KNOP_SUCCESS		0
#define KNOP_ERR_NO_MEM	(-1)

/* Freeze delay (ms) to make the thread interruptible by 'kthread_should_stop()' */
#define STOP_DELAY		100

typedef void (*pWorkerFunction)(void*);

/* Hevily dependant on 'linux/kthread.h' entrails */
typedef int (*threadfn)	(void *data);

enum fsm_states		{INVITED=0, TO_BE_STARTED, STARTED, TO_BE_STOPPED, STOPPED, TO_BE_RESTARTED, UNDEFINED};

enum usb_type			{USB_UNDEFINED=0, USB2, USB3};

typedef struct _device
{
	/* ID of the MODSIM's port handled by the thread */
	int			iNum;

	/* Thread name */
	char			*pcName;

	/* The pointer to the thread task */
	struct task_struct	*stTask;
	
	/* Functino to operate in a thread */
	pWorkerFunction	PortWorkingLoop; 

	/* State of the MODSIM's port */
	enum fsm_states	iState;

#if 0
	/* Uppor limit for single time cell */
	int 			iThreshold;

	/* Amount of time cells to reproduce */
	int 			iScaLenght;
#endif /* (0) */

	/* A name fo a file to take data from  */	
	char			datafile_name[NAME_MAX];
	
	/* Pointer to a tree-shaped structure to store data from 'datafile_name' at */
	pTimepointType		pTimeChain;
	
	/* TODO: also can be a local var. in the port worker thread */
	struct file		*fp;

	/* A kind of USB the device simulates */
	enum usb_type		uchType;

	/* Amount of failed attempts to process this port */
	int			iFails;

} device, * pDevice;

pDevice  pDevices[HW_ALL_PORTS];

/* That's the way we process USB2-/USB3-protocol hardware */
static void port_worker(void *);

#if 1
/* Path to absolute as we're using it in forked thread (which knows no nothing about current process' destination) */	
static char datafile_name[NAME_MAX] = "/home/debian/time_kernel/data.txt";
/* Can read, can't change */
module_param_string(data, datafile_name, NAME_MAX, S_IRUGO);
/* Let the user to know abot first parameter */
MODULE_PARM_DESC(data, "A relative path to data to be emitted via at appliance's ports");
#endif /* 0 */


/* Amount of times to emit the data */
static unsigned int retries = 0;
/* Can read, can't change */
module_param(retries, uint, S_IRUGO);
/* Let the user to know abot first parameter */
MODULE_PARM_DESC(retries, "Amount of repeats");


/* Null terminated */
static char HwName[10] = "MODSIM-6";

/* The pointer to the kobject */
static struct kobject *modsim_kobj;

/*!
 * @brief		A callback function to display the LED period
 */
static ssize_t retries_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return KNOP_SUCCESS;
}

/*!
 * @brief		A callback function to display the LED period
 */
static ssize_t retries_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	return KNOP_SUCCESS;
}

/*!
 * @brief		A callback function to display the LED period
 */
static ssize_t datafile_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return KNOP_SUCCESS;
}

/*!
 * @brief		A callback function to store the LED period value
 */
static ssize_t datafile_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	return KNOP_SUCCESS;
}

/*!
 * @brief		Use these helper macros to define the name and access levels of the
 *			kobj_attributes. The kobj_attribute has an attribute attr (name and
 *			mode), show and store function pointers The period variable is
 *			associated with the blinkPeriod variable and it is to be exposed with
 *			mode 0666 using the period_show and period_store functions above
 */
static struct kobj_attribute datafile_attr = __ATTR(datafile, 0664, datafile_show, datafile_store);

/*!
 * @brief		
 */
static struct kobj_attribute retries_attr = __ATTR(retries, 0664, retries_show, retries_store);

/*!
 * @brief		The modsim_attrs[] is an array of attributes that is used to create
 *			the attribute group below. The attr property of the kobj_attribute
 *			is used to extract the attribute struct
 */
static struct attribute *modsim_attrs[] =
{
	/* The period at which the LED flashes */
	&datafile_attr.attr,
	
	/* Is the LED on or off? */
	&retries_attr.attr,
	
	NULL,
};

/*!
 * @brief
 */
static struct attribute_group attr_group =
{
	/* The name of a driver; a default one */
	.name  = HwName,

	/* The attributes array defined just above */
	.attrs = modsim_attrs,
};


/*!
 * @brief		The port worker
 *
 * \param[in] 		pDevStruct A pointer to 'struct _device', passed via 'param2' of
 *			preceeding 'kthread_run(param1, >>>param2<<<, param3)' ( see
 *			'modsim6_init(...)' for details)
 *
 * return		None for 2 independent reasons: a) there's no easy way to check this
 *			functions return value from the main thread; 2.) there's no way to
 *			terminate the loop implmented inside unless it's interrupted by thread
 *			termination (to come from outside, or the main thread)
 */
static void port_worker(void * pDevStruct)
{
/* Local device structure pointer */
pDevice pDev = (pDevice)pDevStruct; 

/* Time measurements (nsec) port before and after data has been emitted to a port */
ktime_t start, end;

/* Amount of nanoseconds last port writing operation has taken */
s64 s64MaxLastStepTime, s64LastStepTime;

/* Max time (ns) for emiting data to a port. (Heavily dependent on 'rata.txt' which is heavily dependent 'get_RawData.py'.) */
int iMaxDuration;

	/* Assign protocol timing constants */	
	switch (pDev->uchType)
	{
		/*  480 Mbit/s */
		case USB2:
		{
			/* Provides 3.5K single port writings per second. (Normally, 200 port writings should suffice.) */
			s64MaxLastStepTime = 280000;

			/* As writing to USB 2.0 reveals no apparent issues let's keep it (positive an) as low as possible */
			iMaxDuration = 5;
			break;
		}
		/*  up to 4.8 Gbit/s */
		case USB3:
		{
			/* Provides 4K single port writings per second. (Normally, 200 port writings should suffice.) */
			s64MaxLastStepTime = 250000         		    + 20000;

			/* As writing to the USB 3.0 is not that semless as to USB 2.0, let's use more atttempt for writing  */
			iMaxDuration = 10;
			break;
		}
		case USB_UNDEFINED:
		default:
		{
			printk(KERN_ALERT "pDev->datafile_name: unknown hardware kind=%d", pDev->uchType);
			
			pDev->iState = STOPPED;
			break;
		}
		
	} /* switch (pDev->uchType) */

	while( !kthread_should_stop() )
	{
		switch (pDev->iState)
		{
		/* Cross-case migrating, that's why here */
		pTimepointType	pPoint = NULL;
		
			/* Open and get data from 'datafile_name'; enroll it; and close the file afterwards */
			case INVITED: 
			{
			/* Buffer to store more-than-single-line obtained from datafile  */
			char cBuf [LARGE_BUF_SZ];

			/* Will initialize them explicitly */
			int iPosition, iOldSec, iRet;

				//.printk(KERN_INFO "about to start %s worker thread\n", pDev->pcName);

				if ( NULL == (pDev->fp = file_open (pDev->datafile_name, O_RDWR | O_LARGEFILE, 0) ) )
				{
					printk(KERN_ALERT "Can't open datafile <%s>. Be to provide correct _absolute path.", pDev->datafile_name);
					pDev->iState = STOPPED;
					break;
				}


				//.printk(KERN_INFO "Loading USB-data from file <%s>\n", pDev->datafile_name);

				/* Zero'ing - 'must-do' beffo entering the while-loop */
				pDev->pTimeChain = NULL;				
				/* Zeroing is must */
				iPosition = iOldSec=0;

				while (KNOP_EOF != (iRet = file_readln(pDev->fp, iPosition, cBuf, &iRet) ) )  
				{
				/* Aux. buffer to keep results of parsing */
				char * cpTmp = cBuf;
				QuasiFloatType qfltDTM, qfltDIn, qfltDOut;
				
					iPosition += (iRet+1);
					
					/* For each character in aux. buffer */
					while (*cpTmp)

						/* replace all commas with spaces, to let the <scanf()> parse it */
						{ if (',' == *cpTmp) *cpTmp = ' '; cpTmp++; }

					/* Set default values. MISRA RULE #TODO */
					memset (&qfltDTM,  0, sizeof (struct _QuasiFloatType) );
					memset (&qfltDIn,  0, sizeof (struct _QuasiFloatType) );
					memset (&qfltDOut, 0, sizeof (struct _QuasiFloatType) );

					/* Find 3 floats separated by spaces in aux. buffer */
					sscanf(cBuf, "%d.%d %d.%d %d.%d",
									&qfltDTM.integer, &qfltDTM.fraction,
									&qfltDIn.integer, &qfltDIn.fraction,
									&qfltDOut.integer, &qfltDOut.fraction );

					/* Attach just scanned data (three floats) to tail of dynamic structure */
					_EnrollPoint(pDev->pcName, &(pDev->pTimeChain), &qfltDTM, &qfltDIn, &qfltDOut, __func__);
				}

				file_close(pDev->fp);
				//.printk(KERN_INFO "Closed <%s> alright", datafile_name);

				OpenGPIO(pDev->iNum);
				//.printk(KERN_INFO "Opened the port #%d", pDev->iNum);

				pDev->iFails = 0;
				pDev->iState = TO_BE_RESTARTED;
				break;
				
			} /* case INVITED */
			
			case TO_BE_RESTARTED:
				pPoint = pDev->pTimeChain;
				//.printk(KERN_INFO "A pointer to fist entry is taken. pPointChain=<%p> pDev->pTimeChain=<%p> alright \n", pPoint, pDev->pTimeChain);
				_PreProcessPoints(pDev->pcName, pDev->pTimeChain);
				//.printk(KERN_INFO "Pre-Processed pTimeChain=<%p> alright", pDev->pTimeChain);

				// 1. stop the thread presumable working 
				// 2. Fall down into 'TO_BE_STARTED' case
			case TO_BE_STARTED:
				//printk(KERN_INFO "starting %s worker thread...\n", pDev->pcName);
				pDev->iState = STARTED;
				printk(KERN_INFO "%s worker STARTED\n", pDev->pcName);
				start = ktime_get();
				break;

			case STARTED:
				if ( ( NULL != pPoint) && ( pDev->iFails < iMaxDuration) )
				{
#if 0
#pragma GCC diagnostic push
/* Gag the compiler which does not see that 'pPoint' alredy has already has been initialized on 'INVITED' state */
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif /* (0) */
					_ProcessPoint(pDev->pcName, pPoint, pDev->iNum);
#if 0
#pragma GCC diagnostic pop
#endif /* (0) */
					end = ktime_get();
					s64LastStepTime = ktime_to_ns(ktime_sub(end, start));
#if 0
#pragma GCC diagnostic push
/* Gag the compiler which does not see that 's64MaxLastStepTime' alredy has already has been initialized at thread entrance */
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"					
#endif /* (0) */
					if (s64LastStepTime > s64MaxLastStepTime)
#if 0
#pragma GCC diagnostic pop
#endif /* (0) */		
					{
						printk("%s restarting due inacceptable internal delay %lld ", pDev->pcName, (long long)s64LastStepTime);
						pDev->iState = TO_BE_RESTARTED;
						pDev->iFails++;
					}

					pPoint = pPoint->pNext;
				}
				else
				{
#if 1
#pragma GCC diagnostic push
/* Gag the compiler which does not see that 's64LastStepTime', 's64LastStepTime' alredy have been initialized */
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"					
					printk("%s: Mission %s [iRetries=%d, s64LastStepTime=%lld]", pDev->pcName, ( iMaxDuration == pDev->iFails)? "failed":"accomplished", pDev->iFails, (long long)s64LastStepTime);
#endif /* (0) */
					pDev->iState = TO_BE_STOPPED;
				}				
				
				set_current_state(TASK_INTERRUPTIBLE);

				start = ktime_get();
				break;

			case TO_BE_STOPPED:
				//.printk(KERN_INFO "Stopping %s worker ...\n", pDev->pcName);

				/* Dispose pointer to Raw Data file */
				DeletePoints(&(pDev->pTimeChain));
				//.printk(KERN_INFO "Disposed pTimeChain=<%p> alright", pDev->pTimeChain);

				CloseGPIO(pDev->iNum);
				//.printk(KERN_INFO "Closed the port #%d", pDev->iNum);

				printk(KERN_INFO "%s: STOPPED (as %s)", pDev->pcName,  ( iMaxDuration == pDev->iFails)? "interrupted":"finalized" );
				/* To let the thread presumably working be stopped just fall down into case 'STOPPED' */
				pDev->iState = STOPPED;

			case STOPPED:
				/* Stay looping idly to remain capable of being interrupted on demand (current comes from module_exit() ) */
				set_current_state(TASK_INTERRUPTIBLE);
				msleep(STOP_DELAY);
				break;
				
			default:
			case UNDEFINED:
				printk(KERN_INFO "%s: undefined state=%d of FSM. (Unloaded the driver, the sooner the better for the hardware\n", pDev->pcName, pDev->iState);
				break;
				
		} /* switch (pDev->iState) */

	} /* 	while( !kthread_should_stop() ) */
}

/* Stop and clean everything started and allocated on interations preceding <i-th> iteration */
/*!
 * @brief		Stop and clean everything started and allocated on interations preceding <i-th> iteration
 *
 * \param[in] 		iteration standing next to all these iteratino to be cleaned 
 *
 * return		None
 */
static void vDisposeStopCleanPorts(int i)
{
/* Cycle counter */
int j;

	for (j = 0; j < i; j++)
	{
		pDevices[j]->iState = TO_BE_STOPPED;

		/* Let the worker thread transit to 'CLOSED'. Note: this works only in atomic context */
		msleep(STOP_DELAY);

		if (NULL != pDevices[j]->stTask)

			kthread_stop(pDevices[j]->stTask);
			
		kfree(pDevices[j]->pcName);

		kfree(pDevices[j]);
	}

} /* static void vCleanPrevious(int i) */

/* TODO: remove this temporary garbage */
static void vDisposeStopCleanPorts_noFSM(int i)
{
/* Cycle counter */
int j;

	for (j = 0; j < i; j++)
	{
		if (NULL != pDevices[j]->stTask)

			kthread_stop(pDevices[j]->stTask);
			
		kfree(pDevices[j]->pcName);

		kfree(pDevices[j]);
	}

} /* static void vCleanPrevious(int i) */

/*!
 * @brief		Set uo structures and launch port processing threads (workers)
 *
 * \param[in] 		None
 * \param[out] 	None
 *
 * return		0 - success, .. 
 */
static int __init modsim6_init(void)
{
/* Tempo6rary result storage */
int iRes = 0; 

/* Cycle counter */
int i;

	printk(KERN_INFO "MODSIM: Initializing the MODSIM LKM\n");
	
	/* Create kernel object to point to a '/sys/kernel' */
	if( NULL == (modsim_kobj = kobject_create_and_add("modsim-6", kernel_kobj->parent) ) )
	{
		printk(KERN_ALERT "MODSIM: failed to create kobject\n");

		return -ENOMEM;
	}

	/* Add the attributes to '/sys/modsim-6/XXXX',  for example, /sys/modsim-6/led60/ledOn */
	if(0 != (iRes = sysfs_create_group(modsim_kobj, &attr_group) ) )
	{
		printk(KERN_ALERT "MODSIM: failed to create sysfs group\n");

		/* clean up -- remove the kobject sysfs entry */
		kobject_put(modsim_kobj);                

		return iRes;
	}

	/* For each ports of 'Modsim-6' hardware */
	for (i = 0; i < HW_ALL_PORTS; i++)
	{
		/* Allocate memory for port's structure, wait (even sleep) as lomg as it's required */
		pDevices[i] = kmalloc( sizeof (struct _device), GFP_KERNEL );

		/* Allocated allright? */
		if (NULL != pDevices[i])
		{
			/* Allocate placeholder for port's thread name */
			pDevices[i]->pcName = kmalloc( 0x40 , GFP_KERNEL ); 

			/* TODO: some better way: strcpy (datafile_name, argv[1]); */
			strcpy (pDevices[i]->datafile_name, datafile_name);

			/* Order index */		
			pDevices[i]->iNum = i;

			/* On real 'Modsim-6' hardware all but last port are USB2, whilst last one is of USB3 kind */
			(i != (HW_ALL_PORTS-1) ) ? ( pDevices[i]->uchType = USB2) : (pDevices[i]->uchType = USB3); 

#pragma GCC diagnostic push
/* As long as (1 < HW_ALL_PORTS) we don't face a devision by '0' in denominator, but the GCC compiler does not see this. (So let's gag it.) */
#pragma GCC diagnostic ignored "-Wdiv-by-zero"
			/* Port's thread name assignment.  USB2 - all but last oneUSB2, USB3 - last one */
			if (1 < HW_ALL_PORTS)

				/* Compose thread name for either USB2 or or USB3 port */
				sprintf(pDevices[i]->pcName, "USB%d Thread #%d", 2+(pDevices[i]->iNum)/(HW_ALL_PORTS-1), (pDevices[i]->iNum < HW_ALL_PORTS-1) ? pDevices[i]->iNum : 0) ;
			else
				/* Assign thread idex only. Name to be assigned manually (if has to).  */
				sprintf(pDevices[i]->pcName, "USB-X Thread #%d" , 0);
#pragma GCC diagnostic pop

			/* To define very initial state which preceeds all other possible states */
			pDevices[i]->iState = INVITED;

			/* Main device processing thread */
			pDevices[i]->PortWorkingLoop = port_worker; 

			/* Thread for each port is being started here */
			pDevices[i]->stTask = kthread_run((threadfn) (pDevices[i]->PortWorkingLoop), pDevices[i], pDevices[i]->pcName);
		}
		/* no memory? - destroy evrything started and allocated up to the moment */
		else
		{
			printk(KERN_ALERT "Failed to allocate GFP_KERNEL-memory for task %d. Destroying all and exiting\n", i);

			/* Clen/stop/deallocate things inialized on 1st, 2nd, .. ,  (i-1)th iterations */
			vDisposeStopCleanPorts(i);
						
			/* Un-make the kobject sysfs entry */
			kobject_put(modsim_kobj);

			/* Do not proceed and exit with success */
			return KNOP_ERR_NO_MEM;
		}
	}

	for (i = 0; i < HW_ALL_PORTS; i++)
	{
	int iRetVal = KNOP_SUCCESS;
	
		if ( IS_ERR ( pDevices[i]->PortWorkingLoop) )

			iRetVal = PTR_ERR(pDevices[i]->PortWorkingLoop); 
		
		if (KNOP_SUCCESS != iRetVal)
		{
			printk(KERN_ALERT "Failed to create task %d. Destroying all and exiting\n", i);

			/* Clen/stop/deallocate things inialized on 1st, 2nd, .. ,  (i-1)th iterations */
			vDisposeStopCleanPorts(i);
			
			/* Un-make the kobject sysfs entry */
			kobject_put(modsim_kobj);

			/* No use to exit with success */
			return iRetVal;
		}
	}
	
	printk(KERN_INFO "Latency Checker module init = %d (correct so far)\n", iRes);

	return KNOP_SUCCESS;
	
} /* static int __init modsim6_init(void) */

/*!
 * @brief		Cleanup function
 *
 * \param[in] 		None
 * \param[out] 	None
 *
 * return		None
 */
static void __exit modsim6_exit(void)
{
	/* Clen/stop/deallocate things inialized on 1st, 2nd, .. ,  (HW_ALL_PORTS-1)th iterations. FSM state is 'STOPPED', so what's ythe use to stop it again?  */	
	vDisposeStopCleanPorts_noFSM(HW_ALL_PORTS);

	printk(KERN_INFO "Dynamically allocated stuff disposed\n");
	
	/* remove the kobject sysfs entry */
	kobject_put(modsim_kobj);

	printk(KERN_INFO "Exit with '0' (exiting gracefully)\n");

} /* static void __exit modsim6_exit(void) */

/* Initialize 'sysfs' entries, allocate memory portioins, start working threads */
module_init(modsim6_init);

/* Stop the threads running, dispose allocated memory areas, unexport and deinit the rest odds and ends */
module_exit(modsim6_exit);

/* Provide a compatibility with 'gpio_xxx()' and other GPL-only symbols */
MODULE_LICENSE("GPL");

/* A name of  */
MODULE_AUTHOR("K. Mauch, konstantin.mauch@");

MODULE_DESCRIPTION("Hardware driver of 'Modem Simulator 6'");

MODULE_VERSION("0.9");
