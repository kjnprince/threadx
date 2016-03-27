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
/**   Semaphore (SEM)                                                     */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_sem.h"

/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_semaphore_create                                PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function creates a counting semaphore with the initial count   */ 
/*    specified in this call.                                             */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    semaphore_ptr                     Pointer to semaphore control block*/ 
/*    name_ptr                          Pointer to semaphore name         */ 
/*    initial_count                     Initial semaphore count           */ 
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
/*  11-11-1997     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_semaphore_create(TX_SEMAPHORE *semaphore_ptr, 
                                        CHAR *name_ptr, ULONG initial_count)
{

TX_INTERRUPT_SAVE_AREA

TX_SEMAPHORE   *tail_ptr;                   /* Working semaphore pointer  */


    /* Setup the basic semaphore fields.  */
    semaphore_ptr -> tx_semaphore_name =             name_ptr;
    semaphore_ptr -> tx_semaphore_count =            initial_count;
    semaphore_ptr -> tx_semaphore_suspension_list =  TX_NULL;
    semaphore_ptr -> tx_semaphore_suspended_count =  0;
    
    /* Disable interrupts to place the semaphore on the created list.  */
    TX_DISABLE

    /* Setup the semaphore ID to make it valid.  */
    semaphore_ptr -> tx_semaphore_id =  TX_SEMAPHORE_ID;

    /* Place the semaphore on the list of created semaphores.  First,
       check for an empty list.  */
    if (_tx_semaphore_created_ptr)
    {

        /* Pickup tail pointer.  */
        tail_ptr =  _tx_semaphore_created_ptr -> tx_semaphore_created_previous;

        /* Place the new semaphore in the list.  */
        _tx_semaphore_created_ptr -> tx_semaphore_created_previous =  semaphore_ptr;
	    tail_ptr -> tx_semaphore_created_next =                       semaphore_ptr;

        /* Setup this semaphore's next and previous created links.  */
        semaphore_ptr -> tx_semaphore_created_previous =  tail_ptr;
        semaphore_ptr -> tx_semaphore_created_next =      _tx_semaphore_created_ptr;	
    }
    else
    {

        /* The created semaphore list is empty.  Add semaphore to empty list.  */
        _tx_semaphore_created_ptr =                       semaphore_ptr;
        semaphore_ptr -> tx_semaphore_created_next =      semaphore_ptr;
        semaphore_ptr -> tx_semaphore_created_previous =  semaphore_ptr;
    }

    /* Increment the number of semaphores created counter.  */
    _tx_semaphore_created_count++;

    /* Log this kernel call.  */
    TX_EL_SEMAPHORE_CREATE_INSERT

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

