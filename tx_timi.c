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


/* Locate timer management component data in this file.  */

#define TX_TIMER_INIT


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_thr.h"
#include    "tx_tim.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_timer_initialize                                PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function initializes the various control data structures for   */ 
/*    the clock control component.                                        */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_thread_create                 Create the system timer thread    */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_initialize_high_level         High level initialization         */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Changed setup of the timer    */ 
/*                                            list end pointer to avoid   */ 
/*                                            strict compiler error       */ 
/*                                            detection, resulting in     */ 
/*                                            version 3.0e.               */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
VOID    _tx_timer_initialize(VOID)
{

REG_1 UINT                  i;              /* Working index variable     */ 
REG_2 TX_INTERNAL_TIMER     **timer_ptr;    /* Working timer pointer      */


    /* Initialize the system clock to 0.  */
    _tx_timer_system_clock =  0;

    /* Initialize the time-slice value to 0 to make sure it is disabled.  */
    _tx_timer_time_slice =  0;

    /* Clear the expired flags.  */
    _tx_timer_expired_time_slice =  TX_FALSE;
    _tx_timer_expired =             TX_FALSE;

    /* Initialize the thread and application timer management control structures.  */

    /* First, initialize the timer list.  */
    timer_ptr =  &_tx_timer_list[0];
    for (i = 0; i < TX_TIMER_ENTRIES; i++)
        *timer_ptr++ =  TX_NULL;

    /* Initialize all of the list pointers.  */
    _tx_timer_list_start =   &_tx_timer_list[0];
    _tx_timer_current_ptr =  &_tx_timer_list[0];

    /* Set the timer list end pointer to one past the actual timer list.  This is done
       to make the timer interrupt handling in assembly language a little easier.  */
    _tx_timer_list_end =     &_tx_timer_list[TX_TIMER_ENTRIES-1];
    _tx_timer_list_end =     _tx_timer_list_end + 1;

    /* Create the system timer thread.  This thread processes all of the timer 
       expirations and re-schedules.  Its stack and priority are defined in the
       low-level initialization component.  */
    _tx_thread_create(&_tx_timer_thread, "System Timer Thread", _tx_timer_thread_entry, 
                (ULONG) TX_TIMER_ID,  _tx_timer_stack_start, _tx_timer_stack_size, 
                _tx_timer_priority, _tx_timer_priority, TX_NO_TIME_SLICE, TX_DONT_START);

    /* Initialize the head pointer of the created application timer list.  */
    _tx_timer_created_ptr =  TX_NULL;

    /* Set the created count to zero.  */
    _tx_timer_created_count =  0;
}

