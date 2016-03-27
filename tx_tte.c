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
/**   Timer Management (TIM)                                              */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_tim.h"
#include    "tx_thr.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_timer_thread_entry                              PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function manages thread and application timer expirations.     */ 
/*    Actually, from this thread's point of view, there is no difference. */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    timer_thread_input                Used just for verification        */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    Timer Expiration Function                                           */ 
/*    _tx_thread_suspend                Thread suspension                 */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    ThreadX Scheduler                                                   */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  12-25-1997     William E. Lamie         Added logic to enable timer   */ 
/*                                            deactivation inside of the  */ 
/*                                            expiration routine and      */ 
/*                                            update of the remaining     */ 
/*                                            ticks is now done here,     */ 
/*                                            resulting in version 3.0c.  */
/*  03-01-1998     William E. Lamie         Moved the re-activate timer   */ 
/*                                            logic into this function    */ 
/*                                            to eliminate the additional */ 
/*                                            activate function call,     */ 
/*                                            resulting in version 3.0d.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
VOID    _tx_timer_thread_entry(ULONG timer_thread_input)
{

TX_INTERRUPT_SAVE_AREA

TX_INTERNAL_TIMER           *expired_timers;            /* Head of expired timers list  */
TX_INTERNAL_TIMER           *reactivate_timer;          /* Dummy list head pointer      */
TX_INTERNAL_TIMER           **timer_list;               /* Timer list pointer           */
REG_2  TX_INTERNAL_TIMER    *current_timer;             /* Current timer pointer        */ 
REG_3 UINT                  expiration_time;            /* Value used for pointer offset*/
VOID                        (*timeout_function)(ULONG); /* Local timeout function ptr   */
ULONG                       timeout_param;              /* Local timeout parameter      */


    /* Make sure the timer input is correct.  This also gets rid of the
       silly compiler warnings.  */
    if (timer_thread_input != TX_TIMER_ID)
        return;

    /* Set the reactivate_timer to NULL.  */
    reactivate_timer =  TX_NULL;

    /* Now go into an infinite loop to process timer expirations.  */
    do
    {

        /* First, move the current list pointer and clear the timer 
           expired value.  This allows the interrupt handling portion
           to continue looking for timer expirations.  */
        TX_DISABLE

        /* Save the current timer expiration list pointer.  */
        expired_timers =  *_tx_timer_current_ptr;

        /* Modify the head pointer in the first timer in the list, if there
           is one!  */
        if (expired_timers)
            expired_timers -> tx_list_head =  &expired_timers;

        /* Set the current list pointer to NULL.  */
        *_tx_timer_current_ptr =  TX_NULL;

        /* Move the current pointer up one timer entry wrap if we get to 
           the end of the list.  */
        _tx_timer_current_ptr++;
        if (_tx_timer_current_ptr == _tx_timer_list_end)
            _tx_timer_current_ptr =  _tx_timer_list_start;

        /* Clear the expired flag.  */
        _tx_timer_expired =  TX_FALSE;

        /* Now, restore previous interrupt posture.  */
        TX_RESTORE
        

        /* Next, process the expiration of the associated timers at this
           time slot.  */
        TX_DISABLE
        while (expired_timers)
        {

            /* Something is on the list.  Remove it and process the expiration.  */
            current_timer =  expired_timers;
            
            /* Determine if this is the only timer.  */
            if (current_timer == expired_timers -> tx_active_next)
            {

                /* Yes, this is the only timer in the list.  */

                /* Set the head pointer to NULL.  */
                expired_timers =  TX_NULL;
            }
            else
            {

                /* No, not the only expired timer.  */
            
                /* Remove this timer from the expired list.  */
                (current_timer -> tx_active_next) -> tx_active_previous =  
                                                    current_timer -> tx_active_previous;
                (current_timer -> tx_active_previous) -> tx_active_next =
                                                    current_timer -> tx_active_next;

                /* Modify the next timer's list head to point at the current list head.  */
                (current_timer -> tx_active_next) -> tx_list_head =  &expired_timers;

                /* Set the list head pointer.  */
                expired_timers =  current_timer -> tx_active_next;
            }

            /* In any case, the timer is now off of the expired list.  */

            /* Determine if the timer has expired or if it is just a really 
               big timer that needs to be placed in the list again.  */
            if (current_timer -> tx_remaining_ticks > TX_TIMER_ENTRIES)
            {

                /* Timer is bigger than the timer entries and must be
                   re-scheduled.  */

                /* Decrement the remaining ticks of the timer.  */
                current_timer -> tx_remaining_ticks =  
                        current_timer -> tx_remaining_ticks - TX_TIMER_ENTRIES;
                
                /* Set the timeout function to NULL in order to bypass the
                   expiration.  */
                timeout_function =  TX_NULL;

                /* Make the timer appear that it is still active while interrupts
                   are enabled.  This will permit proper processing of a timer
                   deactivate from an ISR.  */
                current_timer -> tx_list_head =    &reactivate_timer;
                current_timer -> tx_active_next =  current_timer;
            }
            else
            {

                /* Timer did expire.  Copy the calling function and ID 
                   into local variables before interrupts are re-enabled.  */
                timeout_function =  current_timer -> tx_timeout_function;
                timeout_param =     current_timer -> tx_timeout_param;

                /* Copy the re-initialize ticks into the remaining ticks.  */
                current_timer -> tx_remaining_ticks =  current_timer -> tx_re_initialize_ticks;

                /* Determine if the timer should be re-activated.  */
                if (current_timer -> tx_remaining_ticks)
                {

                    /* Make the timer appear that it is still active while processing
                       the expiration routine and with interrupts enabled.  This will 
                       permit proper processing of a timer deactivate from both the
                       expiration routine and an ISR.  */
                    current_timer -> tx_list_head =    &reactivate_timer;
                    current_timer -> tx_active_next =  current_timer;
                }
                else
                {

                    /* Set the list pointer of this timer to NULL.  This is used to indicate
                       the timer is no longer active.  */
                    current_timer -> tx_list_head =  TX_NULL;
                }
            }

            /* Restore interrupts for timer expiration call.  */
            TX_RESTORE

            /* Call the timer-expiration function, if non-NULL.  */
            if (timeout_function)
                (timeout_function) (timeout_param);

            /* Lockout interrupts again.  */
            TX_DISABLE

            /* Determine if the timer needs to be re-activated.  */
            if (current_timer -> tx_list_head == &reactivate_timer)
            {

                /* Re-activate the timer.  */

                /* Calculate the amount of time remaining for the timer.  */
                if (current_timer -> tx_remaining_ticks > TX_TIMER_ENTRIES)
                {

                    /* Set expiration time to the maximum number of entries.  */
                    expiration_time =  TX_TIMER_ENTRIES - 1;
                }
                else
                {

                    /* Timer value fits in the timer entries.  */

                    /* Set the expiration time.  */
                    expiration_time =  (UINT) current_timer -> tx_remaining_ticks - 1;
                }

                /* At this point, we are ready to put the timer back on one of
                   the timer lists.  */
    
                /* Calculate the proper place for the timer.  */
                timer_list =  _tx_timer_current_ptr + expiration_time;
                if (timer_list >= _tx_timer_list_end)
                {

                    /* Wrap from the beginning of the list.  */
                    timer_list =  _tx_timer_list_start +
                                        (timer_list - _tx_timer_list_end);
                }

                /* Now put the timer on this list.  */
                if (*timer_list)
                {

                    /* This list is not NULL, add current timer to the end. */
                    current_timer -> tx_active_next =                          *timer_list;
                    current_timer -> tx_active_previous =                      (*timer_list) -> tx_active_previous;
                    (current_timer -> tx_active_previous) -> tx_active_next =  current_timer;
                    (*timer_list) -> tx_active_previous =                      current_timer;
                    current_timer -> tx_list_head =                            timer_list;
                }
                else
                {
                
                    /* This list is NULL, just put the new timer on it.  */

                    /* Setup the links in this timer.  */
                    current_timer -> tx_active_next =      current_timer;
                    current_timer -> tx_active_previous =  current_timer;
                    current_timer -> tx_list_head =        timer_list;

                    /* Setup the list head pointer.  */
                    *timer_list =  current_timer;
                }
            }

            /* Restore interrupts.  */
            TX_RESTORE

            /* Lockout interrupts again.  */
            TX_DISABLE
        }

        /* Finally, suspend this thread and wait for the next expiration.  */

        /* Determine if another expiration took place while we were in this
           thread.  If so, process another expiration.  */
        if (!_tx_timer_expired)
        {

            /* Otherwise, no timer expiration, so suspend the thread.  */

            /* Set the status to suspending, in order to indicate the 
               suspension is in progress.  */
            _tx_thread_current_ptr -> tx_state =  TX_SUSPENDED;

            /* Set the suspending flag. */
            _tx_thread_current_ptr -> tx_suspending =  TX_TRUE;

            /* Increment the preempt disable count prior to suspending.  */
            _tx_thread_preempt_disable++;

            /* Restore interrupts.  */
            TX_RESTORE

            /* Call actual thread suspension routine.  */
            _tx_thread_suspend(_tx_thread_current_ptr);
        }
        else
        {

            /* Restore interrupts.  */
            TX_RESTORE
        }

    } while (TX_FOREVER);
}

