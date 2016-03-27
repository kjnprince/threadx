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


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    tx_thr.h                                            PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the ThreadX thread control component, including   */ 
/*    data types and external references.  It is assumed that tx_api.h    */
/*    and tx_port.h have already been included.                           */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  03-01-1998     William E. Lamie         Added a new variable for      */ 
/*                                            nested preemption threshold */ 
/*                                            conditions, resulting in    */ 
/*                                            version 3.0d.               */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef  TX_THR
#define  TX_THR


/* Define thread control specific data definitions.  */

#define TX_THREAD_ID                    0x54485244UL
#define TX_THREAD_MAX_BYTE_VALUES       256
#define TX_THREAD_PRIORITY_MASK         0x1F
#define TX_THREAD_PRIORITY_GROUP_MASK   0xFF
#define TX_THREAD_GROUP_SIZE            8
#define TX_THREAD_GROUP_0               0
#define TX_THREAD_GROUP_1               8
#define TX_THREAD_GROUP_2               16
#define TX_THREAD_GROUP_3               24


/* Define thread control function prototypes.  */

VOID        _tx_thread_initialize(VOID);
UINT        _tx_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, 
                VOID (*entry_function)(ULONG), ULONG entry_input,
                VOID *stack_start, ULONG stack_size, 
                UINT priority, UINT preempt_threshold, 
                ULONG time_slice, UINT auto_start);
UINT        _tx_thread_delete(TX_THREAD *thread_ptr);
TX_THREAD  *_tx_thread_identify(VOID);
UINT        _tx_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold,
                        UINT *old_threshold);
UINT        _tx_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority,
                        UINT *old_priority);
VOID        _tx_thread_relinquish(VOID);
UINT        _tx_thread_resume(TX_THREAD *thread_ptr);
UINT        _tx_thread_resume_api(TX_THREAD *thread_ptr);
UINT        _tx_thread_sleep(ULONG timer_ticks);
VOID        _tx_thread_suspend(TX_THREAD *thread_ptr);
UINT        _tx_thread_suspend_api(TX_THREAD *thread_ptr);
UINT        _tx_thread_terminate(TX_THREAD *thread_ptr);
UINT        _tx_thread_time_slice_change(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice);
VOID        _tx_thread_stack_build(TX_THREAD *thread_ptr, 
                                VOID (*function_ptr)(VOID));
VOID        _tx_thread_timeout(ULONG timeout_input);
VOID        _tx_thread_system_return(VOID);
VOID        _tx_thread_shell_entry(VOID);
VOID        _tx_thread_schedule(VOID);
VOID        _tx_thread_context_save(VOID);
VOID        _tx_thread_context_restore(VOID);
VOID        _tx_thread_preempt_check(VOID);


/* Define error checking shells for API services.  These are only referenced by the 
   application.  */

UINT        _txe_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, 
                VOID (*entry_function)(ULONG), ULONG entry_input,
                VOID *stack_start, ULONG stack_size, 
                UINT priority, UINT preempt_threshold, 
                ULONG time_slice, UINT auto_start);
UINT        _txe_thread_delete(TX_THREAD *thread_ptr);
TX_THREAD  *_tx_thread_identify(VOID);
UINT        _txe_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold,
                        UINT *old_threshold);
UINT        _txe_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority,
                        UINT *old_priority);
VOID        _txe_thread_relinquish(VOID);
UINT        _txe_thread_resume_api(TX_THREAD *thread_ptr);
UINT        _txe_thread_suspend_api(TX_THREAD *thread_ptr);
UINT        _txe_thread_terminate(TX_THREAD *thread_ptr);
UINT        _txe_thread_time_slice_change(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice);


/* Thread control component data declarations follow.  */

/* Determine if the initialization function of this component is including
   this file.  If so, make the data definitions really happen.  Otherwise,
   make them extern so other functions in the component can access them.  */

#ifdef  TX_THREAD_INIT
#define THREAD_DECLARE 
#else
#define THREAD_DECLARE extern
#endif



/* Define the pointer that contains the system stack pointer.  This is
   utilized when control returns from a thread to the system to reset the
   current stack.  This is setup in the low-level initialization function. */

THREAD_DECLARE  VOID *          _tx_thread_system_stack_ptr;


/* Define the current thread pointer.  This variable points to the currently
   executing thread.  If this variable is NULL, no thread is executing.  */

THREAD_DECLARE  TX_THREAD *     _tx_thread_current_ptr;


/* Define the variable that holds the next thread to execute.  It is important
   to remember that this is not necessarily equal to the current thread 
   pointer.  */

THREAD_DECLARE  TX_THREAD *     _tx_thread_execute_ptr;


/* Define the head pointer of the created thread list.  */

THREAD_DECLARE  TX_THREAD *     _tx_thread_created_ptr;


/* Define the variable that holds the number of created threads. */

THREAD_DECLARE  ULONG           _tx_thread_created_count;


/* Define the current state variable.  When this value is 0, a thread
   is executing or the system is idle.  Other values indicate that 
   interrupt or initialization processing is active.  This variable is
   initialized to TX_INITIALIZE_IN_PROGRESS to indicate initialization is
   active.  */

#ifdef  TX_THREAD_INIT
THREAD_DECLARE  ULONG           _tx_thread_system_state = TX_INITIALIZE_IN_PROGRESS;
#else
THREAD_DECLARE  ULONG           _tx_thread_system_state;
#endif


/* Define the 32-bit priority bit-map.  If any thread at a certain priority
   is ready for execution, the corresponding bit is set.  There are 32 thread
   priorities available in ThreadX, ranging from 0 to 31.  Priority 0 is
   the highest.  */

THREAD_DECLARE  ULONG           _tx_thread_priority_map;


/* Define the 32-bit preempt priority bit map.  Each set bit corresponds to 
   preempted priority level that had preemption threshold active to protect
   against a range of relatively higher priority threads.  */

THREAD_DECLARE  ULONG           _tx_thread_preempted_map;


/* Define the variable that holds the highest priority group ready for 
   execution.  It is important to note that this is not necessarily the same
   as the priority of the thread pointed to by _tx_execute_thread.  */

THREAD_DECLARE  UINT            _tx_thread_highest_priority;


/* Define the array that contains the lowest set bit of any 8-bit pattern.  
   Portions of the tmt_priority_map are used and indexes to quickly find
   the next priority group ready for execution.  This table is initialized
   in the thread control initialization processing.  */

THREAD_DECLARE  UCHAR           _tx_thread_lowest_bit[TX_THREAD_MAX_BYTE_VALUES];


/* Define the array of thread pointers.  Each entry represents the threads that
   are ready at that priority group.  For example, index 10 in this array
   represents the first thread ready at priority 10.  If this entry is NULL,
   no threads are ready at that priority.  */

THREAD_DECLARE  TX_THREAD *     _tx_thread_priority_list[TX_MAX_PRIORITIES];


/* Define the global preempt disable variable.  If this is non-zero, preemption is
   disabled.  It is used internally by ThreadX to prevent preemption of a thread in 
   the middle of a service that is resuming or suspending another thread.  */

THREAD_DECLARE  UINT            _tx_thread_preempt_disable;


#ifdef  TX_THREAD_INIT
CHAR _tx_thread_special_string[] = 
  "G-GB-GL-M-D-DL-KML-CMR-HMR-ML2-GZ-KH2-CM-RP-TC-NH-TD-AP-HA-GF-DD-AT-MF-MS-DW-USA-CA-SD-SDSU";
#endif

#endif
