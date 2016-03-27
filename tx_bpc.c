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
/**   Block Pool (BLO)                                                    */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define TX_SOURCE_CODE


/* Include necessary system files.  */

#include    "tx_api.h"
#include    "tx_blo.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_block_pool_create                               PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function creates a pool of fixed-size memory blocks in the     */ 
/*    specified memory area.                                              */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    pool_ptr                          Pointer to pool control block     */ 
/*    name_ptr                          Pointer to block pool name        */ 
/*    block_size                        Number of bytes in each block     */ 
/*    pool_start                        Address of beginning of pool area */ 
/*    pool_size                         Number of bytes in the block pool */ 
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
/*  11-11-1997     William E. Lamie         Corrected problem with block  */ 
/*                                            pools that are evenly       */ 
/*                                            divisible by the block size */ 
/*                                            and modified comment(s),    */ 
/*                                            resulting in version 3.0b.  */ 
/*  01-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0e.  */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            added logic to track events,*/ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
UINT    _tx_block_pool_create(TX_BLOCK_POOL *pool_ptr, CHAR *name_ptr, ULONG block_size,
                    VOID *pool_start, ULONG pool_size)
{

TX_INTERRUPT_SAVE_AREA

TX_BLOCK_POOL   *tail_ptr;                  /* Working block pool pointer  */
ULONG           blocks;                     /* Number of blocks in pool    */
CHAR_PTR        block_ptr;                  /* Working block pointer       */
CHAR_PTR        next_block_ptr;             /* Next block pointer          */
CHAR_PTR        end_of_pool;                /* End of pool area            */


    /* Round the block size up to something that is evenly divisable by
       an ULONG.  This helps gaurantee proper alignment.  */
    block_size =  ((block_size + sizeof(ULONG) - 1)/sizeof(ULONG)) * sizeof(ULONG);

    /* Round the pool size down to something that is evenly divisable by 
       an ULONG.  */
    pool_size =   (pool_size/sizeof(ULONG)) * sizeof(ULONG);

    /* Setup the basic block pool fields.  */
    pool_ptr -> tx_block_pool_name =             name_ptr;
    pool_ptr -> tx_block_pool_suspension_list =  TX_NULL;
    pool_ptr -> tx_block_pool_suspended_count =  0;
    pool_ptr -> tx_block_pool_start =            (CHAR_PTR) pool_start;
    pool_ptr -> tx_block_pool_size =             pool_size;
    pool_ptr -> tx_block_pool_block_size =       block_size;
    
    /* Calculate the end of the pool's memory area.  */
    end_of_pool =  ((CHAR_PTR) pool_start) + (UINT) pool_size;

    /* Walk through the pool area, setting up the available block list.  */
    blocks =            0;
    block_ptr =         (CHAR_PTR) pool_start;
    next_block_ptr =    block_ptr + (UINT) (block_size + sizeof(CHAR_PTR));
    while(next_block_ptr <= end_of_pool)
    {

        /* Yes, we have another block.  Increment the block count.  */
        blocks++;

        /* Setup the link to the next block.  */
        *((CHAR_PTR *) block_ptr) =  next_block_ptr;

        /* Advance to the next block.  */
        block_ptr =   next_block_ptr;

        /* Update the next block pointer.  */
        next_block_ptr =  block_ptr + (UINT) (block_size + sizeof(CHAR_PTR));
    }

    /* Backup to the last block in the pool.  */
    block_ptr =  block_ptr - (UINT) (block_size + sizeof(CHAR_PTR));

    /* Set the last block's forward pointer to NULL.  */
    *((CHAR_PTR *) block_ptr) =  TX_NULL;

    /* Save the remaining information in the pool control block.  */
    pool_ptr -> tx_block_pool_available =  blocks;
    pool_ptr -> tx_block_pool_total =      blocks;

    /* Quickly check to make sure at least one block is in the pool.  */
    if (blocks)
        pool_ptr -> tx_block_pool_available_list =  (CHAR_PTR) pool_start;
    else
        pool_ptr -> tx_block_pool_available_list =  TX_NULL;
    
    /* Disable interrupts to place the block pool on the created list.  */
    TX_DISABLE

    /* Setup the block pool ID to make it valid.  */
    pool_ptr -> tx_block_pool_id =  TX_BLOCK_POOL_ID;

    /* Place the block pool on the list of created block pools.  First,
       check for an empty list.  */
    if (_tx_block_pool_created_ptr)
    {

        /* Pickup tail pointer.  */
        tail_ptr =  _tx_block_pool_created_ptr -> tx_block_pool_created_previous;

        /* Place the new block pool in the list.  */
        _tx_block_pool_created_ptr -> tx_block_pool_created_previous =  pool_ptr;
	    tail_ptr -> tx_block_pool_created_next =  pool_ptr;

        /* Setup this block pools's created links.  */
        pool_ptr -> tx_block_pool_created_previous =  tail_ptr;
        pool_ptr -> tx_block_pool_created_next =      _tx_block_pool_created_ptr;	
    }
    else
    {

        /* The created block pool list is empty.  Add block pool to empty list.  */
        _tx_block_pool_created_ptr =                  pool_ptr;
        pool_ptr -> tx_block_pool_created_next =      pool_ptr;
        pool_ptr -> tx_block_pool_created_previous =  pool_ptr;
    }

    /* Increment the number of block pools created.  */
    _tx_block_pool_created_count++;

    /* Log this kernel call.  */
    TX_EL_BLOCK_POOL_CREATE_INSERT

    /* Restore interrupts.  */
    TX_RESTORE

    /* Return TX_SUCCESS.  */
    return(TX_SUCCESS);
}

