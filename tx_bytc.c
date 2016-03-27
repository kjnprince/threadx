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
/**   Byte Pool (BYT)                                                     */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_byt.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_byte_pool_create                                PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function creates a pool of memory bytes in the specified       */ 
/*    memory area.                                                        */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    pool_ptr                          Pointer to pool control block     */ 
/*    name_ptr                          Pointer to byte pool name         */ 
/*    pool_start                        Address of beginning of pool area */ 
/*    pool_size                         Number of bytes in the byte pool  */ 
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
UINT    _tx_byte_pool_create(TX_BYTE_POOL *pool_ptr, CHAR *name_ptr, VOID *pool_start,
                    ULONG pool_size)
{

TX_INTERRUPT_SAVE_AREA

TX_BYTE_POOL   *tail_ptr;                   /* Working byte pool pointer   */
CHAR_PTR        block_ptr;                  /* Working block pointer       */


    /* Round the pool size down to something that is evenly divisable by 
       an ULONG.  */
    pool_size =   (pool_size/sizeof(ULONG)) * sizeof(ULONG);

    /* Setup the basic byte pool fields.  */
    pool_ptr -> tx_byte_pool_name =              name_ptr;
    pool_ptr -> tx_byte_pool_suspension_list =   TX_NULL;
    pool_ptr -> tx_byte_pool_suspended_count =   0;

    /* Save the start and size of the pool.  */
    pool_ptr -> tx_byte_pool_start =   (CHAR_PTR) pool_start;
    pool_ptr -> tx_byte_pool_size =    pool_size;

    /* Setup memory list to the beginning as well as the search pointer.  */
    pool_ptr -> tx_byte_pool_list =    (CHAR_PTR) pool_start;
    pool_ptr -> tx_byte_pool_search =  (CHAR_PTR) pool_start;

    /* Initially, the pool will have two blocks.  One large block at the 
       beginning that is available and a small allocated block at the end
       of the pool that is there just for the algorithm.  Be sure to count
       the available block's header in the available bytes count.  */
    pool_ptr -> tx_byte_pool_available =   pool_size - sizeof(VOID_PTR) - sizeof(ULONG);
    pool_ptr -> tx_byte_pool_fragments =    2;
    
    /* Calculate the end of the pool's memory area.  */
    block_ptr =  ((CHAR_PTR) pool_start) + (UINT) pool_size;

    /* Backup the end of the pool pointer and build the pre-allocated block.  */
    block_ptr =  block_ptr - sizeof(ULONG);
    *((ULONG *) block_ptr) =  TX_BYTE_BLOCK_ALLOC;
    block_ptr =  block_ptr - sizeof(CHAR_PTR);
    *((CHAR_PTR *) block_ptr) =  pool_start;

    /* Now setup the large available block in the pool.  */
    *((CHAR_PTR *) pool_start) =  block_ptr;
    block_ptr =  (CHAR_PTR) pool_start;
    block_ptr =  block_ptr + sizeof(CHAR_PTR);
    *((ULONG *) block_ptr) =  TX_BYTE_BLOCK_FREE;

    /* Clear the owner id.  */
    pool_ptr -> tx_byte_pool_owner =  TX_NULL;

    /* Disable interrupts to place the byte pool on the created list.  */
    TX_DISABLE

    /* Setup the byte pool ID to make it valid.  */
    pool_ptr -> tx_byte_pool_id =  TX_BYTE_POOL_ID;

    /* Place the byte pool on the list of created byte pools.  First,
       check for an empty list.  */
    if (_tx_byte_pool_created_ptr)
    {

        /* Pickup tail pointer.  */
        tail_ptr =  _tx_byte_pool_created_ptr -> tx_byte_pool_created_previous;

        /* Place the new byte pool in the list.  */
        _tx_byte_pool_created_ptr -> tx_byte_pool_created_previous =  pool_ptr;
	    tail_ptr -> tx_byte_pool_created_next =  pool_ptr;

        /* Setup this byte pools's created links.  */
        pool_ptr -> tx_byte_pool_created_previous =  tail_ptr;
        pool_ptr -> tx_byte_pool_created_next =      _tx_byte_pool_created_ptr;	
    }
    else
    {

        /* The created byte pool list is empty.  Add byte pool to empty list.  */
        _tx_byte_pool_created_ptr =                  pool_ptr;
        pool_ptr -> tx_byte_pool_created_next =      pool_ptr;
        pool_ptr -> tx_byte_pool_created_previous =  pool_ptr;
    }

    /* Increase the byte pool created count.  */
    _tx_byte_pool_created_count++;

    /* Log this kernel call.  */
    TX_EL_BYTE_POOL_CREATE_INSERT

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

