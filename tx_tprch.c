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


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_thread_priority_change                          PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function changes the priority of the specified thread.  It     */ 
/*    also returns the old priority and handles preemption if the calling */ 
/*    thread is currently executing and the priority change results in a  */ 
/*    higher priority thread ready for execution.                         */ 
/*                                                                        */ 
/*    Note: the preemption threshold is automatically changed to the new  */ 
/*    priority.                                                           */
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    thread_ptr                            Pointer to thread to suspend  */ 
/*    new_priority                          New thread priority           */ 
/*    old_priority                          Old thread priority           */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    status                                Completion status             */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    _tx_thread_system_return              Return to system if self-     */ 
/*                                            suspension                  */ 
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
/*  07-04-1997     William E. Lamie         Added casting of local        */ 
/*                                            priority_group variable in  */ 
/*                                            order to avoid compiler     */ 
/*                                            warnings, resulting in      */ 
/*                                            version 3.0a.               */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  03-01-1998     William E. Lamie         Corrected casting in making   */ 
/*                                            new priority bit map, and   */ 
/*                                            added clearing of bit in    */ 
/*                                            preempted map if thread is  */ 
/*                                            head of the priority list,  */ 
/*                                            resulting in version 3.0d.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_thread_priority_change(TX_THREAD *thread_ptr, UINT new_priority, UINT *old_priority)
{

TX_INTERRUPT_SAVE_AREA

REG_1 UINT      priority;               /* Thread priority          */
REG_2 ULONG     priority_map;           /* Working priority map     */
REG_3 UCHAR     priority_group;         /* Priority group           */


    /* Determine if the priority is legal.  */
    if (new_priority >= TX_MAX_PRIORITIES)
    
        /* Return an error status.  */
        return(TX_PRIORITY_ERROR);

    /* Lockout interrupts while the thread is being suspended.  */
    TX_DISABLE

    /* Save the previous priority.  */
    *old_priority =  thread_ptr -> tx_priority;

    /* Log this kernel call.  */
    TX_EL_THREAD_PRIORITY_CHANGE_INSERT

    /* Determine if this thread is currently ready.  */
    if (thread_ptr -> tx_state != TX_READY)
    {

        /* Easy, just setup the priority and threshold in the thread's control
           block.  */
        thread_ptr -> tx_priority =           new_priority;
        thread_ptr -> tx_preempt_threshold =  new_priority;

        /* Setup the new priority bit.  */
        thread_ptr -> tx_priority_bit =       (((ULONG) 1) << new_priority);

        /* Restore interrupts.  */
        TX_RESTORE
    }
    else
    {

        /* The thread is ready and must first be removed from the list.  */

        /* Pickup priority of thread.  */
        priority =  thread_ptr -> tx_priority;

        /* Determine if there are other threads at this priority that are
           ready.  */
        if (thread_ptr -> tx_ready_next != thread_ptr)
        {

            /* Yes, there are other threads at this priority ready.  */
    
            /* Adjust the head pointer if this thread is at the front
               of the list.  */
            if (_tx_thread_priority_list[priority] == thread_ptr)
            {

                /* Setup a new head pointer at this priority.  */
	            _tx_thread_priority_list[priority] =  thread_ptr -> tx_ready_next;

                /* Clear the corresponding bit in the preempted map.  */
                _tx_thread_preempted_map =  _tx_thread_preempted_map & ~(thread_ptr -> tx_priority_bit);
            }
    
            /* Just remove this thread from the priority list.  */
            (thread_ptr -> tx_ready_next) -> tx_ready_previous =    thread_ptr -> tx_ready_previous;
            (thread_ptr -> tx_ready_previous) -> tx_ready_next =    thread_ptr -> tx_ready_next;

            /* Setup the new priority bit.  */
            thread_ptr -> tx_priority_bit =     (((ULONG) 1) << new_priority);
        }
        else
        {

            /* This is the only thread at this priority ready to run.  Set the head 
               pointer to NULL.  */
            _tx_thread_priority_list[priority] =    TX_NULL;

            /* Clear this priority bit in the ready priority bit map.  */
            _tx_thread_priority_map =  _tx_thread_priority_map & ~(thread_ptr -> tx_priority_bit);

            /* Clear this priority bit in the preempted bit map just in case.  */
            _tx_thread_preempted_map =  _tx_thread_preempted_map & ~(thread_ptr -> tx_priority_bit);

            /* Setup the new priority bit.  */
            thread_ptr -> tx_priority_bit =  (((ULONG) 1) << new_priority);

            /* Put the priority map in a working copy.  */
            priority_map =  _tx_thread_priority_map;

            /* Find the next highest priority.  */

            /* Check for priorities 0-7.  */
            priority_group =  (UCHAR) (priority_map & TX_THREAD_PRIORITY_GROUP_MASK);
            if (priority_group)
                _tx_thread_highest_priority =  _tx_thread_lowest_bit[priority_group];		
            else
            {
		
                /* Check for priorities 8-15.  */
                priority_map =    priority_map >> TX_THREAD_GROUP_SIZE;
                priority_group =  (UCHAR) (priority_map & TX_THREAD_PRIORITY_GROUP_MASK);
                if (priority_group)
                    _tx_thread_highest_priority =  
                           TX_THREAD_GROUP_1 + _tx_thread_lowest_bit[priority_group];
                else
                {

                    /* Check for priorities 16-23.  */
                    priority_map =    priority_map >> TX_THREAD_GROUP_SIZE;
                    priority_group =  (UCHAR) (priority_map & TX_THREAD_PRIORITY_GROUP_MASK);
                    if (priority_group)
                        _tx_thread_highest_priority =  
                                    TX_THREAD_GROUP_2 + _tx_thread_lowest_bit[priority_group];
                    else
                    {

                        priority_map =    priority_map >> TX_THREAD_GROUP_SIZE;
                        priority_group =  (UCHAR) (priority_map & TX_THREAD_PRIORITY_GROUP_MASK);
                        if (priority_group)
                            _tx_thread_highest_priority =  
                                    TX_THREAD_GROUP_3 + _tx_thread_lowest_bit[priority_group];
                        else

                            /* No more priorities, set highest priority accordingly.  */
                            _tx_thread_highest_priority =  TX_MAX_PRIORITIES;
                    }
                }
            }
        }

        /* Determine if this thread is the thread designated to execute.  */
        if (thread_ptr == _tx_thread_execute_ptr)
        {

            /* Pickup the highest priority thread to execute, if there is
               one.  */
	        if (_tx_thread_highest_priority != TX_MAX_PRIORITIES)

                /* Setup the next thread to execute.  */
                _tx_thread_execute_ptr =  _tx_thread_priority_list[_tx_thread_highest_priority];
            else

                /* No more threads ready to execute, set execute pointer to NULL.  */
                _tx_thread_execute_ptr =  TX_NULL;
        }

        /* Okay at this point, the thread is off of the list.  Temporarily setup the 
           preemption threshold to prevent any other thread from running.  */
        thread_ptr -> tx_preempt_threshold =  0;

        /* Setup the new thread priority.  */
        thread_ptr -> tx_priority =  new_priority;

        /* Set the state to suspended so the resume function will work. */
        thread_ptr -> tx_state =  TX_SUSPENDED;

        /* Temporarily disable preemption.  */
        _tx_thread_preempt_disable++;
        
        /* Restore interrupts temporarily. */
        TX_RESTORE

        /* Resume this thread.  */
        _tx_thread_resume(thread_ptr);

        /* Setup the preempt threshold.  */
        thread_ptr -> tx_preempt_threshold =  new_priority;

        /* Determine if a context switch is required.  */
        if ((_tx_thread_current_ptr != _tx_thread_execute_ptr) &&
            (_tx_thread_system_state == 0))

            /* Return control to the system.  */
            _tx_thread_system_return();
    }

    /* Return success if we get here!  */
    return(TX_SUCCESS);
}

