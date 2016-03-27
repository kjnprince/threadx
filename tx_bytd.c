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
/**   Byte Pool (BYT)                                                     */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"
#include    "tx_tim.h"
#include    "tx_byt.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_byte_pool_delete                                PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function deletes the specified byte pool.  All threads         */ 
/*    suspended on the byte pool are resumed with the TX_DELETED status   */ 
/*    code.                                                               */ 
/*                                                                        */ 
/*    It is important to note that the byte pool being deleted, or the    */ 
/*    memory associated with it should not be in use when this function   */ 
/*    is called.                                                          */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    pool_ptr                          Pointer to pool control block     */ 
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
UINT    _tx_byte_pool_delete(TX_BYTE_POOL *pool_ptr)
{

TX_INTERRUPT_SAVE_AREA

TX_THREAD       *thread_ptr;                /* Working thread pointer  */


    /* Disable interrupts to remove the byte pool from the created list.  */
    TX_DISABLE

    /* Log this kernel call.  */
    TX_EL_BYTE_POOL_DELETE_INSERT

    /* Decrease byte pool created count.  */
    _tx_byte_pool_created_count--;

    /* Clear the byte pool ID to make it invalid.  */
    pool_ptr -> tx_byte_pool_id =  0;

    /* See if the byte pool is the only one on the list.  */
    if (pool_ptr == pool_ptr -> tx_byte_pool_created_next)
    {

        /* Only created byte pool, just set the created list to NULL.  */
        _tx_byte_pool_created_ptr =  TX_NULL;
    }
    else
    {

        /* Link-up the neighbors.  */
        (pool_ptr -> tx_byte_pool_created_next) -> tx_byte_pool_created_previous =
                                            pool_ptr -> tx_byte_pool_created_previous;
        (pool_ptr -> tx_byte_pool_created_previous) -> tx_byte_pool_created_next =
                                            pool_ptr -> tx_byte_pool_created_next;

        /* See if we have to update the created list head pointer.  */
        if (_tx_byte_pool_created_ptr == pool_ptr)
            
            /* Yes, move the head pointer to the next link. */
            _tx_byte_pool_created_ptr =  pool_ptr -> tx_byte_pool_created_next; 
    }

    /* Temporarily disable preemption.  */
    _tx_thread_preempt_disable++;

    /* Restore interrupts.  */
    TX_RESTORE

    /* Walk through the byte pool list to resume any and all threads suspended
       on this byte pool.  */
    thread_ptr =  pool_ptr -> tx_byte_pool_suspension_list;    
    while (pool_ptr -> tx_byte_pool_suspended_count)
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
        pool_ptr -> tx_byte_pool_suspended_count--;
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

