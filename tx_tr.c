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
/*    _tx_thread_resume                                   PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function places the specified thread on the list of ready      */ 
/*    threads at the thread's specific priority.  If a thread preemption  */ 
/*    is detected, this function returns a TX_TRUE.                       */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread to resume   */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_TRUE                               If preemption is necessary    */ 
/*    TX_FALSE                              If preemption is not needed   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_thread_create                     Thread create function        */ 
/*    _tx_thread_resume_api                 Application resume service    */ 
/*    Other ThreadX Components                                            */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  03-01-1998     William E. Lamie         Added logic to set preempted  */ 
/*                                            bit map when a thread with  */ 
/*                                            preemption threshold is     */ 
/*                                            preempted, resulting in     */ 
/*                                            version 3.0d.               */ 
/*  01-01-1999     William E. Lamie         Corrected a problem if a      */ 
/*                                            thread being terminated is  */ 
/*                                            resumed from an ISR,        */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_thread_resume(TX_THREAD *thread_ptr)
{

TX_INTERRUPT_SAVE_AREA
UINT                preemption;         /* Preempt condition flag       */
REG_1 UINT          priority;           /* Thread priority              */
REG_2 TX_THREAD     *head_ptr;          /* Thread priority head pointer */
REG_2 TX_THREAD     *tail_ptr;          /* Thread priority tail pointer */


    /* Initialize the preemption flag to false.  */
    preemption =  TX_FALSE;

    /* Lockout interrupts while the thread is being resumed.  */
    TX_DISABLE

    /* Decrease the preempt disabled count.  */
    _tx_thread_preempt_disable--;

    /* Determine if the thread is in the process of suspending.  If so, the thread
       control block is already on the linked list so nothing needs to be done.  */
    if (thread_ptr -> tx_suspending)
    {

        /* Make sure the type of suspension under way is not a terminate or
           thread completion.  In either of these cases, do not void the 
           interrupted suspension processing.  */
        if ((thread_ptr -> tx_state != TX_COMPLETED) &&
            (thread_ptr -> tx_state != TX_TERMINATED))
        {

            /* Clear the suspending flag.  */
            thread_ptr -> tx_suspending =   TX_FALSE;

            /* Restore the state to ready.  */
            thread_ptr -> tx_state =        TX_READY;
        }
    }

    /* Check to make sure the thread has not already been resumed.  */
    else if (thread_ptr -> tx_state != TX_READY)
    {

        /* Check for a delayed suspend flag.  */
        if (thread_ptr -> tx_delayed_suspend)
        {

            /* Clear the delayed suspend flag and change the state.  */
            thread_ptr -> tx_delayed_suspend =  TX_FALSE;
            thread_ptr -> tx_state =            TX_SUSPENDED;
        }
        else
        {

            /* Log the thread status change.  */
            TX_EL_THREAD_STATUS_CHANGE_INSERT(thread_ptr, TX_READY);

            /* Make this thread ready.  */

            /* Change the state to ready.  */
            thread_ptr -> tx_state =  TX_READY;

            /* Pickup priority of thread.  */
            priority =  thread_ptr -> tx_priority;

            /* Determine if there are other threads at this priority that are
               ready.  */
            head_ptr =  _tx_thread_priority_list[priority];
            if (head_ptr)
            {

                /* Yes, there are other threads at this priority already ready.  */

                /* Just add this thread to the priority list.  */
                tail_ptr =                          head_ptr -> tx_ready_previous;
                tail_ptr -> tx_ready_next =         thread_ptr;
                head_ptr -> tx_ready_previous =     thread_ptr;
                thread_ptr -> tx_ready_previous =   tail_ptr;
                thread_ptr -> tx_ready_next =       head_ptr;
            }   
            else
            {

                /* First thread at this priority ready.  Add to the front of the
                   list.  */
                _tx_thread_priority_list[priority] =    thread_ptr;
                thread_ptr -> tx_ready_next =           thread_ptr;
                thread_ptr -> tx_ready_previous =       thread_ptr;

                /* Or in the thread's priority bit.  */
                _tx_thread_priority_map =  _tx_thread_priority_map | thread_ptr -> tx_priority_bit;

                /* Check to see if this is a higher priority thread.  */
                if (_tx_thread_execute_ptr == TX_NULL)
                {

                    /* No other thread is ready.  Setup the highest priority and 
                       the execute thread pointer.  */
                    _tx_thread_execute_ptr =       thread_ptr;
                    _tx_thread_highest_priority =  priority;
                }
                else if (priority < _tx_thread_highest_priority)
                {

                    /* A new highest priority thread is present. */

                    /* Update the highest priority variable.  */
                    _tx_thread_highest_priority =  priority;

                    /* Determine if preemption is allowed.  */
                    if (priority < _tx_thread_execute_ptr -> tx_preempt_threshold)
                    {

                        /* Determine if this thread had preemption threshold set.  */
                        if (_tx_thread_execute_ptr -> tx_preempt_threshold !=
                                _tx_thread_execute_ptr -> tx_priority)
                        {

                            /* Remember that this thread was preempted by a thread
                               above the thread's threshold.  */
                            _tx_thread_preempted_map =  _tx_thread_preempted_map |
                                                _tx_thread_execute_ptr -> tx_priority_bit;
                        }

                        /* Yes, modify the execute thread pointer.  */
                        _tx_thread_execute_ptr =  thread_ptr;
                    }
                }
            }
        }
    }	

    /* Restore interrupts.  */
    TX_RESTORE

    /* Determine if preemption should take place.  */
    if ((_tx_thread_current_ptr != _tx_thread_execute_ptr) && (_tx_thread_system_state == 0))

        /* Yes, set the preemption flag.  */
        preemption =  TX_TRUE;

    /* Return preemption flag. */
    return(preemption);
}

