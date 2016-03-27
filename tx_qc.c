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
/**   Queue (QUE)                                                         */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_que.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_queue_create                                    PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function creates a message queue.  The message size and depth  */ 
/*    of the queue is specified by the caller.                            */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    queue_ptr                         Pointer to queue control block    */ 
/*    name_ptr                          Pointer to queue name             */ 
/*    message_size                      Size of each queue message        */ 
/*    queue_start                       Starting address of the queue area*/ 
/*    queue_size                        Number of bytes in the queue      */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    TX_SUCCESS                        Successful completion status      */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
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
/*  07-04-1997     William E. Lamie         Removed message size typdef   */ 
/*                                            references, resulting in    */ 
/*                                            version 3.0a.               */ 
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_queue_create(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, 
                        VOID *queue_start, ULONG queue_size)
{

TX_INTERRUPT_SAVE_AREA

TX_QUEUE    *tail_ptr;                      /* Working queue pointer     */
REG_1 UINT  capacity;                       /* Queue's message capacity  */
REG_2 UINT  used_words;                     /* Number of words used      */


    /* Setup the basic queue fields.  */
    queue_ptr -> tx_queue_name =             name_ptr;
    queue_ptr -> tx_queue_suspension_list =  TX_NULL;
    queue_ptr -> tx_queue_suspended_count =  0;
    
    /* Save the message size in the control block.  */
    queue_ptr -> tx_queue_message_size =  message_size;

    /* Determine how many messages will fit in the queue area and the number
       of ULONGs used.  */
    if (message_size == TX_1_ULONG)
    {
        capacity =  queue_size / (TX_1_ULONG * sizeof(ULONG));
        used_words =  capacity;
    }
    else if (message_size == TX_2_ULONG)
    {
        capacity =  queue_size / (TX_2_ULONG * sizeof(ULONG));
        used_words =  capacity * TX_2_ULONG;
    }
    else if (message_size == TX_4_ULONG)
    {
        capacity =  queue_size / (TX_4_ULONG * sizeof(ULONG));
        used_words =  capacity * TX_4_ULONG;
    }
    else if (message_size == TX_8_ULONG)
    {
        capacity =  queue_size / (TX_8_ULONG * sizeof(ULONG));
        used_words =  capacity * TX_8_ULONG;
    }
    else 
    {
        capacity =  queue_size / (TX_16_ULONG * sizeof(ULONG));
        used_words =  capacity * TX_16_ULONG;
    }

    /* Save the starting address and calculate the ending address of 
       the queue.  Note that the ending address is really one past the
       end!  */
    queue_ptr -> tx_queue_start =  (ULONG_PTR) queue_start;
    queue_ptr -> tx_queue_end =    ((ULONG_PTR) queue_start) + used_words;

    /* Set the read and write pointers to the beginning of the queue
       area.  */
    queue_ptr -> tx_queue_read =   (ULONG_PTR) queue_start;
    queue_ptr -> tx_queue_write =  (ULONG_PTR) queue_start;

    /* Setup the number of enqueued messages and the number of message
       slots available in the queue.  */
    queue_ptr -> tx_queue_enqueued =           0;
    queue_ptr -> tx_queue_available_storage =  capacity;
    queue_ptr -> tx_queue_capacity =           capacity;

    /* Disable interrupts to put the queue on the created list.  */
    TX_DISABLE

    /* Setup the queue ID to make it valid.  */
    queue_ptr -> tx_queue_id =  TX_QUEUE_ID;

    /* Place the queue on the list of created queues.  First,
       check for an empty list.  */
    if (_tx_queue_created_ptr)
    {

        /* Pickup tail pointer.  */
        tail_ptr =  _tx_queue_created_ptr -> tx_queue_created_previous;

        /* Place the new queue in the list.  */
        _tx_queue_created_ptr -> tx_queue_created_previous =  queue_ptr;
	    tail_ptr -> tx_queue_created_next =  queue_ptr;

        /* Setup this queues's created links.  */
        queue_ptr -> tx_queue_created_previous =  tail_ptr;
        queue_ptr -> tx_queue_created_next =      _tx_queue_created_ptr;	
    }
    else
    {

        /* The created queue list is empty.  Add queue to empty list.  */
        _tx_queue_created_ptr =                   queue_ptr;
        queue_ptr -> tx_queue_created_next =      queue_ptr;
        queue_ptr -> tx_queue_created_previous =  queue_ptr;
    }

    /* Increment the number of queues created counter.  */
    _tx_queue_created_count++;

    /* Log this kernel call.  */
    TX_EL_QUEUE_CREATE_INSERT

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

