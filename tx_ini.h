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
/**   Initialization (INI)                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/


/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    tx_ini.h                                            PORTABLE C      */ 
/*                                                           3.0f         */ 
/*  AUTHOR                                                                */ 
/*                                                                        */ 
/*    William E. Lamie, Express Logic, Inc.                               */ 
/*                                                                        */ 
/*  DESCRIPTION                                                           */ 
/*                                                                        */ 
/*    This file defines the ThreadX initialization component, including   */ 
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
/*  01-01-1999     William E. Lamie         Modified comments and added   */ 
/*                                            prototype for compiler's    */ 
/*                                            early kernel setup function,*/ 
/*                                            added new initialization    */ 
/*                                            state constant, resulting   */ 
/*                                            in version 3.0e.            */ 
/*  11-01-1999     William E. Lamie         Modified comment(s),          */ 
/*                                            resulting in version 3.0f.  */ 
/*                                                                        */ 
/**************************************************************************/ 

#ifndef  TX_INI
#define  TX_INI


/* Define constants that indicate initialization is in progress.  */

#define	 TX_INITIALIZE_IN_PROGRESS	    0xF0F0F0F0UL
#define  TX_INITIALIZE_ALMOST_DONE      0xF0F0F0F1UL
#define  TX_INITIALIZE_IS_FINISHED      0x00000000UL


/* Define initialization function prototypes.  */

VOID        _tx_initialize_kernel_enter(VOID);
VOID        _tx_initialize_kernel_setup(VOID);
VOID        _tx_initialize_low_level(VOID);
VOID        _tx_initialize_high_level(VOID);


/* Initialization component data declarations follow.  */

/* Determine if the initialization function of this component is including
   this file.  If so, make the data definitions really happen.  Otherwise,
   make them extern so other functions in the component can access them.  */

#ifdef  TX_INITIALIZE_INIT
#define INITIALIZE_DECLARE 
#else
#define INITIALIZE_DECLARE extern
#endif


/* Define the unused memory pointer.  The value of the first available 
   memory address is placed in this variable in the low-level
   initialization function.  The content of this variable is passed 
   to the application's system definition function.  */

INITIALIZE_DECLARE VOID_PTR    _tx_initialize_unused_memory;


#endif
