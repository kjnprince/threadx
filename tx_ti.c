/**************************************************************************/ 
/*                                                                        */ 
/*            Copyright (c) 1996-2000 by Express Logic Inc.               */ 
/*                                                                        */ 
/*  This software is copyrighted by and is the sole property of Express   */ 
/*  Logic, Inc.  All rights, title, ownership, or other interests         */ 
/*  in the software remain the property of Express Logic, Inc.  This      */ 
/*  software may only be used in accordance with the corresponding        */ 
/*  license agreement.  Any unauthorized use, duplication, transmission,  */ 
/*  distribution, or disclosure of this software is expressly forbidden.  */ 
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */ 
/*  written consent of Express Logic, Inc.                                */ 
/*                                                                        */ 
/*  Express Logic, Inc. reserves the right to modify this software        */ 
/*  without notice.                                                       */ 
/*                                                                        */ 
/*  Express Logic, Inc.                                                   */
/*  11440 West Bernardo Court               info@expresslogic.com         */
/*  Suite 366                               http://www.expresslogic.com   */
/*  San Diego, CA  92127                                                  */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** ThreadX Component                                                     */ 
/**                                                                       */
/**   Thread Control (THR)                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Locate thread control component data in this file.  */

#define TX_THREAD_INIT


/* Include necessary system files.  */

#include "tx_api.h"
#include "tx_ini.h"
#include "tx_thr.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_initialize                               PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function initializes the various control data structures for   */ 
/*    the thread control component.                                       */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    None                                                                */
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_initialize_high_level         High level initialization         */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  03-01-1998     William E. Lamie         Added initialization for a    */ 
/*                                            a new entry in the priority */ 
/*                                            list and added logic to     */ 
/*                                            initialize the preempted    */ 
/*                                            bit map, resulting in       */ 
/*                                            version 3.0d.               */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
VOID    _tx_thread_initialize(VOID)
{

REG_1 UINT          i;                  /* Working index variable     */ 
REG_2 UCHAR         set_bit;            /* Lowest set bit  		  */
REG_3 UINT          temp;               /* Working shift variable	  */ 
REG_4 UCHAR         *lowest_set_ptr;    /* Pointer in set bit array	  */
REG_5 TX_THREAD_PTR	*priority_list_ptr; /* Pointer in priority list   */


    /* Note: the system stack pointer and the system state variables are 
       initialized by the low and high-level initialization functions,
       respectively.  */

    /* Initialize the current thread pointer to NULL.  */
    _tx_thread_current_ptr =  TX_NULL;

    /* Initialize the execute thread pointer to NULL.  */
    _tx_thread_execute_ptr =  TX_NULL;

    /* Initialize the priority information.  */
    _tx_thread_priority_map =      0;
    _tx_thread_preempted_map =     0;
    _tx_thread_highest_priority =  TX_MAX_PRIORITIES;

    /* Initialize the lowest-set bit table.  This is used during thread
       suspension and resumption to find the next thread priority that
       is ready for execution.  */
    _tx_thread_lowest_bit[0] =  0;
    lowest_set_ptr =            &_tx_thread_lowest_bit[1];
    for (i = 1; i < TX_THREAD_MAX_BYTE_VALUES; i++)
    {

        /* Place the current index into the temp variable. */
	    temp =  i;

	    /* Find the lowest bit set in the byte.  */
        set_bit =  0;
        while (!(temp & 1))
        {

            /* Shift the index value down one and increment the
               set-bit position.  */
            temp =  temp >> 1;
            set_bit++;
        }

        /* At this point, "set_bit" contains the first
           bit set in the value represented by the index "i".  */
        *(lowest_set_ptr++) =  set_bit;
    }

    /* Initialize the array of priority head pointers.  */
    priority_list_ptr =  &_tx_thread_priority_list[0];
    for (i = 0; i < TX_MAX_PRIORITIES; i++)
        *(priority_list_ptr++) =    TX_NULL;

    /* Initialize the head pointer of the created threads list and the
       number of threads created.  */
    _tx_thread_created_ptr =        TX_NULL;
    _tx_thread_created_count =      0;

    /* Clear the global preempt disable variable.  */
    _tx_thread_preempt_disable =  0;
}

