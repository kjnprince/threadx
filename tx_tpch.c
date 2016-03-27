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
/**   Thread Control                                                      */
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
/*    _tx_thread_preemption_change                        PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function processes preemption threshold change requests.  The  */ 
/*    previous preemption is returned to the caller.  If the new request  */ 
/*    allows a higher priority thread to execute, preemption takes place  */ 
/*    inside of this function.                                            */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread             */ 
/*    new_threshold                         New preemption threshold      */ 
/*    old_threshold                         Old preemption threshold      */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Service return status         */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_thread_system_return              Return to system              */ 
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
/*  03-01-1998     William E. Lamie         Added logic to clear this     */ 
/*                                            priority of the preempted   */ 
/*                                            bit map if the threshold is */ 
/*                                            lowered to the priority of  */ 
/*                                            the specified thread,       */ 
/*                                            resulting in version 3.0d.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold,
                    UINT *old_threshold)
{

TX_INTERRUPT_SAVE_AREA
UINT    status;                         /* Return status            */


    /* Lockout interrupts while the thread is being resumed.  */
    TX_DISABLE

    /* Determine if the preemption threshold is valid.  */
    if (new_threshold <= thread_ptr -> tx_priority)
    {

        /* Legal new preemption threshold. */

        /* Log this kernel call.  */
        TX_EL_THREAD_PREEMPTION_CHANGE_INSERT

        /* Determine if the new threshold is the same as the
           priority.  */
        if (thread_ptr -> tx_priority == new_threshold)
        {

            /* Determine if this thread is at the head of the list.  */
            if (_tx_thread_priority_list[thread_ptr -> tx_priority] == thread_ptr)
            {

                /* Yes, this thread is at the front of the list.  Make sure
                   the preempted bit is cleared for this thread.  */
                _tx_thread_preempted_map =  
                                _tx_thread_preempted_map & ~(thread_ptr -> tx_priority_bit);
            }
        }

        /* Save the current preemption threshold.   */
        *old_threshold =  thread_ptr -> tx_preempt_threshold;

        /* Setup the new threshold.  */
        thread_ptr -> tx_preempt_threshold =  new_threshold;

        /* See if preemption needs to take place.  */
        if ((_tx_thread_execute_ptr == thread_ptr) &&
                (_tx_thread_highest_priority < thread_ptr -> tx_priority) &&
                (_tx_thread_highest_priority < new_threshold))
        {

            /* Preemption needs to take place.  */

            /* Setup the highest priority thread to execute.  */
            _tx_thread_execute_ptr =  _tx_thread_priority_list[_tx_thread_highest_priority];

            /* Restore interrupts.  */
            TX_RESTORE

            /* Transfer control to the system to execute the higher priority
               thread, if preemption is enabled.  */
            if (!_tx_thread_preempt_disable)

                /* Transfer control to system.  */
                _tx_thread_system_return();
            
            /* Disable interrupts again.  */
            TX_DISABLE
        }

        /* Set status to success.  */
        status =  TX_SUCCESS;
    }
    else

        /* Invalid preemption threshold.  */
        status =  TX_THRESH_ERROR;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return completion status.  */
    return(status);
}

