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
/**   Application Interface (API)                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  APPLICATION INTERFACE DEFINITION                       RELEASE        */ 
/*                                                                        */ 
/*    tx_api.h                                            PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the basic Application Interface (API) to the      */ 
/*    high-performance ThreadX real-time kernel.  All service prototypes  */ 
/*    and data structure definitions are defined in this file.  	      */ 
/*    Please note that basic data type definitions and other architecture-*/ 
/*    specific information is contained in the file tx_port.h.            */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-31-1996     William E. Lamie         Initial Version 3.0           */ 
/*  07-04-1997     William E. Lamie         Added port-specific extension */ 
/*                                            define to thread control    */ 
/*                                            block for port-specific     */ 
/*                                            information, resulting in   */ 
/*                                            version 3.0a.               */ 
/*  11-11-1997     William E. Lamie         Modified several comments     */ 
/*                                            and added return value for  */ 
/*                                            lifting delayed suspend     */ 
/*                                            condition by the thread     */ 
/*                                            resume service, resulting   */ 
/*                                            in version 3.0b.            */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s), added    */ 
/*                                            logic to bring in the event */ 
/*                                            logging include file, and   */ 
/*                                            added a member to thread    */ 
/*                                            control block for compiler  */ 
/*                                            usage, resulting in         */ 
/*                                            version 3.0f.               */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef  TX_API
#define  TX_API

/* Determine if a C++ compiler is being used.  If so, ensure that standard
   C is used to process the API information.  */

#ifdef   __cplusplus

/* Yes, C++ compiler is present.  Use standard C.  */
extern   "C" {

#endif


/* Include the port-specific data type file.  */
#include "tx_port.h"


/* Define basic constants for the ThreadX kernel.  */


/* API input parameters and general constants.  */

#define TX_NO_WAIT          0
#define TX_WAIT_FOREVER     0xFFFFFFFFUL
#define TX_AND              2
#define TX_AND_CLEAR        3
#define TX_OR               0
#define TX_OR_CLEAR         1
#define TX_1_ULONG          1
#define TX_2_ULONG          2
#define TX_4_ULONG          4
#define TX_8_ULONG          8
#define TX_16_ULONG         16
#define TX_NO_TIME_SLICE    0
#define TX_MAX_PRIORITIES   32
#define TX_AUTO_START       1
#define TX_DONT_START       0
#define TX_AUTO_ACTIVATE    1
#define TX_NO_ACTIVATE      0
#define TX_TRUE             1
#define TX_FALSE            0
#define TX_NULL             0
#define TX_FOREVER          1


/* Thread execution state values.  */

#define TX_READY            0
#define TX_COMPLETED        1
#define TX_TERMINATED       2
#define TX_SUSPENDED        3   
#define TX_SLEEP            4
#define TX_QUEUE_SUSP       5
#define TX_SEMAPHORE_SUSP   6
#define TX_EVENT_FLAG       7
#define TX_BLOCK_MEMORY     8
#define TX_BYTE_MEMORY      9
#define TX_IO_DRIVER        10
#define TX_FILE             11
#define TX_TCP_IP           12


/* API return values.  */

#define TX_SUCCESS          0x00
#define TX_DELETED          0x01
#define TX_NO_MEMORY        0x10
#define TX_POOL_ERROR       0x02
#define TX_PTR_ERROR        0x03
#define TX_WAIT_ERROR       0x04
#define TX_SIZE_ERROR       0x05
#define TX_GROUP_ERROR      0x06
#define TX_NO_EVENTS        0x07
#define TX_OPTION_ERROR     0x08
#define TX_QUEUE_ERROR      0x09
#define TX_QUEUE_EMPTY      0x0A
#define TX_QUEUE_FULL       0x0B
#define TX_SEMAPHORE_ERROR  0x0C
#define TX_NO_INSTANCE      0x0D
#define TX_THREAD_ERROR     0x0E
#define TX_PRIORITY_ERROR   0x0F
#define TX_START_ERROR      0x10
#define TX_DELETE_ERROR     0x11
#define TX_RESUME_ERROR     0x12
#define TX_CALLER_ERROR     0x13
#define TX_SUSPEND_ERROR    0x14
#define TX_TIMER_ERROR      0x15
#define TX_TICK_ERROR       0x16
#define TX_ACTIVATE_ERROR   0x17
#define TX_THRESH_ERROR     0x18
#define TX_SUSPEND_LIFTED   0x19


/* Define the control block definitions for all system objects.  */


/* Define the basic timer management structures.  These are the structures 
   used to manage thread sleep, timeout, and user timer requests.  */

/* Define the common internal timer control block.  */

typedef  struct	TX_INTERNAL_TIMER_STRUCT
{

    /* Define the remaining ticks and re-initialization tick value.  */
    ULONG       tx_remaining_ticks;
    ULONG       tx_re_initialize_ticks;

    /* Need to define remaining ticks, type, next and previous pointers, etc.  */
    VOID        (*tx_timeout_function)(ULONG);
    ULONG       tx_timeout_param;


    /* Define the next and previous internal link pointers for active
       internal timers.  */
    struct TX_INTERNAL_TIMER_STRUCT
                *tx_active_next,
                *tx_active_previous;

    /* Keep track of the pointer to the head of this list as well.  */
    struct TX_INTERNAL_TIMER_STRUCT
                **tx_list_head;
} TX_INTERNAL_TIMER;


/* Define the timer structure utilized by the application.  */

typedef  struct TX_TIMER_STRUCT
{

    /* Define the timer ID used for error checking.  */
    ULONG       tx_timer_id;

    /* Define the timer's name.  */
    CHAR_PTR    tx_timer_name;

    /* Define the expiration routine, parameter, initial expiration, and
       reschedule expiration.  */

    /* Define the actual contents of the timer.  This is the block that
       is used in the actual timer expiration processing.  */
    TX_INTERNAL_TIMER   tx_timer_internal;

    /* Define the pointers for the created list.  */
    struct TX_TIMER_STRUCT  
                *tx_timer_created_next,
                *tx_timer_created_previous;
} TX_TIMER;

typedef TX_TIMER *      TX_TIMER_PTR;


/* ThreadX thread control block structure follows.  Additional fields
   can be added providing they are added after the information that is
   referenced in the port-specific assembly code.  */

typedef  struct TX_THREAD_STRUCT
{
    /* The first section of the control block contains critical
       information that is referenced by the port-specific 
       assembly language code.  Any changes in this section could
       necessitate changes in the assembly language.  */
    ULONG       tx_thread_id;           /* Control block ID         */
    ULONG       tx_run_count;           /* Thread's run counter     */
    VOID_PTR    tx_stack_ptr;           /* Thread's stack pointer   */
    VOID_PTR    tx_stack_start;         /* Stack starting address   */
    VOID_PTR    tx_stack_end;           /* Stack ending address     */
    ULONG       tx_stack_size;          /* Stack size               */
    ULONG       tx_time_slice;          /* Current time-slice       */
    ULONG       tx_new_time_slice;      /* New time-slice           */

    /* Define pointers to the next and previous ready threads.  */ 
    struct TX_THREAD_STRUCT 
                *tx_ready_next,      
                *tx_ready_previous;

    /* Define the port extension field.  This typically is defined 
       to white space, but some ports of ThreadX may need to have 
       additional fields in the thread control block.  This is 
       defined in the file tx_port.h.  */
    TX_THREAD_PORT_EXTENSION
  
    /***************************************************************/  
         
    /* Nothing after this point is referenced by the target-specific
       assembly language.  Hence, information after this point can 
       be added to the control block providing the complete system 
       is recompiled.  */
    CHAR_PTR    tx_thread_name;         /* Pointer to thread's name */
    UINT        tx_priority;            /* Priority of thread (0-31)*/
    UINT        tx_state;               /* Thread's execution state */
    UINT        tx_delayed_suspend;     /* Delayed suspend flag     */
    UINT        tx_suspending;          /* Thread suspending flag   */
    UINT        tx_preempt_threshold;   /* Preemption threshold     */
    ULONG       tx_priority_bit;        /* Priority ID bit          */

    /* Define the thread's entry point and input parameter.  */
    VOID        (*tx_thread_entry)(ULONG);
    ULONG       tx_entry_parameter;

    /* Define the thread's timer block.   This is used for thread 
       sleep and timeout requests.  */
    TX_INTERNAL_TIMER
                tx_thread_timer;

    /* Define the thread's cleanup function and associated data.  This
       is used to cleanup various data structures when a thread 
       suspension is lifted or terminated either by the user or 
       a timeout.  */
    VOID        (*tx_suspend_cleanup)(struct TX_THREAD_STRUCT *);
    VOID_PTR	tx_suspend_control_block;
    struct TX_THREAD_STRUCT
                *tx_suspended_next,
                *tx_suspended_previous;
    ULONG       tx_suspend_info;
    VOID_PTR    tx_additional_suspend_info;
    UINT        tx_suspend_option;
    UINT        tx_suspend_status;

    /* Define a pointer for Green Hills use.  */
    VOID_PTR    tx_eh_globals;

    /* Define pointers to the next and previous threads in the 
       created list.  */
    struct TX_THREAD_STRUCT 
                *tx_created_next,    
                *tx_created_previous;
} TX_THREAD;

typedef TX_THREAD *     TX_THREAD_PTR;


/* Define the semaphore structure utilized by the application.  */

typedef struct TX_SEMAPHORE_STRUCT
{

    /* Define the semaphore ID used for error checking.  */
    ULONG       tx_semaphore_id;

    /* Define the semaphore's name.  */
    CHAR_PTR    tx_semaphore_name;

    /* Define the actual semaphore count.  A zero means that no semaphore
       instance is available.  */
    ULONG       tx_semaphore_count;

    /* Define the semaphore suspension list head along with a count of
       how many threads are suspended.  */
    struct TX_THREAD_STRUCT  *tx_semaphore_suspension_list;
    ULONG                    tx_semaphore_suspended_count;

    /* Define the created list next and previous pointers.  */
    struct TX_SEMAPHORE_STRUCT 
                *tx_semaphore_created_next,    
                *tx_semaphore_created_previous;

} TX_SEMAPHORE;

typedef TX_SEMAPHORE *      TX_SEMAPHORE_PTR;


/* Define the queue structure utilized by the application.  */

typedef struct TX_QUEUE_STRUCT
{

    /* Define the queue ID used for error checking.  */
    ULONG       tx_queue_id;

    /* Define the queue's name.  */
    CHAR_PTR    tx_queue_name;

    /* Define the message size that was specified in queue creation.  */
    UINT        tx_queue_message_size;

    /* Define the total number of messages in the queue.  */
    ULONG       tx_queue_capacity;

    /* Define the current number of messages enqueue and the available
       queue storage space.  */
    ULONG       tx_queue_enqueued;
    ULONG       tx_queue_available_storage;

    /* Define pointers that represent the start and end for the queue's 
       message area.  */
    ULONG_PTR   tx_queue_start;
    ULONG_PTR   tx_queue_end;

    /* Define the queue read and write pointers.  Send requests use the write
       pointer while receive requests use the read pointer.  */
    ULONG_PTR   tx_queue_read;
    ULONG_PTR   tx_queue_write;

    /* Define the queue suspension list head along with a count of
       how many threads are suspended.  */
    struct TX_THREAD_STRUCT  *tx_queue_suspension_list;
    ULONG                    tx_queue_suspended_count;

    /* Define the created list next and previous pointers.  */
    struct TX_QUEUE_STRUCT 
                *tx_queue_created_next,    
                *tx_queue_created_previous;

} TX_QUEUE;

typedef TX_QUEUE *      TX_QUEUE_PTR;


/* Define the event flags group structure utilized by the application.  */

typedef struct TX_EVENT_FLAGS_GROUP_STRUCT
{

    /* Define the event flags group ID used for error checking.  */
    ULONG       tx_event_flags_id;

    /* Define the event flags group's name.  */
    CHAR_PTR    tx_event_flags_name;

    /* Define the actual current event flags in this group. A zero in a 
       particular bit indicates the event flag is not set.  */
    ULONG       tx_event_flags_current;

    /* Define the reset search flag that is set when an ISR sets flags during
       the search of the suspended threads list.  */
    UINT        tx_event_flags_reset_search;

    /* Define the event flags group suspension list head along with a count of
       how many threads are suspended.  */
    struct TX_THREAD_STRUCT  *tx_event_flags_suspension_list;
    ULONG                    tx_event_flags_suspended_count;

    /* Define the created list next and previous pointers.  */
    struct TX_EVENT_FLAGS_GROUP_STRUCT 
                *tx_event_flags_created_next,    
                *tx_event_flags_created_previous;

} TX_EVENT_FLAGS_GROUP;

typedef TX_EVENT_FLAGS_GROUP *      TX_EVENT_FLAGS_GROUP_PTR;


/* Define the block memory pool structure utilized by the application.  */

typedef struct TX_BLOCK_POOL_STRUCT
{

    /* Define the block pool ID used for error checking.  */
    ULONG       tx_block_pool_id;

    /* Define the block pool's name.  */
    CHAR_PTR    tx_block_pool_name;

    /* Define the number of available memory blocks in the pool.  */
    ULONG       tx_block_pool_available;

    /* Save the initial number of blocks.  */
    ULONG       tx_block_pool_total;

    /* Define the head pointer of the available block pool.  */
    CHAR_PTR    tx_block_pool_available_list;

    /* Save the start address of the block pool's memory area.  */
    CHAR_PTR    tx_block_pool_start;

    /* Save the block pool's size in bytes.  */
    ULONG       tx_block_pool_size;

    /* Save the individual memory block size - rounded for alignment.  */
    ULONG       tx_block_pool_block_size;

    /* Define the block pool suspension list head along with a count of
       how many threads are suspended.  */
    struct TX_THREAD_STRUCT  *tx_block_pool_suspension_list;
    ULONG                    tx_block_pool_suspended_count;

    /* Define the created list next and previous pointers.  */
    struct TX_BLOCK_POOL_STRUCT 
                *tx_block_pool_created_next,    
                *tx_block_pool_created_previous;

} TX_BLOCK_POOL;

typedef TX_BLOCK_POOL *      TX_BLOCK_POOL_PTR;


/* Define the byte memory pool structure utilized by the application.  */

typedef struct TX_BYTE_POOL_STRUCT
{

    /* Define the byte pool ID used for error checking.  */
    ULONG       tx_byte_pool_id;

    /* Define the byte pool's name.  */
    CHAR_PTR    tx_byte_pool_name;

    /* Define the number of available bytes in the pool.  */
    ULONG       tx_byte_pool_available;

    /* Define the number of fragments in the pool.  */
    ULONG       tx_byte_pool_fragments;

    /* Define the head pointer of byte pool.  */
    CHAR_PTR    tx_byte_pool_list;

    /* Define the search pointer used for initial searching for memory
       in a byte pool.  */
    CHAR_PTR    tx_byte_pool_search;

    /* Save the start address of the byte pool's memory area.  */
    CHAR_PTR    tx_byte_pool_start;

    /* Save the byte pool's size in bytes.  */
    ULONG       tx_byte_pool_size;

    /* This is used to mark the owner of the byte memory pool during
       a search.  If this value changes during the search, the local search
       pointer must be reset.  */
    struct TX_THREAD_STRUCT  *tx_byte_pool_owner;

    /* Define the byte pool suspension list head along with a count of
       how many threads are suspended.  */
    struct TX_THREAD_STRUCT  *tx_byte_pool_suspension_list;
    ULONG                    tx_byte_pool_suspended_count;

    /* Define the created list next and previous pointers.  */
    struct TX_BYTE_POOL_STRUCT 
                *tx_byte_pool_created_next,    
                *tx_byte_pool_created_previous;

} TX_BYTE_POOL;

typedef TX_BYTE_POOL *      TX_BYTE_POOL_PTR;


/* Define the system API mappings based on the error checking 
   selected by the user.  Note: this section is only applicable to 
   application source code, hence the conditional that turns off this
   stuff when the include file is processed by the ThreadX source. */

#ifndef  TX_SOURCE_CODE


/* Determine if error checking is desired.  If so, map API functions 
   to the appropriate error checking front-ends.  Otherwise, map API
   functions to the core functions that actually perform the work. 
   Note: error checking is enabled by default.  */

#ifdef TX_DISABLE_ERROR_CHECKING

/* Services without error checking.  */

#define tx_kernel_enter             _tx_initialize_kernel_enter

#define tx_byte_allocate            _tx_byte_allocate
#define tx_byte_pool_create         _tx_byte_pool_create
#define tx_byte_pool_delete         _tx_byte_pool_delete
#define tx_byte_release             _tx_byte_release

#define tx_block_allocate           _tx_block_allocate
#define tx_block_pool_create        _tx_block_pool_create
#define tx_block_pool_delete        _tx_block_pool_delete
#define tx_block_release            _tx_block_release

#define tx_event_flags_create       _tx_event_flags_create
#define tx_event_flags_delete       _tx_event_flags_delete
#define tx_event_flags_get          _tx_event_flags_get
#define tx_event_flags_set          _tx_event_flags_set

#ifdef  TX_ENABLE_EVENT_LOGGING
UINT    _tx_el_interrupt_control(UINT new_posture);
#define tx_interrupt_control    _tx_el_interrupt_control
#else
#define tx_interrupt_control    _tx_thread_interrupt_control
#endif

#define tx_queue_create             _tx_queue_create
#define tx_queue_delete             _tx_queue_delete
#define tx_queue_flush              _tx_queue_flush
#define tx_queue_receive            _tx_queue_receive
#define tx_queue_send               _tx_queue_send

#define tx_semaphore_create         _tx_semaphore_create
#define tx_semaphore_delete         _tx_semaphore_delete
#define tx_semaphore_get            _tx_semaphore_get
#define tx_semaphore_put            _tx_semaphore_put

#define tx_thread_create            _tx_thread_create
#define tx_thread_delete            _tx_thread_delete
#define tx_thread_identify          _tx_thread_identify
#define tx_thread_preemption_change _tx_thread_preemption_change
#define tx_thread_priority_change   _tx_thread_priority_change
#define tx_thread_relinquish        _tx_thread_relinquish
#define tx_thread_resume            _tx_thread_resume_api
#define tx_thread_sleep             _tx_thread_sleep
#define tx_thread_suspend           _tx_thread_suspend_api
#define tx_thread_terminate         _tx_thread_terminate
#define tx_thread_time_slice_change _tx_thread_time_slice_change

#define tx_time_get                 _tx_time_get
#define tx_time_set                 _tx_time_set
#define tx_timer_activate           _tx_timer_activate_api
#define tx_timer_change             _tx_timer_change
#define tx_timer_create             _tx_timer_create
#define tx_timer_deactivate         _tx_timer_deactivate_api
#define tx_timer_delete             _tx_timer_delete

#else

/* Services with error checking.  */

#define tx_kernel_enter             _tx_initialize_kernel_enter

#define tx_byte_allocate            _txe_byte_allocate
#define tx_byte_pool_create         _txe_byte_pool_create
#define tx_byte_pool_delete         _txe_byte_pool_delete
#define tx_byte_release             _txe_byte_release

#define tx_block_allocate           _txe_block_allocate
#define tx_block_pool_create        _txe_block_pool_create
#define tx_block_pool_delete        _txe_block_pool_delete
#define tx_block_release            _txe_block_release

#define tx_event_flags_create       _txe_event_flags_create
#define tx_event_flags_delete       _txe_event_flags_delete
#define tx_event_flags_get          _txe_event_flags_get
#define tx_event_flags_set          _txe_event_flags_set

#ifdef  TX_ENABLE_EVENT_LOGGING
UINT    _tx_el_interrupt_control(UINT new_posture);
#define tx_interrupt_control    _tx_el_interrupt_control
#else
#define tx_interrupt_control    _tx_thread_interrupt_control
#endif

#define tx_queue_create             _txe_queue_create
#define tx_queue_delete             _txe_queue_delete
#define tx_queue_flush              _txe_queue_flush
#define tx_queue_receive            _txe_queue_receive
#define tx_queue_send               _txe_queue_send

#define tx_semaphore_create         _txe_semaphore_create
#define tx_semaphore_delete         _txe_semaphore_delete
#define tx_semaphore_get            _txe_semaphore_get
#define tx_semaphore_put            _txe_semaphore_put

#define tx_thread_create            _txe_thread_create
#define tx_thread_delete            _txe_thread_delete
#define tx_thread_identify          _tx_thread_identify
#define tx_thread_preemption_change _txe_thread_preemption_change
#define tx_thread_priority_change   _txe_thread_priority_change
#define tx_thread_relinquish        _txe_thread_relinquish
#define tx_thread_resume            _txe_thread_resume_api
#define tx_thread_sleep             _tx_thread_sleep
#define tx_thread_suspend           _txe_thread_suspend_api
#define tx_thread_terminate         _txe_thread_terminate
#define tx_thread_time_slice_change _txe_thread_time_slice_change

#define tx_time_get                 _tx_time_get
#define tx_time_set                 _tx_time_set
#define tx_timer_activate           _txe_timer_activate_api
#define tx_timer_change             _txe_timer_change
#define tx_timer_create             _txe_timer_create
#define tx_timer_deactivate         _txe_timer_deactivate_api
#define tx_timer_delete             _txe_timer_delete

#endif


/* Define the function prototypes of the ThreadX API.  */

VOID        tx_kernel_enter(VOID);

UINT        tx_byte_allocate(TX_BYTE_POOL *pool_ptr, VOID **memory_ptr, ULONG memory_size,
                    ULONG wait_option);
UINT        tx_byte_pool_create(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start,
                    ULONG pool_size);
UINT        tx_byte_pool_delete(TX_BYTE_POOL *pool_ptr);
UINT        tx_byte_release(VOID *memory_ptr);

UINT        tx_block_allocate(TX_BLOCK_POOL *pool_ptr, VOID **block_ptr, ULONG wait_option);
UINT        tx_block_pool_create(TX_BLOCK_POOL *pool_ptr, CHAR *name_ptr, ULONG block_size,
                    VOID *pool_start, ULONG pool_size);
UINT        tx_block_pool_delete(TX_BLOCK_POOL *pool_ptr);
UINT        tx_block_release(VOID *block_ptr);

UINT        tx_event_flags_create(TX_EVENT_FLAGS_GROUP *group_ptr, CHAR *name_ptr);
UINT        tx_event_flags_delete(TX_EVENT_FLAGS_GROUP *group_ptr);
UINT        tx_event_flags_get(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG requested_flags,
                    UINT get_option, ULONG *actual_flags_ptr, ULONG wait_option);
UINT        tx_event_flags_set(TX_EVENT_FLAGS_GROUP *group_ptr, ULONG flags_to_set, 
                    UINT set_option);

UINT        tx_interrupt_control(UINT new_posture);

UINT        tx_queue_create(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, 
                        VOID *queue_start, ULONG queue_size);
UINT        tx_queue_delete(TX_QUEUE *queue_ptr);
UINT        tx_queue_flush(TX_QUEUE *queue_ptr);
UINT        tx_queue_receive(TX_QUEUE *queue_ptr, VOID *destination_ptr, ULONG wait_option);
UINT        tx_queue_send(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option);

UINT        tx_semaphore_create(TX_SEMAPHORE *semaphore_ptr, CHAR *name_ptr, ULONG initial_count);
UINT        tx_semaphore_delete(TX_SEMAPHORE *semaphore_ptr);
UINT        tx_semaphore_get(TX_SEMAPHORE *semaphore_ptr, ULONG wait_option);
UINT        tx_semaphore_put(TX_SEMAPHORE *semaphore_ptr);

UINT        tx_thread_create(TX_THREAD *thread_ptr, CHAR *name_ptr, 
                VOID (*entry_function)(ULONG), ULONG entry_input,
                VOID *stack_start, ULONG stack_size, 
                UINT priority, UINT preempt_threshold, 
                ULONG time_slice, UINT auto_start);
UINT        tx_thread_delete(TX_THREAD *thread_ptr);
TX_THREAD  *tx_thread_identify(VOID);
UINT        tx_thread_preemption_change(TX_THREAD *thread_ptr, UINT new_threshold,
                        UINT *old_threshold);
UINT        tx_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority,
                        UINT *old_priority);
VOID        tx_thread_relinquish(VOID);
UINT        tx_thread_resume(TX_THREAD *thread_ptr);
UINT        tx_thread_sleep(ULONG timer_ticks);
UINT        tx_thread_suspend(TX_THREAD *thread_ptr);
UINT        tx_thread_terminate(TX_THREAD *thread_ptr);
UINT        tx_thread_time_slice_change(TX_THREAD *thread_ptr, ULONG new_time_slice, ULONG *old_time_slice);

ULONG       tx_time_get(VOID);
VOID        tx_time_set(ULONG new_time);

UINT        tx_timer_activate(TX_TIMER *timer_ptr);
UINT        tx_timer_change(TX_TIMER *timer_ptr, ULONG initial_ticks, ULONG reschedule_ticks);
UINT        tx_timer_create(TX_TIMER *timer_ptr, CHAR *name_ptr, 
                VOID (*expiration_function)(ULONG), ULONG expiration_input, ULONG initial_ticks,
                ULONG reschedule_ticks, UINT auto_activate);
UINT        tx_timer_deactivate(TX_TIMER *timer_ptr);
UINT        tx_timer_delete(TX_TIMER *timer_ptr);


#endif


/* Bring in the event logging constants and prototypes.  Note that
   TX_ENABLE_EVENT_LOGGING must be defined when building the ThreadX
   library components in order to enable event logging.  */

#ifdef  TX_ENABLE_EVENT_LOGGING
#include "tx_el.h"
#else
#define TX_EL_INITIALIZE
#define TX_EL_THREAD_REGISTER(a)
#define TX_EL_THREAD_STATUS_CHANGE_INSERT(a, b)
#define TX_EL_BYTE_ALLOCATE_INSERT
#define TX_EL_BYTE_POOL_CREATE_INSERT
#define TX_EL_BYTE_POOL_DELETE_INSERT
#define TX_EL_BYTE_RELEASE_INSERT
#define TX_EL_BLOCK_ALLOCATE_INSERT
#define TX_EL_BLOCK_POOL_CREATE_INSERT
#define TX_EL_BLOCK_POOL_DELETE_INSERT
#define TX_EL_BLOCK_RELEASE_INSERT
#define TX_EL_EVENT_FLAGS_CREATE_INSERT
#define TX_EL_EVENT_FLAGS_DELETE_INSERT
#define TX_EL_EVENT_FLAGS_GET_INSERT
#define TX_EL_EVENT_FLAGS_SET_INSERT
#define TX_EL_INTERRUPT_CONTROL_INSERT
#define TX_EL_QUEUE_CREATE_INSERT
#define TX_EL_QUEUE_DELETE_INSERT
#define TX_EL_QUEUE_FLUSH_INSERT
#define TX_EL_QUEUE_RECEIVE_INSERT
#define TX_EL_QUEUE_SEND_INSERT
#define TX_EL_SEMAPHORE_CREATE_INSERT
#define TX_EL_SEMAPHORE_DELETE_INSERT
#define TX_EL_SEMAPHORE_GET_INSERT
#define TX_EL_SEMAPHORE_PUT_INSERT
#define TX_EL_THREAD_CREATE_INSERT
#define TX_EL_THREAD_DELETE_INSERT
#define TX_EL_THREAD_IDENTIFY_INSERT
#define TX_EL_THREAD_PREEMPTION_CHANGE_INSERT
#define TX_EL_THREAD_PRIORITY_CHANGE_INSERT
#define TX_EL_THREAD_RELINQUISH_INSERT
#define TX_EL_THREAD_RESUME_INSERT
#define TX_EL_THREAD_SLEEP_INSERT
#define TX_EL_THREAD_SUSPEND_INSERT
#define TX_EL_THREAD_TERMINATE_INSERT
#define TX_EL_THREAD_TIME_SLICE_CHANGE_INSERT
#define TX_EL_TIME_GET_INSERT
#define TX_EL_TIME_SET_INSERT
#define TX_EL_TIMER_ACTIVATE_INSERT
#define TX_EL_TIMER_CHANGE_INSERT
#define TX_EL_TIMER_CREATE_INSERT
#define TX_EL_TIMER_DEACTIVATE_INSERT
#define TX_EL_TIMER_DELETE_INSERT
#endif


/* Determine if a C++ compiler is being used.  If so, complete the standard
   C conditional started above.  */
#ifdef   __cplusplus
        }
#endif

#endif
