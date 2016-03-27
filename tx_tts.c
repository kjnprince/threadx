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
/*    _tx_thread_time_slice                               PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function moves the currently executing thread to the end of    */ 
/*    the threads ready at the same priority level as a result of a       */ 
/*    time-slice interrupt.  If no other thread of the same priority is   */ 
/*    ready, this function simply returns.                                */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_TRUE                               If time-slice was done        */
/*    TX_FALSE                              If time-slice was not needed  */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_timer_interrupt                   Timer interrupt handling      */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_thread_time_slice(VOID)
{

TX_INTERRUPT_SAVE_AREA
REG_1 UINT          status;             /* Time-slice status flag	    */
REG_2 TX_THREAD     *thread_ptr;        /* Thread priority head pointer */


    /* Pickup the current thread pointer.  */
    thread_ptr =  _tx_thread_current_ptr;

    /* Default time-slice status to false.  A true value indicates this
       routine did indeed perform a time-slice.  */
    status =  TX_FALSE;

    /* Lockout interrupts while thread attempts to relinquish control.  */
    TX_DISABLE

    /* Make sure the thread is still active, i.e. not suspended.  */
    if (thread_ptr -> tx_state == TX_READY)
    {

        /* Setup a fresh time-slice for the thread.  */
        thread_ptr -> tx_time_slice =  thread_ptr -> tx_new_time_slice;

        /* Check to make sure preemption is enabled.  */
        if (_tx_thread_preempt_disable)
        {

            /* Preemption is disabled by the system, set time-slice to 1 for retry.  */
            thread_ptr -> tx_time_slice =  1;

            /* Set status to false.  */
            status =  TX_FALSE;
        }

        /* Determine if there is another thread at the same priority.  */
        else if ((thread_ptr -> tx_ready_next != thread_ptr) &&
                 (thread_ptr -> tx_priority == thread_ptr -> tx_preempt_threshold))
        {
        
            /* There is another thread at this priority, make it the highest at
               this priority level.  */
            _tx_thread_priority_list[thread_ptr -> tx_priority] =  thread_ptr -> tx_ready_next;
	
            /* Designate the highest priority thread as the one to execute.  Don't use this 
               thread's priority as an index just in case a higher priority thread is now 
               ready!  */
            _tx_thread_execute_ptr =  _tx_thread_priority_list[_tx_thread_highest_priority];

            /* Set the status to true to indicate a preemption is going to take
               place.  */
            status =  TX_TRUE;
        }
    }

    /* Restore previous interrupt posture.  */
    TX_RESTORE

    /* Return to caller.  */
    return(status);
}
