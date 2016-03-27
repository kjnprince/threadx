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

#ifndef TX_DISABLE_STACK_CHECKING
#include    "string.h"
#endif


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_create                                   PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function creates a thread and places it on the list of created */ 
/*    threads.                                                            */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Thread control block pointer  */ 
/*    name                                  Pointer to thread name string */ 
/*    entry_function                        Entry function of the thread  */ 
/*    entry_input                           32-bit input value to thread  */ 
/*    stack_start                           Pointer to start of stack     */ 
/*    stack_size                            Stack size in bytes           */ 
/*    priority                              Priority of thread (0-31)     */ 
/*    preempt_threshold                     Preemption threshold          */
/*    time_slice                            Thread time-slice value       */ 
/*    auto_start                            Automatic start selection     */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    return status                         Thread create return status   */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_thread_stack_build                Build initial thread stack    */ 
/*    _tx_thread_resume                     Resume automatic start thread */ 
/*    _tx_thread_system_return              Return to system on preemption*/ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    Application Code                                                    */ 
/*    _tx_timer_initialize                  Create system timer thread    */ 
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
/*                                            added stack fill logic, and */ 
/*                                            fixed bug in thread timer   */ 
/*                                            logic by clearing the       */ 
/*                                            re-initialize ticks field,  */ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_thread_create(TX_THREAD *thread_ptr, CHAR *name, 
			    VOID (*entry_function)(ULONG), ULONG entry_input,
			    VOID *stack_start, ULONG stack_size, 
			    UINT priority, UINT preempt_threshold,
			    ULONG time_slice, UINT auto_start)
{

TX_INTERRUPT_SAVE_AREA
REG_1 TX_THREAD_PTR	    tail_ptr;	    /* Created list tail pointer  */


#ifndef TX_DISABLE_STACK_CHECKING

    /* Set the thread stack to a pattern prior to creating the initial
       stack frame.  This pattern is used by the stack checking routines
       to see how much has been used.  */
    memset(stack_start, TX_STACK_FILL, stack_size);

#endif

    /* Prepare the thread control block prior to placing it on the created
       list.  */

    /* Place the supplied parameters into the thread's control block.  */
    thread_ptr -> tx_thread_name =      name;
    thread_ptr -> tx_thread_entry =     entry_function;
    thread_ptr -> tx_entry_parameter =  entry_input;
    thread_ptr -> tx_stack_ptr =        TX_NULL;
    thread_ptr -> tx_stack_start =      stack_start;
    thread_ptr -> tx_stack_size =       stack_size;
    thread_ptr -> tx_stack_end =        (VOID_PTR) (((CHAR_PTR) stack_start) + (stack_size-1));
    thread_ptr -> tx_priority =         priority & TX_THREAD_PRIORITY_MASK;
    thread_ptr -> tx_preempt_threshold= preempt_threshold & TX_THREAD_PRIORITY_MASK;
    thread_ptr -> tx_time_slice =       time_slice;
    thread_ptr -> tx_new_time_slice =   time_slice;

    /* Now fill in the values that are required for thread initialization.  */
    thread_ptr -> tx_run_count =        0;
    thread_ptr -> tx_state =            TX_SUSPENDED;
    thread_ptr -> tx_delayed_suspend =  TX_FALSE;
    thread_ptr -> tx_suspending =       TX_FALSE;

    /* Setup the necessary fields in the thread timer block.  */
    thread_ptr -> tx_thread_timer.tx_timeout_function =     _tx_thread_timeout;
    thread_ptr -> tx_thread_timer.tx_timeout_param =        (ULONG) thread_ptr;
    thread_ptr -> tx_thread_timer.tx_list_head =            TX_NULL;
    thread_ptr -> tx_thread_timer.tx_re_initialize_ticks =  0;

    /* Clear the suspension information.  */
    thread_ptr -> tx_suspend_cleanup =        TX_NULL;
    thread_ptr -> tx_suspend_control_block =  TX_NULL;
    thread_ptr -> tx_suspended_next =         TX_NULL;
    thread_ptr -> tx_suspended_previous=      TX_NULL;

    /* Setup the priority bit.  */
    thread_ptr -> tx_priority_bit =  (((ULONG) 1) << (priority & TX_THREAD_PRIORITY_MASK));

    /* Call the target specific stack frame building routine to build the 
       thread's initial stack and to setup the actual stack pointer in the
       control block.  */
    _tx_thread_stack_build(thread_ptr, _tx_thread_shell_entry);

    /* Prepare to make this thread a member of the created thread list.  */
    TX_DISABLE

    /* Load the thread ID field in the thread control block.  */
    thread_ptr -> tx_thread_id =  TX_THREAD_ID;

    /* Place the thread on the list of created threads.  First,
       check for an empty list.  */
    if (_tx_thread_created_ptr)
    {

        /* Pickup tail pointer.  */
        tail_ptr =  _tx_thread_created_ptr -> tx_created_previous;

        /* Place the new thread in the list.  */
        _tx_thread_created_ptr -> tx_created_previous =  thread_ptr;
	    tail_ptr -> tx_created_next =  thread_ptr;

        /* Setup this thread's created links.  */
        thread_ptr -> tx_created_previous =  tail_ptr;
        thread_ptr -> tx_created_next =      _tx_thread_created_ptr;	
    }
    else
    {

        /* The created thread list is empty.  Add thread to empty list.  */
        _tx_thread_created_ptr =            thread_ptr;
        thread_ptr -> tx_created_next =     thread_ptr;
        thread_ptr -> tx_created_previous = thread_ptr;
    }

    /* Increment the created thread counter.  */
    _tx_thread_created_count++;

    /* Temporarily disable preemption.  */
    _tx_thread_preempt_disable++;

    /* Register thread in the thread array structure.  */
    TX_EL_THREAD_REGISTER(thread_ptr);

    /* Log this kernel call.  */
    TX_EL_THREAD_CREATE_INSERT

    /* Green Hills specific code.  */
    {
        #pragma weak __cpp_exception_init
        extern void __cpp_exception_init(void **);
        static void (*const cpp_init_funcp)(void **) = __cpp_exception_init;
        if (cpp_init_funcp) 
            __cpp_exception_init(&(thread_ptr->tx_eh_globals));
    }
    /* End of Green Hills specific code.  */

    /* Restore previous interrupt posture.  */
    TX_RESTORE

    /* Determine if an automatic start was requested.  If so, call the resume
       thread function and then check for a preemption condition.  */
    if (auto_start)
    {

        /* Call the resume thread function to make this thread ready.  Upon 
           return, check for a preemption condition caused by creating
           a thread of higher priority.  */
        if (_tx_thread_resume(thread_ptr))

            /* A preemption condition exists, pass control back to the
               system in order to get the higher priority thread running.  */
            _tx_thread_system_return();
    }
    else
    {

        /* Disable interrupts.  */
        TX_DISABLE

        /* Remove temporary disable preemption.  */
        _tx_thread_preempt_disable--;

        /* Restore interrupts.  */
        TX_RESTORE
    }

    /* Always return a success.  */
    return(TX_SUCCESS);
}

void _tx_bring_in_ghs_support(void)
{
    /* This routine is never called, it is just there to make
       sure the stack checking services are brought in by the
       library.  */
    _txs_thread_stack_check(TX_NULL);
}

