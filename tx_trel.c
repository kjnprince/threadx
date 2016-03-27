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


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_relinquish                               PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function moves the currently executing thread to the end of    */ 
/*    the list of threads ready at the same priority. If no other threads */ 
/*    of the same or higher priority are ready, this function simply      */ 
/*    returns.                                                            */ 
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
/*    _tx_thread_system_return              Return to the system          */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  03-01-1998     William E. Lamie         Added logic to clear the      */ 
/*                                            preempted bit if the thread */ 
/*                                            relinquishes control,       */ 
/*                                            resulting in version 3.0d.  */ 
/*  01-01-1999     William E. Lamie         Corrected problem with        */ 
/*                                            asynchronous priority       */ 
/*                                            changes after the thread    */ 
/*                                            priority has been retrieved */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
VOID    _tx_thread_relinquish(VOID)
{

TX_INTERRUPT_SAVE_AREA
REG_1 UINT      priority;               /* Thread priority              */
REG_2 TX_THREAD *thread_ptr;            /* Thread priority head pointer */


    /* Pickup the current thread pointer.  */
    thread_ptr =  _tx_thread_current_ptr;

    /* Lockout interrupts while thread attempts to relinquish control.  */
    TX_DISABLE

    /* Log this kernel call.  */
    TX_EL_THREAD_RELINQUISH_INSERT

    /* Pickup the thread's priority.  */
    priority =  thread_ptr -> tx_priority; 

    /* Determine if there is another thread at the same priority.  */
    if (thread_ptr -> tx_ready_next != thread_ptr)
    {

        /* Yes, there is another thread at this priority, make it the highest at
           this priority level.  */
        _tx_thread_priority_list[priority] =  thread_ptr -> tx_ready_next;
	
        /* Mark the new thread as the one to execute.  */
        _tx_thread_execute_ptr = thread_ptr -> tx_ready_next;

        /* Determine if there is anything in the preempted bit map.  */
        if (_tx_thread_preempted_map)
        {

            /* Clear the bit for this priority level.  */
            _tx_thread_preempted_map =  _tx_thread_preempted_map & ~(thread_ptr -> tx_priority_bit);
        }
    }

    /* Determine if there is a higher-priority thread ready.  */
    if (_tx_thread_highest_priority < priority)
    {

        /* Yes, there is a higher priority thread ready to execute.  Make
           it visible to the thread scheduler.  */
        _tx_thread_execute_ptr =  _tx_thread_priority_list[_tx_thread_highest_priority];
    }

    /* Restore previous interrupt posture.  */
    TX_RESTORE

    /* Determine if this thread needs to return to the system.  */
    if (_tx_thread_execute_ptr != thread_ptr)

        /* Transfer control to the system so the scheduler can execute
           the next thread.  */
        _tx_thread_system_return();
}
