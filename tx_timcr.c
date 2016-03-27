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
/*    _tx_timer_create                                    PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function creates an application timer from the specified       */ 
/*    input.                                                              */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    timer_ptr                         Pointer to timer control block    */ 
/*    name_ptr                          Pointer to timer name             */ 
/*    expiration_function               Application expiration function   */ 
/*    initial_ticks                     Initial expiration ticks          */ 
/*    reschedule_ticks                  Reschedule ticks                  */ 
/*    auto_activate                     Automatic activation flag         */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_SUCCESS                        Successful completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_timer_activate                Timer activation function         */ 
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
UINT    _tx_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr, 
            VOID (*expiration_function)(ULONG), ULONG expiration_input,
            ULONG initial_ticks, ULONG reschedule_ticks, UINT auto_activate)
{

TX_INTERRUPT_SAVE_AREA

TX_TIMER        *tail_ptr;                  /* Working timer pointer      */


    /* Setup the basic timer fields.  */
    timer_ptr -> tx_timer_name =                            name_ptr;
    timer_ptr -> tx_timer_internal.tx_remaining_ticks =     initial_ticks;
    timer_ptr -> tx_timer_internal.tx_re_initialize_ticks = reschedule_ticks;
    timer_ptr -> tx_timer_internal.tx_timeout_function =    expiration_function;
    timer_ptr -> tx_timer_internal.tx_timeout_param =       expiration_input;
    timer_ptr -> tx_timer_internal.tx_list_head =           TX_NULL;
    
    /* Disable interrupts to put the timer on the created list.  */
    TX_DISABLE

    /* Setup the timer ID to make it valid.  */
    timer_ptr -> tx_timer_id =  TX_TIMER_ID;

    /* Place the timer on the list of created application timers.  First,
       check for an empty list.  */
    if (_tx_timer_created_ptr)
    {

        /* Pickup tail pointer.  */
        tail_ptr =  _tx_timer_created_ptr -> tx_timer_created_previous;

        /* Place the new timer in the list.  */
        _tx_timer_created_ptr -> tx_timer_created_previous =  timer_ptr;
	    tail_ptr -> tx_timer_created_next =                   timer_ptr;

        /* Setup this timer's created links.  */
        timer_ptr -> tx_timer_created_previous =  tail_ptr;
        timer_ptr -> tx_timer_created_next =      _tx_timer_created_ptr;	
    }
    else
    {

        /* The created timer list is empty.  Add timer to empty list.  */
        _tx_timer_created_ptr =                   timer_ptr;
        timer_ptr -> tx_timer_created_next =      timer_ptr;
        timer_ptr -> tx_timer_created_previous =  timer_ptr;
    }

    /* Increment the number of created timers.  */
    _tx_timer_created_count++;

    /* Log this kernel call.  */
    TX_EL_TIMER_CREATE_INSERT

    /* Restore interrupts.  */
    TX_RESTORE

    /* Determine if this timer needs to be activated.  */
    if (auto_activate)

        /* Call actual activation function.  */
        _tx_timer_activate(&(timer_ptr -> tx_timer_internal));

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

