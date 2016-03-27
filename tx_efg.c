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
/**   Event Flags (EVE)                                                   */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"
#include    "tx_tim.h"
#include    "tx_eve.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_event_flags_get                                 PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function gets the specified event flags from the group,        */ 
/*    acording to the get option.  The get option also specifies whether  */ 
/*    or not the retrieved flags are cleared.                             */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    group_ptr                         Pointer to group control block    */ 
/*    requested_event_flags             Event flags requested             */ 
/*    get_option                        Specifies and/or and clear options*/ 
/*    actual_flags_ptr                  Pointer to place the actual flags */ 
/*                                        the service retrieved           */ 
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
UINT        _tx_event_flags_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested_flags,
                    UINT get_option, ULONG *actual_flags_ptr, ULONG wait_option)
{

TX_INTERRUPT_SAVE_AREA

REG_1   UINT        status;                 /* Return status           */
REG_2   TX_THREAD   *thread_ptr;            /* Working thread pointer  */


    /* Disable interrupts to examine the event flags group.  */
    TX_DISABLE

    /* Log this kernel call.  */
    TX_EL_EVENT_FLAGS_GET_INSERT

    /* Determine if the event flags are present, based on the get option.  */
    if (get_option & TX_EVENT_FLAGS_AND_MASK)
    {

        /* All flags must be present to satisfy request.  */
        if ((group_ptr -> tx_event_flags_current & requested_flags) == requested_flags)
    
            /* Yes, all the events are present.  */
            status =  TX_SUCCESS;
        else

            /* No, not all the events are present.  */
            status =  TX_NO_EVENTS;
    }
    else
    {

        /* Any of the events will satisfy the request.  */
        if (group_ptr -> tx_event_flags_current & requested_flags)

            /* Yes, one or more of the requested events are set.  */
            status =  TX_SUCCESS;
        else
    
            /* No, none of the events are currently set.  */
            status =  TX_NO_EVENTS;
    }

    /* Now determine if the request can be satisfied immediately.  */
    if (status == TX_SUCCESS)
    {

        /* Yes, this request can be handled immediately.  */

        /* Return the actual event flags that satisfied the request.  */
        *actual_flags_ptr =  group_ptr -> tx_event_flags_current;

        /* Determine whether or not clearing needs to take place.  */
        if (get_option & TX_EVENT_FLAGS_CLEAR_MASK)

            /* Yes, clear the flags that satisfied this request.  */
            group_ptr -> tx_event_flags_current =
                group_ptr -> tx_event_flags_current & ~requested_flags;
    }
    else
    {

        /* Determine if the request specifies suspension.  */
        if (wait_option)
        {

            /* Prepare for suspension of this thread.  */
            
            /* Pickup thread pointer.  */
            thread_ptr =  _tx_thread_current_ptr;

            /* Setup cleanup routine pointer.  */
            thread_ptr -> tx_suspend_cleanup =  _tx_event_flags_cleanup;

            /* Remember which event flags we are looking for.  */
            thread_ptr -> tx_suspend_info =  requested_flags;

            /* Save the get option as well.  */
            thread_ptr -> tx_suspend_option =  get_option;

            /* Save the destination for the current events.  */
            thread_ptr -> tx_additional_suspend_info =  (VOID_PTR) actual_flags_ptr;

            /* Setup cleanup information, i.e. this event flags group control
               block.  */
            thread_ptr -> tx_suspend_control_block =  (VOID_PTR) group_ptr;

            /* Setup suspension list.  */
            if (group_ptr -> tx_event_flags_suspension_list)
            {

                /* This list is not NULL, add current thread to the end. */
                thread_ptr -> tx_suspended_next =      
                        group_ptr -> tx_event_flags_suspension_list;
                thread_ptr -> tx_suspended_previous =  
                        (group_ptr -> tx_event_flags_suspension_list) -> tx_suspended_previous;
                ((group_ptr -> tx_event_flags_suspension_list) -> tx_suspended_previous) -> tx_suspended_next =  
                        thread_ptr;
                (group_ptr -> tx_event_flags_suspension_list) -> tx_suspended_previous =   thread_ptr;
            }
            else
            {

                /* No other threads are suspended.  Setup the head pointer and
                   just setup this threads pointers to itself.  */
                group_ptr -> tx_event_flags_suspension_list =  thread_ptr;
                thread_ptr -> tx_suspended_next =              thread_ptr;
                thread_ptr -> tx_suspended_previous =          thread_ptr;
            }

            /* Increment the suspended thread count.  */
            group_ptr -> tx_event_flags_suspended_count++;

            /* Set the state to suspended.  */
            thread_ptr -> tx_state =    TX_EVENT_FLAG;

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
    
            /* Immediate return, return error completion.  */
            status =  TX_NO_EVENTS;
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return completion status.  */
    return(status);
}

