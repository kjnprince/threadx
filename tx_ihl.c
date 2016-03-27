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
/*						                                                  */
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
/**   Initialization (INI)                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_ini.h"
#include    "tx_thr.h"
#include    "tx_tim.h"
#include    "tx_sem.h"
#include    "tx_que.h"
#include    "tx_eve.h"
#include    "tx_blo.h"
#include    "tx_byt.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_initialize_high_level                           PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function is responsible for initializing all of the other      */ 
/*    components in the ThreadX real-time kernel.                         */
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
/*    _tx_thread_initialize             Initialize the thread control     */ 
/*                                        component                       */ 
/*    _tx_timer_initialize              Initialize the timer control      */ 
/*                                        component                       */ 
/*    _tx_semaphore_initialize          Initialize the semaphore control  */ 
/*                                        component                       */ 
/*    _tx_queue_initialize              Initialize the queue control      */ 
/*                                        component                       */ 
/*    _tx_event_flags_initialize        Initialize the event flags control*/ 
/*                                        component                       */ 
/*    _tx_block_pool_initialize         Initialize the block pool control */ 
/*                                        component                       */ 
/*    _tx_byte_pool_initialize          Initialize the byte pool control  */ 
/*                                        component                       */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_initialize_kernel_enter       Kernel entry function             */ 
/*    _tx_initialize_kernel_setup       Early kernel setup fuction that   */ 
/*                                        is optionally called by         */ 
/*                                        compiler's startup code.        */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Moved code to set the system  */ 
/*                                            state and call the user's   */ 
/*                                            application define function */ 
/*                                            to kernel enter function,   */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s), and      */ 
/*                                            added logic to initialize   */ 
/*                                            event logging, resulting in */ 
/*                                            version 3.0f.               */ 
/*                                                                        */ 
/**************************************************************************/ 
VOID    _tx_initialize_high_level(VOID)
{

    /* Initialize the event log, if enabled.  */
    TX_EL_INITIALIZE

    /* Call the thread control initialization function.  */
    _tx_thread_initialize();

    /* Call the timer control initialization function.  */
    _tx_timer_initialize();

    /* Call the semaphore initialization function.  */
    _tx_semaphore_initialize();

    /* Call the queue initialization function.  */
    _tx_queue_initialize();

    /* Call the event flag initialization function.  */
    _tx_event_flags_initialize();

    /* Call the block pool initialization function.  */
    _tx_block_pool_initialize();

    /* Call the byte pool initialization function.  */
    _tx_byte_pool_initialize();
}

