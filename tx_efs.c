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
/*    _tx_event_flags_set                                 PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function sets the specified flags in the event group based on  */ 
/*    the set option specified.  All threads suspended on the group whose */ 
/*    get request can now be satisfied are resumed.                       */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    group_ptr                         Pointer to group control block    */ 
/*    flags_to_set                      Event flags to set                */ 
/*    set_option                        Specified either AND or OR        */ 
/*                                        operation on the event flags    */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_SUCCESS                        Always returns success            */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_deactivate              Deactivate timer routine          */ 
/*    _tx_thread_resume                 Resume thread service             */ 
/*    _tx_thread_system_return          Return to system routine          */ 
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
/*  07-04-1997     William E. Lamie         Corrected a problem setting   */ 
/*                                            event flags from ISRs with  */ 
/*                                            thread preemption, resulting*/ 
/*                                            in version 3.0a             */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  03-01-1998     William E. Lamie         Corrected problem building    */ 
/*                                            the satisfied thread list   */ 
/*                                            when two or more threads    */ 
/*                                            are satisfied, resulting in */ 
/*  01-01-1999     William E. Lamie         Corrected problem setting     */ 
/*                                            events from ISRs while the  */ 
/*                                            same event flag group is    */ 
/*                                            being set from a thread,    */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            added logic the optimizes   */ 
/*                                            processing when a single    */ 
/*                                            thread is suspended on the  */ 
/*                                            event flag group, resulting */ 
/*                                            in version 3.0f.            */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_event_flags_set(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags_to_set, 
                    UINT set_option)
{

TX_INTERRUPT_SAVE_AREA

REG_1   UINT        status;                 /* Return status                 */
REG_2   TX_THREAD   *thread_ptr;            /* Working thread pointer        */
REG_3   TX_THREAD   *next_thread_ptr;       /* Next thread in satisfied list */
REG_4   TX_THREAD   *satisfied_list;        /* Event satisfied list pointer  */
REG_5   TX_THREAD   *last_satisfied;        /* Last satisfied pointer        */
TX_THREAD           *suspended_list;        /* Temporary copy of suspend list*/
ULONG               suspended_count;        /* Number of suspended threads   */ 
ULONG               current_event_flags;    /* Current event flags           */
UINT                get_option;             /* Suspended thread's get option */


    /* Disable interrupts to remove the semaphore from the created list.  */
    TX_DISABLE

    /* Log this kernel call.  */
    TX_EL_EVENT_FLAGS_SET_INSERT

    /* Determine how to set this group's event flags.  */
    if (set_option & TX_EVENT_FLAGS_AND_MASK)
    {

        /* "AND" the flags into the current events of the group.  */
        group_ptr -> tx_event_flags_current =
            group_ptr -> tx_event_flags_current & flags_to_set;
    
        /* There is no need to check for any suspended threads since no
           new bits are set.  */
        TX_RESTORE
        return(TX_SUCCESS);
    }
    else
    {

        /* "OR" the flags into the current events of the group.  */
        group_ptr -> tx_event_flags_current =
            group_ptr -> tx_event_flags_current | flags_to_set;
    }

    /* Determine if there are any threads suspended on the event flag group.  */
    if (group_ptr -> tx_event_flags_suspension_list)
    {

        /* Determine if there is just a single thread waiting on the event 
           flag group.  */
        if (group_ptr -> tx_event_flags_suspended_count == 1)
        {

            /* Single thread waiting for event flags.  Bypass the multiple thread
               logic.  */

            /* Setup thread pointer.  */
            thread_ptr =  group_ptr -> tx_event_flags_suspension_list;

            /* Determine if this thread's get event flag request has been met.  */
            if (thread_ptr -> tx_suspend_option & TX_EVENT_FLAGS_AND_MASK)
            {

                /* All flags must be present to satisfy request.  */
                if ((group_ptr -> tx_event_flags_current & thread_ptr -> tx_suspend_info) == 
                                                                thread_ptr -> tx_suspend_info)
    
                    /* Yes, all the events are present.  */
                    status =  TX_SUCCESS;
                else

                    /* No, not all the events are present.  */
                    status =  TX_NO_EVENTS;
            }
            else
            {

                /* Any of the events will satisfy the request.  */
                if (group_ptr -> tx_event_flags_current & thread_ptr -> tx_suspend_info)

                    /* Yes, one or more of the requested events are set.  */
                    status =  TX_SUCCESS;
                else
    
                    /* No, none of the events are currently set.  */
                    status =  TX_NO_EVENTS;
            }

            /* Was the suspended thread's event request satisfied?  */
            if (status == TX_SUCCESS)
            {

                /* Yes, resume the thread and apply any event flag
                   clearing.  */

                /* Return the actual event flags that satisfied the request.  */
                *((ULONG *) thread_ptr -> tx_additional_suspend_info) =  
                                                    group_ptr -> tx_event_flags_current;

                /* Determine whether or not clearing needs to take place.  */
                if (thread_ptr -> tx_suspend_option & TX_EVENT_FLAGS_CLEAR_MASK)
                {

                    /* Yes, clear the flags that satisfied this request.  */
                    group_ptr -> tx_event_flags_current =
                        group_ptr -> tx_event_flags_current & ~(thread_ptr -> tx_suspend_info);
                }

                /* Clear the suspension information in the event flag group.  */
                group_ptr -> tx_event_flags_suspension_list =  TX_NULL;
                group_ptr -> tx_event_flags_suspended_count =  0;

                /* Clear cleanup routine to avoid timeout.  */
                thread_ptr -> tx_suspend_cleanup =  TX_NULL;

                /* Temporarily disable preemption.  */
                _tx_thread_preempt_disable++;

                /* Restore interrupts.  */
                TX_RESTORE

                /* Deactivate the timeout timer if necessary.  */
                if (thread_ptr -> tx_thread_timer.tx_list_head)
                {

                    /* Deactivate the thread's timeout timer.  */
                    _tx_timer_deactivate(&(thread_ptr -> tx_thread_timer));
                }
                else
                {

                    /* Clear the remaining time to ensure timer doesn't get activated.  */
                    thread_ptr -> tx_thread_timer.tx_remaining_ticks =  0;
                }

                /* Put return status into the thread control block.  */
                thread_ptr -> tx_suspend_status =  TX_SUCCESS;        

                /* Resume thread.  */
                if (_tx_thread_resume(thread_ptr))

                    /* Preemption is required, transfer control back to 
                       system.  */
                    _tx_thread_system_return();

                /* Return successful status.  */
                return(TX_SUCCESS);    
            }
            else
            {

                /* Flags of single suspended thread were not satisfied.  */
                TX_RESTORE

                /* Return successful status.  */
                return(TX_SUCCESS);
            }
        }

        /* Otherwise, the event flag requests of multiple threads must be 
           examined.  */

        /* Setup thread pointer, keep a local copy of the head pointer.  */
        suspended_list =  group_ptr -> tx_event_flags_suspension_list;
        thread_ptr =      suspended_list;

        /* Clear the suspended list head pointer to thwart manipulation of
           the list in ISR's while we are processing here.  */
        group_ptr -> tx_event_flags_suspension_list =  TX_NULL;
        
        /* Setup the satisfied thread pointers.  */
        satisfied_list =  TX_NULL;
        last_satisfied =  TX_NULL;

        /* Setup the suspended count.  */      
        suspended_count =  group_ptr -> tx_event_flags_suspended_count;

        /* Pickup the current event flags.  */
        current_event_flags =  group_ptr -> tx_event_flags_current;

        /* Disable preemption while we process the suspended list.  */
        _tx_thread_preempt_disable++;

        /* Loop to examine all of the suspended threads. */
        do 
        {

            /* Restore interrupts temporarily.  */
            TX_RESTORE

            /* Disable interrupts again.  */
            TX_DISABLE

            /* Determine if we need to reset the search.  */
            if (group_ptr -> tx_event_flags_reset_search)
            {

                /* Clear the reset search flag.  */
                group_ptr -> tx_event_flags_reset_search =  0;

                /* Move the thread pointer to the beginning of the search list.  */
                thread_ptr =  suspended_list;

                /* Reset the suspended count.  */      
                suspended_count =  group_ptr -> tx_event_flags_suspended_count;

				/* Update the current events with any new ones that might
				   have been set in a nested set events call from an ISR.  */
				current_event_flags =  current_event_flags | group_ptr -> tx_event_flags_current;

                /* Determine if we need to get out of this loop.  */
                if (!thread_ptr)
                    break;
            }

            /* Pickup this thread's suspension get option.  */
            get_option =  thread_ptr -> tx_suspend_option;

            /* Determine if this thread's get event flag request has been met.  */
            if (get_option & TX_EVENT_FLAGS_AND_MASK)
            {

                /* All flags must be present to satisfy request.  */
                if ((current_event_flags & thread_ptr -> tx_suspend_info) == 
                                                thread_ptr -> tx_suspend_info)
    
                    /* Yes, all the events are present.  */
                    status =  TX_SUCCESS;
                else

                    /* No, not all the events are present.  */
                    status =  TX_NO_EVENTS;
            }
            else
            {

                /* Any of the events will satisfy the request.  */
                if (current_event_flags & thread_ptr -> tx_suspend_info)

                    /* Yes, one or more of the requested events are set.  */
                    status =  TX_SUCCESS;
                else
    
                    /* No, none of the events are currently set.  */
                    status =  TX_NO_EVENTS;
            }
                
            /* Save next thread pointer.  */
            next_thread_ptr =  thread_ptr -> tx_suspended_next;

            /* Now determine if the request can be satisfied immediately.  */
            if (status == TX_SUCCESS)
            {

                /* Yes, this request can be handled now.  */

                /* Return the actual event flags that satisfied the request.  */
                *((ULONG *) thread_ptr -> tx_additional_suspend_info) =  current_event_flags;

                /* Determine whether or not clearing needs to take place.  */
                if (get_option & TX_EVENT_FLAGS_CLEAR_MASK)

                    /* Yes, clear the flags that satisfied this request.  */
                    group_ptr -> tx_event_flags_current =
                        group_ptr -> tx_event_flags_current & ~(thread_ptr -> tx_suspend_info);
            
                /* We need to remove the thread from the suspension list and place it in the
                   expired list.  */

               /* See if this is the only suspended thread on the list.  */
                if (thread_ptr == thread_ptr -> tx_suspended_next)
                {

                    /* Yes, the only suspended thread.  */

                    /* Update the head pointer.  */
                    suspended_list =  TX_NULL;
                }
                else
                {

                    /* At least one more thread is on the same expiration list.  */

                    /* Update the list head pointer, if removing the head of the
                       list.  */
                    if (suspended_list == thread_ptr)
                        suspended_list =  thread_ptr -> tx_suspended_next;

                    /* Update the links of the adjacent threads.  */
                    (thread_ptr -> tx_suspended_next) -> tx_suspended_previous =  
                                                    thread_ptr -> tx_suspended_previous;
                    (thread_ptr -> tx_suspended_previous) -> tx_suspended_next =
                                                    thread_ptr -> tx_suspended_next;
                } 
                
                /* Decrement the suspension count.  */
                group_ptr -> tx_event_flags_suspended_count--;

                /* Prepare for resumption of the first thread.  */

                /* Clear cleanup routine to avoid timeout.  */
                thread_ptr -> tx_suspend_cleanup =  TX_NULL;

                /* Put return status into the thread control block.  */
                thread_ptr -> tx_suspend_status =  TX_SUCCESS;        

                /* Place this thread on the expired list.  */
                if (!satisfied_list)
                {

                    /* First thread on the satisfied list.  */
                    satisfied_list =  thread_ptr;
                    last_satisfied =  thread_ptr;
    
                    /* Setup intial next pointer.  */
                    thread_ptr -> tx_suspended_next =  TX_NULL;
                }
                else
                {

                    /* Not the first thread on the satisfied list.  */
                
                    /* Link it up at the end.  */
                    last_satisfied -> tx_suspended_next =  thread_ptr;
                    thread_ptr -> tx_suspended_next =      TX_NULL;
                    last_satisfied =                       thread_ptr;
                }
            }

            /* Copy next thread pointer to working thread ptr.  */
            thread_ptr =  next_thread_ptr;

        } while (--suspended_count);

        /* Setup the group's suspension list head again.  */
        group_ptr -> tx_event_flags_suspension_list =  suspended_list;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Walk through the satisfied list, setup initial thread pointer. */
        thread_ptr =  satisfied_list;
        while(thread_ptr)
        {
    
            /* Get next pointer first.  */
            next_thread_ptr =  thread_ptr -> tx_suspended_next;

            /* Deactivate the timeout timer if necessary.  */
            if (thread_ptr -> tx_thread_timer.tx_list_head)
            {

                /* Deactivate the thread's timeout timer.  */
                _tx_timer_deactivate(&(thread_ptr -> tx_thread_timer));
            }
            else
            {

                /* Clear the remaining time to ensure timer doesn't get activated.  */
                thread_ptr -> tx_thread_timer.tx_remaining_ticks =  0;
            }

            /* Disable interrupts.  */
            TX_DISABLE

            /* Disable preemption again.  */
            _tx_thread_preempt_disable++;

            /* Restore interrupt posture.  */
            TX_RESTORE

            /* Resume the thread.  */
            _tx_thread_resume(thread_ptr);

            /* Move next thread to current.  */
            thread_ptr =  next_thread_ptr;
        }

        /* Disable interrupts.  */
        TX_DISABLE

        /* Release thread preemption disable.  */
        _tx_thread_preempt_disable--;

        /* Restore interrupts.  */
        TX_RESTORE

        /* Now determine if preemption is required.  */
		if ((_tx_thread_current_ptr != _tx_thread_execute_ptr) && (_tx_thread_system_state == 0))
		{

            /* Preemption is required, transfer control back to 
               system.  */
            _tx_thread_system_return();
        }
    }
    else
    {

        /* Determine if we need to set the reset search field.  */
        if (group_ptr -> tx_event_flags_suspended_count)
            
            /* We interrupted a search of an event flag group suspension
               list.  Make sure we reset the search.  */
            group_ptr -> tx_event_flags_reset_search++;

        /* Restore interrupts.  */
        TX_RESTORE
    }

    /* Return completion status.  */
    return(TX_SUCCESS);
}

