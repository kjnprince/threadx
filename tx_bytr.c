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
/**   Byte Memory (BYT)                                                   */
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
/*    _tx_byte_release                                    PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function returns previously allocated memory to its            */ 
/*    associated memory byte pool.                                        */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    memory_ptr                        Pointer to allocated memory       */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    [TX_PTR_ERROR | TX_SUCCESS]       Completion status                 */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_deactivate              Deactivate timer routine          */ 
/*    _tx_thread_resume                 Resume thread service             */ 
/*    _tx_thread_system_return          Return to system routine          */ 
/*    _tx_byte_pool_search              Search the byte pool for memory   */ 
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
UINT    _tx_byte_release(VOID *memory_ptr)
{

TX_INTERRUPT_SAVE_AREA

REG_1   TX_BYTE_POOL    *pool_ptr;          /* Pool pointer               */
REG_2   TX_THREAD       *thread_ptr;        /* Working thread pointer     */
REG_3   CHAR_PTR        work_ptr;           /* Working block pointer      */
REG_4   TX_THREAD       *susp_thread_ptr;   /* Suspended thread pointer   */
UINT                    preempt =  0;       /* Preemption counter         */


    /* Determine if the memory pointer is valid.  */
    work_ptr =  (CHAR_PTR) memory_ptr;
    if (work_ptr)
    {
        
        /* Back off the memory pointer to pickup its header.  */
        work_ptr = work_ptr - sizeof(CHAR_PTR) - sizeof(ULONG);

        /* There is a pointer, pickup the pool pointer address.  */
        if (*((ULONG *) (work_ptr + sizeof(CHAR_PTR))) != TX_BYTE_BLOCK_FREE)
        {

            /* Pickup the pool pointer.  */
            pool_ptr =  (TX_BYTE_POOL *) *((TX_BYTE_POOL **) (work_ptr + sizeof(CHAR_PTR)));

            /* See if we have a valid pool.  */
            if ((!pool_ptr) || (pool_ptr -> tx_byte_pool_id != TX_BYTE_POOL_ID))
                return(TX_PTR_ERROR);
        }
        else
            return(TX_PTR_ERROR);
    }
    else
        return(TX_PTR_ERROR);

    /* At this point, we know that the pointer is valid.  */

    /* Pickup the thread pointer.  */
    thread_ptr =  _tx_thread_current_ptr;

    /* Indicate that this thread is the current owner.  */
    pool_ptr -> tx_byte_pool_owner =  thread_ptr;

    /* Lockout interrupts.  */
    TX_DISABLE

    /* Log this kernel call.  */
    TX_EL_BYTE_RELEASE_INSERT

    /* Release the memory.  */
    *((ULONG *) (work_ptr + sizeof(CHAR_PTR))) =  TX_BYTE_BLOCK_FREE;

    /* Update the number of available bytes in the pool.  */
    pool_ptr -> tx_byte_pool_available =  
            pool_ptr -> tx_byte_pool_available + (*((CHAR_PTR *) (work_ptr)) - work_ptr);

    /* Set the pool search value appropriately.  */
    pool_ptr -> tx_byte_pool_search =  work_ptr;

    /* Now examine the suspension list to find threads waiting for 
       memory.  Maybe it is now available!  */
    while(pool_ptr -> tx_byte_pool_suspended_count)
    {

        /* Pickup the first suspended thread pointer.  */
        susp_thread_ptr =  pool_ptr -> tx_byte_pool_suspension_list;

        /* Restore interrupts.  */
        TX_RESTORE

        /* See if the request can be satisfied.  */
        work_ptr =  _tx_byte_pool_search(pool_ptr, susp_thread_ptr -> tx_suspend_info);   

        /* Disable interrupts.  */
        TX_DISABLE

        /* If there is not enough memory, break this loop!  */
        if (!work_ptr)
           break;

        /* Check to make sure the thread is still suspended.  */
        if (susp_thread_ptr !=  pool_ptr -> tx_byte_pool_suspension_list)
        {

            /* Put the memory back on the available list since this thread is no longer
               suspended.  */
            work_ptr = work_ptr - sizeof(CHAR_PTR) - sizeof(ULONG);
            *((ULONG *) (work_ptr + sizeof(CHAR_PTR))) =  TX_BYTE_BLOCK_FREE;

            /* Update the number of available bytes in the pool.  */
            pool_ptr -> tx_byte_pool_available =  
                pool_ptr -> tx_byte_pool_available + (*((CHAR_PTR *) (work_ptr)) - work_ptr);

            /* Set the pool search value appropriately.  */
            pool_ptr -> tx_byte_pool_search =  work_ptr;

            /* Start at the top of this loop.  */
            continue;
        }

        /* Remove the suspended thread from the list.  */

        /* See if this is the only suspended thread on the list.  */
        if (susp_thread_ptr == susp_thread_ptr -> tx_suspended_next)
        {

            /* Yes, the only suspended thread.  */

            /* Update the head pointer.  */
            pool_ptr -> tx_byte_pool_suspension_list =  TX_NULL;
        }
        else
        {

            /* At least one more thread is on the same expiration list.  */

            /* Update the list head pointer.  */
            pool_ptr -> tx_byte_pool_suspension_list =  susp_thread_ptr -> tx_suspended_next;

            /* Update the links of the adjacent threads.  */
            (susp_thread_ptr -> tx_suspended_next) -> tx_suspended_previous =  
                                                    susp_thread_ptr -> tx_suspended_previous;
            (susp_thread_ptr -> tx_suspended_previous) -> tx_suspended_next =
                                                    susp_thread_ptr -> tx_suspended_next;
        } 
 
        /* Decrement the suspension count.  */
        pool_ptr -> tx_byte_pool_suspended_count--;

        /* Prepare for resumption of the thread.  */

        /* Clear cleanup routine to avoid timeout.  */
        susp_thread_ptr -> tx_suspend_cleanup =  TX_NULL;

        /* Temporarily disable preemption.  */
        _tx_thread_preempt_disable++;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Return this block pointer to the suspended thread waiting for
           a block.  */
        *((CHAR_PTR *) susp_thread_ptr -> tx_additional_suspend_info) =  (CHAR_PTR) work_ptr;

        /* Deactivate the timeout timer if necessary.  */
        if (susp_thread_ptr -> tx_thread_timer.tx_list_head)
        {

            /* Deactivate the thread's timeout timer.  */
            _tx_timer_deactivate(&(susp_thread_ptr -> tx_thread_timer));
        }
        else
        {
               
           /* Clear the remaining time, just in case it hasn't started yet.  */
           susp_thread_ptr -> tx_thread_timer.tx_remaining_ticks =  0;
        }

        /* Put return status into the thread control block.  */
        susp_thread_ptr -> tx_suspend_status =  TX_SUCCESS;

        /* Resume thread.  */
        preempt =  preempt + _tx_thread_resume(susp_thread_ptr);

        /* Lockout interrupts.  */
        TX_DISABLE
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Check for preemption.  */
    if (preempt)
    {
        /* Preemption is required, transfer control back to 
           system.  */
        _tx_thread_system_return();
    }

    /* Return completion status.  */
    return(TX_SUCCESS);
}

