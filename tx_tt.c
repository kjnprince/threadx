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
#include    "tx_tim.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_terminate                                PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function handles application thread terminate requests.  Once  */ 
/*    a thread is terminated, it cannot be executed again unless it is    */ 
/*    deleted and re-created.                                             */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread to suspend  */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Return completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_deactivate                  Timer deactivate function     */ 
/*    Suspend Cleanup Routine               Suspension cleanup function   */ 
/*    _tx_thread_suspend                    Actual thread suspension      */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application code                                                    */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Added logic to prevent        */ 
/*                                            calling thread suspend if   */ 
/*                                            the thread being terminated */ 
/*                                            is not in a ready state,    */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_thread_terminate(TX_THREAD *thread_ptr)
{

TX_INTERRUPT_SAVE_AREA

VOID        (*suspend_cleanup)(struct TX_THREAD_STRUCT *);


    /* Deactivate thread timer, if active.  */
    _tx_timer_deactivate(&thread_ptr -> tx_thread_timer);

    /* Lockout interrupts while the thread is being resumed.  */
    TX_DISABLE

    /* Log this kernel call.  */
    TX_EL_THREAD_TERMINATE_INSERT

    /* Check the specified thread's current status.  */
    if ((thread_ptr -> tx_state != TX_COMPLETED) && 
                                (thread_ptr -> tx_state != TX_TERMINATED))
    {

        /* Check to see if the thread is currently ready.  */
        if (thread_ptr -> tx_state == TX_READY)
        {

            /* Set the state to terminated.  */
            thread_ptr -> tx_state =    TX_TERMINATED;

            /* Set the suspending flag.  */
            thread_ptr -> tx_suspending =  TX_TRUE;

            /* Disable internal preemption.  */
            _tx_thread_preempt_disable++;

            /* Since the thread is currently ready, we don't need to
               worry about calling the suspend cleanup routine!  */

            /* Restore interrupts.  */
            TX_RESTORE

            /* Call actual thread suspension routine.  */
            _tx_thread_suspend(thread_ptr);
        }
        else
        {

            /* Change the state to terminated.  */
            thread_ptr -> tx_state =    TX_TERMINATED;

            /* Set the suspending flag.  This prevents the thread from being 
               resumed before the cleanup routine is executed.  */
            thread_ptr -> tx_suspending =  TX_TRUE;

            /* Pickup the cleanup routine address.  */
            suspend_cleanup =  thread_ptr -> tx_suspend_cleanup;

            /* Restore interrupts.  */
            TX_RESTORE

            /* Call any cleanup routines.  */
            if (suspend_cleanup)

                /* Yes, there is a function to call.  */
                (suspend_cleanup)(thread_ptr);

            /* Clear the suspending flag.  */
            thread_ptr -> tx_suspending =  TX_FALSE;
        }
    }
    else
    {

        /* Restore interrupts.  */
        TX_RESTORE
    }

    /* Always return success, since this function does not perform 
       any real error checking.  */
    return(TX_SUCCESS);
}

