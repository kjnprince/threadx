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
/*    _tx_queue_delete                                    PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function deletes the specified queue.  All threads suspended   */ 
/*    on the queue are resumed with the TX_DELETED status code.           */ 
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
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_queue_delete(TX_QUEUE *queue_ptr)
{

TX_INTERRUPT_SAVE_AREA

TX_THREAD       *thread_ptr;                /* Working thread pointer  */


    /* Disable interrupts to remove the queue from the created list.  */
    TX_DISABLE

    /* Log this kernel call.  */
    TX_EL_QUEUE_DELETE_INSERT

    /* Decrement the number of queues created.  */
    _tx_queue_created_count--;

    /* Clear the queue ID to make it invalid.  */
    queue_ptr -> tx_queue_id =  0;

    /* See if the queue is the only one on the list.  */
    if (queue_ptr == queue_ptr -> tx_queue_created_next)
    {

        /* Only created queue, just set the created list to NULL.  */
        _tx_queue_created_ptr =  TX_NULL;
    }
    else
    {

        /* Link-up the neighbors.  */
        (queue_ptr -> tx_queue_created_next) -> tx_queue_created_previous =
                                            queue_ptr -> tx_queue_created_previous;
        (queue_ptr -> tx_queue_created_previous) -> tx_queue_created_next =
                                            queue_ptr -> tx_queue_created_next;

        /* See if we have to update the created list head pointer.  */
        if (_tx_queue_created_ptr == queue_ptr)
            
            /* Yes, move the head pointer to the next link. */
            _tx_queue_created_ptr =  queue_ptr -> tx_queue_created_next; 
    }

    /* Temporarily disable preemption.  */
    _tx_thread_preempt_disable++;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Walk through the queue list to resume any and all threads suspended
       on this queue.  */
    thread_ptr =  queue_ptr -> tx_queue_suspension_list;    
    while (queue_ptr -> tx_queue_suspended_count)
    {
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

        /* Set the return status in the thread to TX_DELETED.  */
        thread_ptr -> tx_suspend_status =  TX_DELETED;

        /* Move the thread pointer ahead.  */
        thread_ptr =  thread_ptr -> tx_suspended_next;

        /* Resume the thread.  */
        _tx_thread_resume(thread_ptr -> tx_suspended_previous);

        /* Decrease the suspended count.  */
        queue_ptr -> tx_queue_suspended_count--;
    }

    /* Disable interrupts.  */
    TX_DISABLE

    /* Release previous preempt disable.  */
    _tx_thread_preempt_disable--;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Check for preemption.  */
    if (_tx_thread_current_ptr != _tx_thread_execute_ptr)

        /* Transfer control to system.  */
        _tx_thread_system_return();

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

