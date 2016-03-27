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
/*    _tx_timer_deactivate                                PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function deactivates, or removes the timer from the active     */ 
/*    timer expiration list.  If the timer is already deactivated, this   */ 
/*    function just returns.                                              */ 
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
/*    _tx_timer_deactivate_api          Application timer deactivate      */ 
/*    Suspension Timeouts                                                 */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  07-04-1997     William E. Lamie         Removed list head update of   */ 
/*                                            next timer unless the head  */ 
/*                                            of the timer list is        */ 
/*                                            removed, resulting in       */ 
/*                                            version 3.0a.               */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_timer_deactivate(TX_INTERNAL_TIMER *timer_ptr)
{

TX_INTERRUPT_SAVE_AREA


    /* Disable interrupts.  */
    TX_DISABLE

    /* Determine if the timer still needs deactivation.  */
    if (timer_ptr -> tx_list_head)
    {

        /* Deactivate the timer.  */

        /* See if this is the only timer in the list.  */
        if (timer_ptr == timer_ptr -> tx_active_next)
        {

            /* Yes, the only timer on the list.  */

            /* Determine if the head pointer needs to be updated.  */
            if (*(timer_ptr -> tx_list_head) == timer_ptr)
            {

                /* Update the head pointer.  */
                *(timer_ptr -> tx_list_head) =  TX_NULL;
            }

            /* Clear the timer's list head pointer.  */
            timer_ptr -> tx_list_head =  TX_NULL;
        }
        else
        {

            /* At least one more timer is on the same expiration list.  */

            /* Update the links of the adjacent timers.  */
            (timer_ptr -> tx_active_next) -> tx_active_previous =  
                                                    timer_ptr -> tx_active_previous;
            (timer_ptr -> tx_active_previous) -> tx_active_next =
                                                    timer_ptr -> tx_active_next;

            /* Determine if the head pointer needs to be updated.  */
            if (*(timer_ptr -> tx_list_head) == timer_ptr)
            {

                /* Update the next timer in the list with the list head 
                   pointer.  */
                (timer_ptr -> tx_active_next) -> tx_list_head =  timer_ptr -> tx_list_head;

                /* Update the head pointer.  */
                *(timer_ptr -> tx_list_head) =  timer_ptr -> tx_active_next;
            }

            /* Clear the timer's list head pointer.  */
            timer_ptr -> tx_list_head =  TX_NULL;
        }
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

