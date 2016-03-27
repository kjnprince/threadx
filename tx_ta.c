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


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_timer_activate                                  PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function places the specified internal timer in the proper     */ 
/*    place in the timer expiration list.  If the timer is already active */ 
/*    this function does nothing.                                         */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    timer_ptr                         Pointer to timer control block    */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_SUCCESS                        Always returns success            */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_timer_thread_entry            Timer thread processing           */ 
/*    _tx_thread_sleep                  Thread sleep function             */ 
/*    -tx_timer_activate_api            Application timer activate        */ 
/*    Suspension Timeouts                                                 */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  07-04-1997     William E. Lamie         Corrected a problem that left */ 
/*                                            an activated timer with a   */ 
/*                                            NULL head pointer.  This    */ 
/*                                            resulted in early timer     */ 
/*                                            expirations and other       */ 
/*                                            list management problems.   */ 
/*                                            The new version is 3.0a.    */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  12-25-1997     William E. Lamie         Removed decrement of the      */ 
/*                                            timer's remaining ticks. It */ 
/*                                            is now done in the timer    */ 
/*                                            expiration processing,      */ 
/*                                            resulting in version 3.0c.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_timer_activate(TX_INTERNAL_TIMER *timer_ptr)
{

TX_INTERRUPT_SAVE_AREA

TX_INTERNAL_TIMER           **timer_list;
REG_3 UINT                  expiration_time;


    /* Disable interrupts.  */
    TX_DISABLE

    /* Determine if the timer still needs activation.  */
    if ((timer_ptr -> tx_remaining_ticks) && 
                                    (timer_ptr -> tx_list_head == TX_NULL))
    {

        /* Activate the timer.  */

        /* Calculate the amount of time remaining for the timer.  */
        if (timer_ptr -> tx_remaining_ticks > TX_TIMER_ENTRIES)
        {

            /* Set expiration time to the maximum number of entries.  */
            expiration_time =  TX_TIMER_ENTRIES - 1;
        }
        else
        {

            /* Timer value fits in the timer entries.  */

            /* Set the expiration time.  */
            expiration_time =  (UINT) timer_ptr -> tx_remaining_ticks - 1;
        }

        /* At this point, we are ready to put the timer on one of
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
            timer_ptr -> tx_active_next =                          *timer_list;
            timer_ptr -> tx_active_previous =                      (*timer_list) -> tx_active_previous;
            (timer_ptr -> tx_active_previous) -> tx_active_next =  timer_ptr;
            (*timer_list) -> tx_active_previous =                  timer_ptr;
            timer_ptr -> tx_list_head =                            timer_list;
        }
        else
        {
                
            /* This list is NULL, just put the new timer on it.  */

            /* Setup the links in this timer.  */
            timer_ptr -> tx_active_next =      timer_ptr;
            timer_ptr -> tx_active_previous =  timer_ptr;
            timer_ptr -> tx_list_head =        timer_list;

            /* Setup the list head pointer.  */
            *timer_list =  timer_ptr;
        }                
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

