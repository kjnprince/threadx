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
/**   Queue (QUE)                                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"
#include    "tx_tim.h"
#include    "tx_que.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_queue_flush                                     PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function resets the specified queue, if there are any messages */ 
/*    in it.  Messages waiting to be placed on the queue are also thrown  */ 
/*    out.                                                                */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    queue_ptr                         Pointer to queue control block    */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_SUCCESS                        Successful completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_deactivate              Deactivate timer routine          */ 
/*    _tx_thread_resume                 Resume thread service             */ 
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
/*  03-01-1998     William E. Lamie         Corrected uninitialized       */ 
/*                                            suspend count problem if    */ 
/*                                            nothing is enqueued,        */ 
/*                                            resulting in version 3.0d.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_queue_flush(TX_QUEUE *queue_ptr)
{

TX_INTERRUPT_SAVE_AREA

TX_THREAD       *suspension_list;           /* Pickup the suspension list head  */
UINT            suspended_count;            /* Count of suspended threads       */
TX_THREAD       *thread_ptr;                /* Working thread pointer           */


    /* Initialize the suspended count.  */
    suspended_count =  0;

    /* Disable interrupts to reset various queue parameters.  */
    TX_DISABLE

    /* Log this kernel call.  */
    TX_EL_QUEUE_FLUSH_INSERT

    /* Determine if there is something on the queue.  */
    if (queue_ptr -> tx_queue_enqueued)
    {

        /* Yes, there is something in the queue.  */

        /* Reset the queue parameters to erase all of the queued messages.  */
        queue_ptr -> tx_queue_enqueued =           0;
        queue_ptr -> tx_queue_available_storage =  queue_ptr -> tx_queue_capacity;
        queue_ptr -> tx_queue_read =               queue_ptr -> tx_queue_start;
        queue_ptr -> tx_queue_write =              queue_ptr -> tx_queue_start;

        /* Now determine if there are any threads suspended on a full queue.  */
        if (queue_ptr -> tx_queue_suspended_count)
        {

            /* Yes, there are threads suspended on this queue, they must be 
               resumed!  */

            /* Copy the information into temporary variables.  */
            suspension_list =  queue_ptr -> tx_queue_suspension_list;
            suspended_count =  queue_ptr -> tx_queue_suspended_count;

            /* Clear the queue variables.  */
            queue_ptr -> tx_queue_suspension_list =  TX_NULL;
            queue_ptr -> tx_queue_suspended_count =  0;

            /* Temporarily disable preemption.  */
            _tx_thread_preempt_disable++;
        }
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Walk through the queue list to resume any and all threads suspended
       on this queue.  */
    if (suspended_count)
    {

        /* Pickup the thread to resume.  */
        thread_ptr =  suspension_list;
        do
        {

            /* Resume the next suspended thread.  */
            
            /* Lockout interrupts.  */
            TX_DISABLE

            /* Clear the cleanup pointer, this prevents the timeout from doing 
               anything.  */
            thread_ptr -> tx_suspend_cleanup =  TX_NULL;

            /* Temporarily disable preemption again.  */
            _tx_thread_preempt_disable++;

            /* Restore interrupts.  */
            TX_RESTORE
    
            /* Yes, deactivate the thread's timer just in case.  */
            _tx_timer_deactivate(&(thread_ptr -> tx_thread_timer));

            /* Clear the remaining time to ensure timer doesn't get activated.  */
            thread_ptr -> tx_thread_timer.tx_remaining_ticks =  0;

            /* Set the return status in the thread to TX_SUCCESS.  */
            thread_ptr -> tx_suspend_status =  TX_SUCCESS;

            /* Move the thread pointer ahead.  */
            thread_ptr =  thread_ptr -> tx_suspended_next;

            /* Resume the thread.  */
            _tx_thread_resume(thread_ptr -> tx_suspended_previous);

            /* Continue while there are suspended threads.  */
        } while(--suspended_count);

        /* Restore previous preempt posture.  */
        _tx_thread_preempt_disable--;

        /* Check for preemption.  */
        if (_tx_thread_current_ptr != _tx_thread_execute_ptr)

            /* Transfer control to system.  */
            _tx_thread_system_return();
    }

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

