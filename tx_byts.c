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
#include    "tx_thr.h"
#include    "tx_byt.h"


/**************************************************************************/ 
/*                                                                        */ 
/*  FUNCTION                                               RELEASE        */ 
/*                                                                        */ 
/*    _tx_byte_pool_search                                PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This function searches a byte pool for a memory block to satisfy    */ 
/*    the requested number of bytes.  Merging of adjacent free blocks     */ 
/*    takes place during the search and a split of the block that         */ 
/*    satisfies the request may occur before this function returns.       */ 
/*                                                                        */ 
/*    It is assumed that this function is called with interrupts enabled  */ 
/*    and with the tx_pool_owner field set to the thread performing the   */ 
/*    search.  Also note that the search can occur during allocation and  */ 
/*    release of a memory block.                                          */ 
/*                                                                        */ 
/*  INPUT                                                                 */ 
/*                                                                        */ 
/*    pool_ptr                          Pointer to pool control block     */ 
/*    memory_size                       Number of bytes required          */ 
/*                                                                        */ 
/*  OUTPUT                                                                */ 
/*                                                                        */ 
/*    CHAR_PTR                          Pointer to the allocated memory,  */ 
/*                                        if successful.  Otherwise, a    */ 
/*                                        NULL is returned                */ 
/*                                                                        */ 
/*  CALLS                                                                 */ 
/*                                                                        */ 
/*    None                                                                */ 
/*                                                                        */ 
/*  CALLED BY                                                             */ 
/*                                                                        */ 
/*    _tx_byte_allocate                 Allocate bytes of memory          */ 
/*    _tx_byte_release                  Release bytes of memory           */ 
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
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 
VOID    *_tx_byte_pool_search(TX_BYTE_POOL *pool_ptr, ULONG memory_size)
{

TX_INTERRUPT_SAVE_AREA

REG_1 CHAR_PTR  current_ptr;                /* Current block pointer      */
REG_2 CHAR_PTR  next_ptr;                   /* Next block pointer         */
REG_3 ULONG     available_bytes;            /* Calculate bytes available  */
REG_4 ULONG     examine_blocks;             /* Blocks to be examined      */


    /* Disable interrupts.  */
    TX_DISABLE

    /* First, determine if there are enough bytes in the pool.  */
    if (memory_size >= pool_ptr -> tx_byte_pool_available)
    {

        /* Restore interrupts.  */
        TX_RESTORE

        /* Not enough memory, return a NULL pointer.  */
        return(TX_NULL);
    }

    /* Walk through the memory pool in search for a large enough block.  */
    current_ptr =      pool_ptr -> tx_byte_pool_search;
    examine_blocks =   pool_ptr -> tx_byte_pool_fragments + 1;
    available_bytes =  0;
    do
    {

        /* Check to see if this block is free.  */
        if (*((ULONG *) (current_ptr + sizeof(CHAR_PTR))) == TX_BYTE_BLOCK_FREE)
        {

            /* Block is free, see if it is large enough.  */

            /* Pickup the next block's pointer.  */
            next_ptr =  *((CHAR_PTR *) current_ptr);

            /* Calculate the number of byte available in this block.  */
            available_bytes =  next_ptr - current_ptr - sizeof(CHAR_PTR) - sizeof(ULONG);

            /* If this is large enough, we are done because our first-fit algorithm
               has been satisfied!  */
            if (available_bytes >= memory_size)
            {
                /* Get out of the search loop!  */
                break;
            }
            else
            {

                /* Clear the available bytes variable.  */
                available_bytes =  0;

                /* Not enough memory, check to see if the neighbor is 
                   free and can be merged.  */
                if (*((ULONG *) (next_ptr + sizeof(CHAR_PTR))) == TX_BYTE_BLOCK_FREE)
                {

                    /* Yes, neighbor block can be merged!  This is quickly accomplished
                       by updating the current block with the next blocks pointer.  */
                    *((CHAR_PTR *) current_ptr) =  *((CHAR_PTR *) next_ptr);

                    /* Reduce the fragment total.  We don't need to increase the bytes
                       available because all free headers are also included in the available
                       count.  */
                    pool_ptr -> tx_byte_pool_fragments--;

                    /* See if the search pointer is affected.  */
                    if (pool_ptr -> tx_byte_pool_search ==  next_ptr)
                        pool_ptr -> tx_byte_pool_search =  current_ptr;
                }
                else
                {

                    /* Neighbor is not free so we can skip over it!  */
                    current_ptr =  *((CHAR_PTR *) next_ptr);

                    /* Decrement the examined block count to account for this one.  */
                    if (examine_blocks)
                        examine_blocks--;
                }
            }
        }
        else
        {

            /* Block is not free, move to next block.  */
            current_ptr =      *((CHAR_PTR *) current_ptr);
        } 

        /* Another block has been searched... decrement counter.  */
        if (examine_blocks)
            examine_blocks--;

        /* Restore interrupts temporarily.  */
        TX_RESTORE

        /* Disable interrupts.  */
        TX_DISABLE

        /* Determine if anything has changed in terms of pool ownership.  */
        if (pool_ptr -> tx_byte_pool_owner != _tx_thread_current_ptr)
        {

            /* Pool changed ownership in the brief period interrupts were
               enabled.  Reset the search.  */
            current_ptr =      pool_ptr -> tx_byte_pool_search;
            examine_blocks =   pool_ptr -> tx_byte_pool_fragments + 1;

            /* Setup our ownership again.  */
            pool_ptr -> tx_byte_pool_owner =  _tx_thread_current_ptr;
        }
    } while(examine_blocks);

    /* Determine if a block was found.  If so, determine if it needs to be
       split.  */
    if (available_bytes)
    {

        /* Determine if we need to split this block.  */
        if ((available_bytes - memory_size) >= ((ULONG) TX_BYTE_BLOCK_MIN))
        {

            /* Split the block.  */
            next_ptr =  current_ptr + memory_size + sizeof(CHAR_PTR) + sizeof(ULONG);

            /* Setup the new free block.  */
            *((CHAR_PTR *) next_ptr) =  *((CHAR_PTR *) current_ptr);
            *((ULONG *) (next_ptr + sizeof(CHAR_PTR))) =  TX_BYTE_BLOCK_FREE;

            /* Increase the total fragment counter.  */
            pool_ptr -> tx_byte_pool_fragments++;

            /* Update the current pointer to point at the newly created block.  */
            *((CHAR_PTR *) current_ptr) = next_ptr;

            /* Set available equal to memory size for subsequent calculation.  */
            available_bytes =  memory_size;
        }

        /* In any case, mark the current block as allocated.  */
        *((TX_BYTE_POOL **) (current_ptr + sizeof(CHAR_PTR))) =  pool_ptr;

        /* Reduce the number of available bytes in the pool.  */
        pool_ptr -> tx_byte_pool_available =  pool_ptr -> tx_byte_pool_available - 
                                            available_bytes - sizeof(CHAR_PTR) - sizeof(ULONG);

        /* Restore interrupts.  */
        TX_RESTORE

        /* Adjust the pointer for the application.  */
        current_ptr =  current_ptr + sizeof(CHAR_PTR) + sizeof(ULONG);
    }
    else
    {

        /* Restore interrupts.  */
        TX_RESTORE

        /* Set current pointer to NULL to indicate nothing was found.  */
        current_ptr =  TX_NULL;
    }

    /* Return the search pointer.  */
    return(current_ptr);
}

