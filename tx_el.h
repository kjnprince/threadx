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
/**   ThreadX/GHS Event Log (EL)                                          */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    tx_el.h                                          68332/Green Hills  */ 
/*                                                           3.0a         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the ThreadX event log functions for the GHS MULTI */ 
/*    EventAnalyzer.  It is assumed that tx_api.h and tx_port.h have      */ 
/*    already been included.                                              */
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  12-02-1999     William E. Lamie         Initial Version 3.0a          */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef  TX_EL
#define  TX_EL


/* Define Event Log specific data definitions.  */

#define TX_EL_VERSION_ID                2           /* Event log version ID             */
#define TX_EL_HEADER_SIZE               24          /* Event log header size            */
#define TX_EL_TNIS                      16          /* Number of thread names supported */
#define TX_EL_TNI_ENTRY_SIZE            44          /* Thread name entries are 44 bytes */
#define TX_EL_TNI_NAME_SIZE             34          /* Thread name size in TNI          */
#define TX_EL_NO_MORE_TNI_ROOM          1           /* Error return from thread register*/
#define TX_EL_EVENT_SIZE                32          /* Number of bytes in each event    */
#define TX_EL_VALID_ENTRY               1           /* Valid log entry                  */ 
#define TX_EL_INVALID_ENTRY             0           /* Invalid log entry                */
#define TX_EL_TICKS_PER_SECOND          0xF4240     /* Number of ticks per second in    */
                                                    /*   the time stamp source          */


/* Define necessary offsets.  */

#define TX_EL_TNI_VALID_OFFSET          34
#define TX_EL_TNI_THREAD_ID_OFFSET      36
#define TX_EL_TNI_THREAD_PRIORITY_OFF   40
#define TX_EL_EVENT_TYPE_OFFSET         0
#define TX_EL_EVENT_SUBTYPE_OFFSET      2
#define TX_EL_EVENT_TIME_UPPER_OFFSET   4
#define TX_EL_EVENT_TIME_LOWER_OFFSET   8
#define TX_EL_EVENT_THREAD_OFFSET       12
#define TX_EL_EVENT_INFO_1_OFFSET       16
#define TX_EL_EVENT_INFO_2_OFFSET       20
#define TX_EL_EVENT_INFO_3_OFFSET       24
#define TX_EL_EVENT_INFO_4_OFFSET       28


/* Undefine constants that might be been defined previously by tx_api.h.  */

#undef TX_EL_INITIALIZE
#undef TX_EL_THREAD_REGISTER(a)
#undef TX_EL_THREAD_STATUS_CHANGE_INSERT(a, b)
#undef TX_EL_BYTE_ALLOCATE_INSERT
#undef TX_EL_BYTE_POOL_CREATE_INSERT
#undef TX_EL_BYTE_POOL_DELETE_INSERT
#undef TX_EL_BYTE_RELEASE_INSERT
#undef TX_EL_BLOCK_ALLOCATE_INSERT
#undef TX_EL_BLOCK_POOL_CREATE_INSERT
#undef TX_EL_BLOCK_POOL_DELETE_INSERT
#undef TX_EL_BLOCK_RELEASE_INSERT
#undef TX_EL_EVENT_FLAGS_CREATE_INSERT
#undef TX_EL_EVENT_FLAGS_DELETE_INSERT
#undef TX_EL_EVENT_FLAGS_GET_INSERT
#undef TX_EL_EVENT_FLAGS_SET_INSERT
#undef TX_EL_INTERRUPT_CONTROL_INSERT
#undef TX_EL_QUEUE_CREATE_INSERT
#undef TX_EL_QUEUE_DELETE_INSERT
#undef TX_EL_QUEUE_FLUSH_INSERT
#undef TX_EL_QUEUE_RECEIVE_INSERT
#undef TX_EL_QUEUE_SEND_INSERT
#undef TX_EL_SEMAPHORE_CREATE_INSERT
#undef TX_EL_SEMAPHORE_DELETE_INSERT
#undef TX_EL_SEMAPHORE_GET_INSERT
#undef TX_EL_SEMAPHORE_PUT_INSERT
#undef TX_EL_THREAD_CREATE_INSERT
#undef TX_EL_THREAD_DELETE_INSERT
#undef TX_EL_THREAD_IDENTIFY_INSERT
#undef TX_EL_THREAD_PREEMPTION_CHANGE_INSERT
#undef TX_EL_THREAD_PRIORITY_CHANGE_INSERT
#undef TX_EL_THREAD_RELINQUISH_INSERT
#undef TX_EL_THREAD_RESUME_INSERT
#undef TX_EL_THREAD_SLEEP_INSERT
#undef TX_EL_THREAD_SUSPEND_INSERT
#undef TX_EL_THREAD_TERMINATE_INSERT
#undef TX_EL_THREAD_TIME_SLICE_CHANGE_INSERT
#undef TX_EL_TIME_GET_INSERT
#undef TX_EL_TIME_SET_INSERT
#undef TX_EL_TIMER_ACTIVATE_INSERT
#undef TX_EL_TIMER_CHANGE_INSERT
#undef TX_EL_TIMER_CREATE_INSERT
#undef TX_EL_TIMER_DEACTIVATE_INSERT
#undef TX_EL_TIMER_DELETE_INSERT



/* Define Event Types.  */

#define TX_EL_THREAD_CHANGE             1
#define TX_EL_INTERRUPT                 2
#define TX_EL_THREADX_CALL              3
#define TX_EL_USER_EVENT                4
#define TX_EL_THREAD_STATUS_CHANGE      5
#define TX_EL_REFRESH                   6           /* Not implemented                  */
#define TX_EL_TIMER                     7           /* Not implemented                  */
#define TX_EL_TIMESOURCE_DELTA          8           /* Not implemented                  */


/* Define TX_EL_THREADX_CALL event sub-types.  */

#define TX_EL_BYTE_ALLOCATE             0
#define TX_EL_BYTE_POOL_CREATE          1
#define TX_EL_BYTE_POOL_DELETE          2
#define TX_EL_BYTE_RELEASE              3
#define TX_EL_BLOCK_ALLOCATE            4
#define TX_EL_BLOCK_POOL_CREATE         5
#define TX_EL_BLOCK_POOL_DELETE         6
#define TX_EL_BLOCK_RELEASE             7
#define TX_EL_EVENT_FLAGS_CREATE        8
#define TX_EL_EVENT_FLAGS_DELETE        9
#define TX_EL_EVENT_FLAGS_GET           10
#define TX_EL_EVENT_FLAGS_SET           11
#define TX_EL_INTERRUPT_CONTROL         12
#define TX_EL_QUEUE_CREATE              13
#define TX_EL_QUEUE_DELETE              14
#define TX_EL_QUEUE_FLUSH               15
#define TX_EL_QUEUE_RECEIVE             16
#define TX_EL_QUEUE_SEND                17
#define TX_EL_SEMAPHORE_CREATE          18
#define TX_EL_SEMAPHORE_DELETE          19
#define TX_EL_SEMAPHORE_GET             20
#define TX_EL_SEMAPHORE_PUT             21
#define TX_EL_THREAD_CREATE             22
#define TX_EL_THREAD_DELETE             23
#define TX_EL_THREAD_IDENTIFY           24
#define TX_EL_THREAD_PREEMPTION_CHANGE  25
#define TX_EL_THREAD_PRIORITY_CHANGE    26
#define TX_EL_THREAD_RELINQUISH         27
#define TX_EL_THREAD_RESUME             28
#define TX_EL_THREAD_SLEEP              29
#define TX_EL_THREAD_SUSPEND            30
#define TX_EL_THREAD_TERMINATE          31
#define TX_EL_THREAD_TIME_SLICE_CHANGE  32
#define TX_EL_TIME_GET                  33
#define TX_EL_TIME_SET                  34
#define TX_EL_TIMER_ACTIVATE            35
#define TX_EL_TIMER_CHANGE              36
#define TX_EL_TIMER_CREATE              37
#define TX_EL_TIMER_DEACTIVATE          38
#define TX_EL_TIMER_DELETE              39


/* Define ThreadX sub-types.  */

#define TX_EL_INTERRUPT_SUB_TYPE        1
#define TX_EL_END_OF_INTERRUPT          3


/* Define event logging filters, which may be used by the application program to
   dynamically enable/disable events in run-time.  */

#define TX_EL_FILTER_STATUS_CHANGE      0x0001
#define TX_EL_FILTER_INTERRUPTS         0x0002
#define TX_EL_FILTER_THREAD_CALLS       0x0004
#define TX_EL_FILTER_TIMER_CALLS        0x0008
#define TX_EL_FILTER_EVENT_FLAG_CALLS   0x0010
#define TX_EL_FILTER_SEMAPHORE_CALLS    0x0020
#define TX_EL_FILTER_QUEUE_CALLS        0x0040
#define TX_EL_FILTER_BLOCK_CALLS        0x0080
#define TX_EL_FILTER_BYTE_CALLS         0x0100
#define TX_EL_FILTER_ALL_EVENTS         0xFFFF
#define TX_EL_ENABLE_ALL_EVENTS         0x0000


/* Define filter macros that are inserted in-line with the other macros below.  */

#ifdef  TX_ENABLE_EVENT_FILTERS     
#define TX_EL_NO_STATUS_EVENTS      if (!(_tx_el_event_filter & TX_EL_FILTER_STATUS_CHANGE)) {
#define TX_EL_NO_INTERRUPT_EVENTS   if (!(_tx_el_event_filter & TX_EL_FILTER_INTERRUPTS)) {
#define TX_EL_NO_THREAD_EVENTS      if (!(_tx_el_event_filter & TX_EL_FILTER_THREAD_CALLS)) {
#define TX_EL_NO_TIMER_EVENTS       if (!(_tx_el_event_filter & TX_EL_FILTER_TIMER_CALLS)) {
#define TX_EL_NO_EVENT_FLAG_EVENTS  if (!(_tx_el_event_filter & TX_EL_FILTER_EVENT_FLAG_CALLS)) {
#define TX_EL_NO_SEMAPHORE_EVENTS   if (!(_tx_el_event_filter & TX_EL_FILTER_EVENT_FLAG_CALLS)) {
#define TX_EL_NO_QUEUE_EVENTS       if (!(_tx_el_event_filter & TX_EL_FILTER_QUEUE_CALLS)) {
#define TX_EL_NO_BLOCK_EVENTS       if (!(_tx_el_event_filter & TX_EL_FILTER_BLOCK_CALLS)) {
#define TX_EL_NO_BYTE_EVENTS        if (!(_tx_el_event_filter & TX_EL_FILTER_BYTE_CALLS)) {
#define TX_EL_END_FILTER            }
#else
#define TX_EL_NO_STATUS_EVENTS
#define TX_EL_NO_INTERRUPT_EVENTS
#define TX_EL_NO_THREAD_EVENTS
#define TX_EL_NO_TIMER_EVENTS
#define TX_EL_NO_EVENT_FLAG_EVENTS
#define TX_EL_NO_SEMAPHORE_EVENTS
#define TX_EL_NO_QUEUE_EVENTS
#define TX_EL_NO_BLOCK_EVENTS
#define TX_EL_NO_BYTE_EVENTS
#define TX_EL_END_FILTER
#endif

/* Define externs and constants for non-event log source modules.  This is for
   the in-line macros below.  */

#ifndef TX_EL_SOURCE_CODE
extern UCHAR            *_tx_el_tni_start;
extern UCHAR            **_tx_el_current_event;
extern UCHAR            *_tx_el_event_area_start;
extern UCHAR            *_tx_el_event_area_end;
extern UINT             _tx_el_maximum_events;
extern ULONG            _tx_el_total_events;
extern TX_THREAD        *_tx_thread_current_ptr;
extern UINT             _tx_el_event_filter;
extern ULONG            _tx_el_fake_time_stamp;


/* Define macros for event logging functions.  */

#define TX_EL_THREAD_CREATE_INSERT              TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO4(TX_EL_THREAD_CREATE, thread_ptr, stack_start, stack_size, priority); TX_EL_END_FILTER
#define TX_EL_EVENT_FLAGS_SET_INSERT            TX_EL_NO_EVENT_FLAG_EVENTS  TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(TX_EL_EVENT_FLAGS_SET, group_ptr, flags_to_set, set_option); TX_EL_END_FILTER
#define TX_EL_THREAD_DELETE_INSERT              TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_THREAD_DELETE, thread_ptr); TX_EL_END_FILTER
#define TX_EL_THREAD_TIME_SLICE_CHANGE_INSERT   TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(TX_EL_THREAD_TIME_SLICE_CHANGE, thread_ptr, thread_ptr -> tx_new_time_slice, new_time_slice); TX_EL_END_FILTER
#define TX_EL_THREAD_TERMINATE_INSERT           TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_THREAD_TERMINATE, thread_ptr); TX_EL_END_FILTER
#define TX_EL_THREAD_SLEEP_INSERT               TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_THREAD_SLEEP, timer_ticks); TX_EL_END_FILTER
#define TX_EL_THREAD_SUSPEND_INSERT             TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_THREAD_SUSPEND, thread_ptr); TX_EL_END_FILTER
#define TX_EL_THREAD_RELINQUISH_INSERT          TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO0(TX_EL_THREAD_RELINQUISH); TX_EL_END_FILTER
#define TX_EL_THREAD_RESUME_INSERT              TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_THREAD_RESUME, thread_ptr); TX_EL_END_FILTER
#define TX_EL_THREAD_PRIORITY_CHANGE_INSERT     TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(TX_EL_THREAD_PRIORITY_CHANGE, thread_ptr, thread_ptr -> tx_priority, new_priority); TX_EL_END_FILTER
#define TX_EL_THREAD_PREEMPTION_CHANGE_INSERT   TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(TX_EL_THREAD_PREEMPTION_CHANGE, thread_ptr, thread_ptr -> tx_preempt_threshold, new_threshold); TX_EL_END_FILTER
#define TX_EL_TIME_SET_INSERT                   TX_EL_NO_TIMER_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_TIME_SET, new_time); TX_EL_END_FILTER
#define TX_EL_TIME_GET_INSERT                   TX_EL_NO_TIMER_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_TIME_GET, _tx_timer_system_clock); TX_EL_END_FILTER
#define TX_EL_TIMER_DELETE_INSERT               TX_EL_NO_TIMER_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_TIMER_DELETE, timer_ptr); TX_EL_END_FILTER
#define TX_EL_TIMER_CREATE_INSERT               TX_EL_NO_TIMER_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO4(TX_EL_TIMER_CREATE, timer_ptr, initial_ticks, reschedule_ticks, auto_activate); TX_EL_END_FILTER
#define TX_EL_TIMER_CHANGE_INSERT               TX_EL_NO_TIMER_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(TX_EL_TIMER_CHANGE, timer_ptr, initial_ticks, reschedule_ticks); TX_EL_END_FILTER
#define TX_EL_THREAD_IDENTIFY_INSERT            TX_EL_NO_THREAD_EVENTS      TX_EL_KERNEL_CALL_EVENT_INSERT_INFO0(TX_EL_THREAD_IDENTIFY); TX_EL_END_FILTER
#define TX_EL_TIMER_DEACTIVATE_INSERT           TX_EL_NO_TIMER_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_TIMER_DEACTIVATE, timer_ptr); TX_EL_END_FILTER
#define TX_EL_TIMER_ACTIVATE_INSERT             TX_EL_NO_TIMER_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_TIMER_ACTIVATE, timer_ptr); TX_EL_END_FILTER
#define TX_EL_SEMAPHORE_PUT_INSERT              TX_EL_NO_SEMAPHORE_EVENTS   TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(TX_EL_SEMAPHORE_PUT, semaphore_ptr, semaphore_ptr -> tx_semaphore_count); TX_EL_END_FILTER
#define TX_EL_SEMAPHORE_GET_INSERT              TX_EL_NO_SEMAPHORE_EVENTS   TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(TX_EL_SEMAPHORE_GET, semaphore_ptr, semaphore_ptr -> tx_semaphore_count); TX_EL_END_FILTER
#define TX_EL_SEMAPHORE_DELETE_INSERT           TX_EL_NO_SEMAPHORE_EVENTS   TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_SEMAPHORE_DELETE, semaphore_ptr); TX_EL_END_FILTER
#define TX_EL_SEMAPHORE_CREATE_INSERT           TX_EL_NO_SEMAPHORE_EVENTS   TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(TX_EL_SEMAPHORE_CREATE, semaphore_ptr, initial_count); TX_EL_END_FILTER
#define TX_EL_QUEUE_SEND_INSERT                 TX_EL_NO_QUEUE_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(TX_EL_QUEUE_SEND, queue_ptr, source_ptr); TX_EL_END_FILTER
#define TX_EL_QUEUE_RECEIVE_INSERT              TX_EL_NO_QUEUE_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(TX_EL_QUEUE_RECEIVE, queue_ptr, destination_ptr); TX_EL_END_FILTER
#define TX_EL_QUEUE_FLUSH_INSERT                TX_EL_NO_QUEUE_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_QUEUE_FLUSH, queue_ptr); TX_EL_END_FILTER
#define TX_EL_QUEUE_DELETE_INSERT               TX_EL_NO_QUEUE_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_QUEUE_DELETE, queue_ptr); TX_EL_END_FILTER
#define TX_EL_QUEUE_CREATE_INSERT               TX_EL_NO_QUEUE_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO4(TX_EL_QUEUE_CREATE, queue_ptr, queue_start, queue_size, message_size); TX_EL_END_FILTER
#define TX_EL_EVENT_FLAGS_GET_INSERT            TX_EL_NO_EVENT_FLAG_EVENTS  TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(TX_EL_EVENT_FLAGS_GET, group_ptr, requested_flags, get_option); TX_EL_END_FILTER
#define TX_EL_EVENT_FLAGS_DELETE_INSERT         TX_EL_NO_EVENT_FLAG_EVENTS  TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_EVENT_FLAGS_DELETE, group_ptr); TX_EL_END_FILTER
#define TX_EL_EVENT_FLAGS_CREATE_INSERT         TX_EL_NO_EVENT_FLAG_EVENTS  TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_EVENT_FLAGS_CREATE, group_ptr); TX_EL_END_FILTER
#define TX_EL_BYTE_RELEASE_INSERT               TX_EL_NO_BYTE_EVENTS        TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(TX_EL_BYTE_RELEASE, pool_ptr, memory_ptr); TX_EL_END_FILTER
#define TX_EL_BYTE_POOL_DELETE_INSERT           TX_EL_NO_BYTE_EVENTS        TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_BYTE_POOL_DELETE, pool_ptr); TX_EL_END_FILTER
#define TX_EL_BYTE_POOL_CREATE_INSERT           TX_EL_NO_BYTE_EVENTS        TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(TX_EL_BYTE_POOL_CREATE, pool_ptr, pool_start, pool_size); TX_EL_END_FILTER
#define TX_EL_BYTE_ALLOCATE_INSERT              TX_EL_NO_BYTE_EVENTS        TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(TX_EL_BYTE_ALLOCATE, pool_ptr, memory_ptr, memory_size); TX_EL_END_FILTER
#define TX_EL_BLOCK_RELEASE_INSERT              TX_EL_NO_BLOCK_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(TX_EL_BLOCK_RELEASE, pool_ptr, block_ptr); TX_EL_END_FILTER
#define TX_EL_BLOCK_POOL_DELETE_INSERT          TX_EL_NO_BLOCK_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(TX_EL_BLOCK_POOL_DELETE, pool_ptr); TX_EL_END_FILTER
#define TX_EL_BLOCK_POOL_CREATE_INSERT          TX_EL_NO_BLOCK_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO4(TX_EL_BLOCK_POOL_CREATE, pool_ptr, pool_start, pool_size, block_size); TX_EL_END_FILTER
#define TX_EL_BLOCK_ALLOCATE_INSERT             TX_EL_NO_BLOCK_EVENTS       TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(TX_EL_BLOCK_ALLOCATE, pool_ptr, block_ptr); TX_EL_END_FILTER

#endif


/* Define Event Log function prototypes.  */

VOID        _tx_el_initialize(VOID);
UINT        _tx_el_thread_register(TX_THREAD *thread_ptr);
VOID        _tx_el_user_event_insert(UINT sub_type, ULONG info_1, ULONG info_2, 
                                                    ULONG info_3, ULONG info_4);
VOID        _tx_el_thread_running(TX_THREAD *thread_ptr);
VOID        _tx_el_thread_preempted(TX_THREAD *thread_ptr);
VOID        _tx_el_interrupt(UINT interrupt_number);
VOID        _tx_el_interrupt_end(UINT interrupt_number);
VOID        _tx_el_interrupt_control_call(void);
VOID        _tx_el_event_log_on(void);
VOID        _tx_el_event_log_off(void);
VOID        _tx_el_event_filter_set(UINT filter);


/* Define macros that are used inside the ThreadX source code.
   If event logging is disabled, these macros will be defined
   as white space.  */

#ifdef  TX_ENABLE_EVENT_LOGGING
#ifndef TX_NO_EVENT_INFO
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO4(a, b, c, d, e)  \
                        {  \
                        UCHAR   *entry_ptr; \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREADX_CALL; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) a; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) _tx_thread_current_ptr;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_1_OFFSET)) =\
                                (ULONG) b;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_2_OFFSET)) =\
                                (ULONG) c;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_3_OFFSET)) =\
                                (ULONG) d;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_4_OFFSET)) =\
                                (ULONG) e;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        }
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(a, b, c, d) \
                        {  \
                        UCHAR   *entry_ptr; \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREADX_CALL; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) a; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) _tx_thread_current_ptr;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_1_OFFSET)) =\
                                (ULONG) b;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_2_OFFSET)) =\
                                (ULONG) c;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_3_OFFSET)) =\
                                (ULONG) d;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        }
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(a, b, c) \
                        {  \
                        UCHAR   *entry_ptr; \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREADX_CALL; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) a; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) _tx_thread_current_ptr;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_1_OFFSET)) =\
                                (ULONG) b;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_2_OFFSET)) =\
                                (ULONG) c;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        }
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(a, b) \
                        {  \
                        UCHAR   *entry_ptr; \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREADX_CALL; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) a; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) _tx_thread_current_ptr;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_INFO_1_OFFSET)) =\
                                (ULONG) b;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        }
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO0(a) \
                        {  \
                        UCHAR   *entry_ptr; \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREADX_CALL; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) a; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) _tx_thread_current_ptr;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        }
#define TX_EL_THREAD_STATUS_CHANGE_INSERT(a, b) \
                        {  \
                        UCHAR   *entry_ptr; \
                        TX_EL_NO_STATUS_EVENTS \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREAD_STATUS_CHANGE; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) b; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) a;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        TX_EL_END_FILTER \
                        }
#define TX_EL_THREAD_REGISTER(a) \
                                _tx_el_thread_register(a);
#define TX_EL_INITIALIZE        _tx_el_initialize();
#else
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO4(a, b, c, d, e)  \
                        {  \
                        UCHAR   *entry_ptr; \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREADX_CALL; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) a; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) _tx_thread_current_ptr;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        }
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(a, b, c, d) \
                        {  \
                        UCHAR   *entry_ptr; \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREADX_CALL; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) a; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) _tx_thread_current_ptr;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        }
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(a, b, c) \
                        {  \
                        UCHAR   *entry_ptr; \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREADX_CALL; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) a; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) _tx_thread_current_ptr;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        }
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(a, b) \
                        {  \
                        UCHAR   *entry_ptr; \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREADX_CALL; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) a; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) _tx_thread_current_ptr;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        }
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO0(a) \
                        {  \
                        UCHAR   *entry_ptr; \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREADX_CALL; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) a; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) _tx_thread_current_ptr;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        }
#define TX_EL_THREAD_STATUS_CHANGE_INSERT(a, b) \
                        {  \
                        UCHAR   *entry_ptr; \
                        TX_EL_NO_STATUS_EVENTS \
                        entry_ptr =  *_tx_el_current_event; \
                        *((unsigned short *) entry_ptr) =  TX_EL_THREAD_STATUS_CHANGE; \
                        *((unsigned short *) (entry_ptr + TX_EL_EVENT_SUBTYPE_OFFSET)) = (unsigned short) b; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_UPPER_OFFSET)) = 0; \
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_TIME_LOWER_OFFSET)) =\
                                (ULONG) _tx_el_fake_time_stamp++;\
                        *((ULONG *) (entry_ptr + TX_EL_EVENT_THREAD_OFFSET)) =\
                                (ULONG) a;\
                        entry_ptr =  entry_ptr + TX_EL_EVENT_SIZE;\
                        if (entry_ptr >= _tx_el_event_area_end) \
                        {\
                                entry_ptr =  _tx_el_event_area_start;\
                        }\
                        *_tx_el_current_event =  entry_ptr;\
                        TX_EL_END_FILTER \
                        }
#define TX_EL_THREAD_REGISTER(a) \
                                _tx_el_thread_register(a);
#define TX_EL_INITIALIZE        _tx_el_initialize();
#endif            
#else
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO4(a, b, c, d, e)
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO3(a, b, c, d)
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO2(a, b, c)
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO1(a, b)
#define TX_EL_KERNEL_CALL_EVENT_INSERT_INFO0(a)
#define TX_EL_THREAD_STATUS_CHANGE_INSERT(a, b)
#define TX_EL_THREAD_REGISTER(a)
#define TX_EL_INITIALIZE
#endif

#endif
