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
/*    _tx_timer_delete                                    PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function deletes the specified application timer.              */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    timer_ptr                         Pointer to timer control block    */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_SUCCESS                        Successful completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_deactivate              Timer deactivation function       */ 
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
UINT    _tx_timer_delete(TX_TIMER *timer_ptr)
{

TX_INTERRUPT_SAVE_AREA


    /* Determine if the timer needs to be deactivated.  */
    if (timer_ptr -> tx_timer_internal.tx_list_head)
        
        /* Yes, deactivate the timer before it is deleted.  */
        _tx_timer_deactivate(&(timer_ptr -> tx_timer_internal));

    /* Disable interrupts to remove the timer from the created list.  */
    TX_DISABLE

    /* Log this kernel call.  */
    TX_EL_TIMER_DELETE_INSERT

    /* Decrement the number of created timers.  */
    _tx_timer_created_count--;

    /* Clear the timer ID to make it invalid.  */
    timer_ptr -> tx_timer_id =  0;

    /* See if the timer is the only one on the list.  */
    if (timer_ptr == timer_ptr -> tx_timer_created_next)
    {

        /* Only created timer, just set the created list to NULL.  */
        _tx_timer_created_ptr =  TX_NULL;
    }
    else
    {

        /* Link-up the neighbors.  */
        (timer_ptr -> tx_timer_created_next) -> tx_timer_created_previous =
                                            timer_ptr -> tx_timer_created_previous;
        (timer_ptr -> tx_timer_created_previous) -> tx_timer_created_next =
                                            timer_ptr -> tx_timer_created_next;

        /* See if we have to update the created list head pointer.  */
        if (_tx_timer_created_ptr == timer_ptr)
            
            /* Yes, move the head pointer to the next link. */
            _tx_timer_created_ptr =  timer_ptr -> tx_timer_created_next; 
    }

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

