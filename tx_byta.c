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
/*    _tx_byte_allocate                                   PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function allocates bytes from the specified memory byte        */ 
/*    pool.                                                               */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    pool_ptr                          Pointer to pool control block     */ 
/*    memory_ptr                        Pointer to place allocated bytes  */ 
/*                                        pointer                         */
/*    memory_size                       Number of bytes to allocate       */ 
/*    wait_option                       Suspension option                 */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                            Completion status                 */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_activate                Activate timer routine            */ 
/*    _tx_thread_suspend                Suspend thread service            */ 
/*    _tx_byte_pool_search              Search byte pool for memory       */ 
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
/*  11-11-1997     Andrea Temple            Added rounding to memory size */ 
/*                                            to ensure ULONG alignment   */ 
/*                                            and modified comments,      */ 
/*                                            resulting in version 3.0b.  */ 
/*  03-01-1998     William E. Lamie         Optimized post RESTORE        */ 
/*                                            processing, resulting in    */ 
/*                                            version 3.0d.               */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_byte_allocate(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr, 
                                    ULONG memory_size,  ULONG wait_option)
{

TX_INTERRUPT_SAVE_AREA

REG_1   UINT        status;                 /* Return status              */
REG_2   TX_THREAD   *thread_ptr;            /* Working thread pointer     */
REG_3   CHAR_PTR    work_ptr;               /* Working byte pointer       */


    /* Pickup the thread pointer.  */
    thread_ptr =  _tx_thread_current_ptr;

    /* Round the memory size up to the next size that is evenly divisable by
       an ULONG.  This guarantees proper alignment.  */
    memory_size = ((memory_size + sizeof(ULONG)-1)/sizeof(ULONG)) * sizeof(ULONG);

    /* Disable interrupts.  */
    TX_DISABLE

    /* Log this kernel call.  */
    TX_EL_BYTE_ALLOCATE_INSERT

    /* Loop to handle cases where the owner of the pool changed.  */
    do
    {

        /* Indicate that this thread is the current owner.  */
        pool_ptr -> tx_byte_pool_owner =  thread_ptr;

        /* Restore interrupts.  */
        TX_RESTORE

        /* At this point, the executing thread owns the pool and can perform a search
           for free memory.  */
        work_ptr =  _tx_byte_pool_search(pool_ptr, memory_size);

        /* Lockout interrupts.  */
        TX_DISABLE

    } while ((!work_ptr) && (pool_ptr -> tx_byte_pool_owner != thread_ptr));

    /* Determine if memory was found.  */
    if (work_ptr)
    {

        /* Copy the pointer into the return destination.  */
        *memory_ptr =  (VOID *) work_ptr;

        /* Set the status to success.  */
        status =  TX_SUCCESS;
    }
    else
    {

        /* No memory of sufficient size was found...  */

        /* Determine if the request specifies suspension.  */
        if (wait_option)
        {

            /* Prepare for suspension of this thread.  */
 
            /* Setup cleanup routine pointer.  */
            thread_ptr -> tx_suspend_cleanup =  _tx_byte_pool_cleanup;

            /* Setup cleanup information, i.e. this pool control
               block.  */
            thread_ptr -> tx_suspend_control_block =  (VOID_PTR) pool_ptr;

            /* Save the return memory pointer address as well.  */
            thread_ptr -> tx_additional_suspend_info =  (VOID_PTR) memory_ptr;

            /* Save the byte size requested.  */
            thread_ptr -> tx_suspend_info =  memory_size;

            /* Setup suspension list.  */
            if (pool_ptr -> tx_byte_pool_suspension_list)
            {

                /* This list is not NULL, add current thread to the end. */
                thread_ptr -> tx_suspended_next =      
                        pool_ptr -> tx_byte_pool_suspension_list;
                thread_ptr -> tx_suspended_previous =  
                        (pool_ptr -> tx_byte_pool_suspension_list) -> tx_suspended_previous;
                ((pool_ptr -> tx_byte_pool_suspension_list) -> tx_suspended_previous) -> tx_suspended_next =  
                        thread_ptr;
                (pool_ptr -> tx_byte_pool_suspension_list) -> tx_suspended_previous =   thread_ptr;
            }
            else
            {

                /* No other threads are suspended.  Setup the head pointer and
                   just setup this threads pointers to itself.  */
                pool_ptr -> tx_byte_pool_suspension_list =  thread_ptr;
                thread_ptr -> tx_suspended_next =           thread_ptr;
                thread_ptr -> tx_suspended_previous =       thread_ptr;
            }

            /* Increment the suspended thread count.  */
            pool_ptr -> tx_byte_pool_suspended_count++;

            /* Set the state to suspended.  */
            thread_ptr -> tx_state =       TX_BYTE_MEMORY;

            /* Set the suspending flag.  */
            thread_ptr -> tx_suspending =  TX_TRUE;

            /* Temporarily disable preemption.  */
            _tx_thread_preempt_disable++;

            /* Save the timeout value.  */
            thread_ptr -> tx_thread_timer.tx_remaining_ticks =  wait_option;

            /* Restore interrupts.  */
            TX_RESTORE

            /* See if we need to start a timer.  */
            if (wait_option != TX_WAIT_FOREVER)
            {

                /* A timeout is required.  */

                /* Activate the thread timer for the timeout.  */
                _tx_timer_activate(&(thread_ptr -> tx_thread_timer));
            }

            /* Call actual thread suspension routine.  */
            _tx_thread_suspend(thread_ptr);

            /* Return the completion status.  */
            return(thread_ptr -> tx_suspend_status);
        }
        else
        {
    
            /* Immediate return, return error completion.  */
            status =  TX_NO_MEMORY;
        }
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return completion status.  */
    return(status);
}

