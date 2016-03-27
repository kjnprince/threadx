/*
		Language Independent Library

	Copyright 1983-1996 Green Hills Software,Inc.

    This program is the property of Green Hills Software, Inc,
    its contents are proprietary information and no part of it
    is to be disclosed to anyone except employees of Green Hills
    Software, Inc., or as agreed in writing signed by the President
    of Green Hills Software, Inc.
*/

#include "tx_ghs.h"
#define TX_DISABLE_ERROR_CHECKING
#include "tx_api.h"
#include <setjmp.h>
#include <string.h>

extern TX_THREAD *_tx_thread_created_ptr;
extern TX_THREAD *_tx_thread_current_ptr;


/* This implementation merely tries to guarantee thread safety within
   individual C library calls such as malloc() and free(), but it does
   not attempt to solve the problems associated with the following
   multithreaded issues:

   1. Use of errno.  This can be made thread-safe by a Premium (full-source)
      user of ThreadX by adding errno to TX_THREAD_PORT_EXTENSION and using
      that within a modified version of libsrc/ind_errno.c.
   
   2. Thread safety ACROSS library calls.  Certain C library calls by their
      very design maintain state across or after calls.  These include
      strtok(), asctime(), gmtime(), tmpnam(NULL), signal().  To make
      this thread-safe, you have to add a ThreadLocalStorage struct to
      each thread.  That seems like overkill for ThreadX

   3. C++ exception handling.  This requires one void *__eh_globals
      per thread.
   */

/* GlobalTLS is a global structure to use for thread local storage if we
   don't have multi-threading.  If you want actual thread local storage,
   you'll have to add a ThreadLocalStorage struct to TX_THREAD, and
   return it in GetThreadLocalStorage based on the current thread.

   The global _tx_thread_current_ptr is a pointer to the current thread
   structure, so if you place a ThreadLocalStorage struct called tx_tls
   in TX_THREAD, the function GetThreadLocalStorage can return
   &(_tx_thread_current_ptr->tx_tls).
 */

ThreadLocalStorage GlobalTLS;

ThreadLocalStorage *GetThreadLocalStorage()
{
    return &GlobalTLS;
}

/********************************************************
   Locks require a recursive mutex.  This is a data structure that
   may be generally useful, so I'll define it here.  Feel free to
   use this in your code, although none of these calls can be made
   from interrupt service routines.

   I haven't made these fully general by creating both error-checking
   and non-error-checking versions.  If you compile this into your
   code, it will assume whatever the 

   This is a good example of how ThreadX objects can be wrapped
   to create other useful objects, depending on your application.

   Do not change the order of any statements in tx_mutex_get or
   tx_mutex_put.

   This data structure should migrate to a ThreadX-specific version
   of ind_thrd.h so it can be used in ind_lockcpp.c as well.

 */

typedef struct TX_MUTEX_STRUCT
{
    struct TX_SEMAPHORE_STRUCT tx_mutex_semaphore;
    TX_THREAD                 *tx_mutex_owner;
    ULONG                      tx_mutex_count;
} TX_MUTEX;

UINT        tx_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr);
UINT        tx_mutex_delete(TX_MUTEX *mutex_ptr);
UINT        tx_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option);
UINT        tx_mutex_put(TX_MUTEX *mutex_ptr);

UINT        tx_mutex_create(TX_MUTEX *mutex_ptr, CHAR *name_ptr) {
    /* Create a semaphore with an initial count of 1. */
    mutex_ptr->tx_mutex_owner=0;
    mutex_ptr->tx_mutex_count=0;
    return tx_semaphore_create((TX_SEMAPHORE *)mutex_ptr, name_ptr, 1);
}

UINT        tx_mutex_delete(TX_MUTEX *mutex_ptr) {
    return tx_semaphore_delete((TX_SEMAPHORE *)mutex_ptr);
}

UINT        tx_mutex_get(TX_MUTEX *mutex_ptr, ULONG wait_option) {
    UINT retval;
    if(mutex_ptr->tx_mutex_owner == tx_thread_identify()) {
	(mutex_ptr->tx_mutex_count)++;
	return TX_SUCCESS;
    } else {
	retval = tx_semaphore_get((TX_SEMAPHORE *)mutex_ptr, wait_option);
	if(retval == TX_SUCCESS) {
	    mutex_ptr->tx_mutex_owner = tx_thread_identify();
	    mutex_ptr->tx_mutex_count = 1;
	}
	return retval;
    }
}

UINT        tx_mutex_put(TX_MUTEX *mutex_ptr) {
    UINT retval;
    /* error checking would do this:
    if(mutex_ptr->tx_mutex_owner != _tx_thread_current_ptr)
	return TX_SEMAPHORE_ERROR;     (Or some other nonzero error value)
    */
    if(--(mutex_ptr->tx_mutex_count) == 0) {
	mutex_ptr->tx_mutex_owner = 0;
	return tx_semaphore_put((TX_SEMAPHORE *)mutex_ptr);
    }
    return TX_SUCCESS;
}

/*******************************************************/
TX_MUTEX __ghLockMutex;


void __ghsLock(void)
{
    tx_mutex_get(&__ghLockMutex, TX_WAIT_FOREVER);
}

void __ghsUnlock(void)
{
    tx_mutex_put(&__ghLockMutex);
}

int __ghs_SaveSignalContext(jmp_buf jmpbuf)
{
    return 0;
}

void __ghs_RestoreSignalContext(jmp_buf jmpbuf)
{
}

/* prototype */
void _tx_initialize_kernel_setup(void);

void __gh_lock_init(void)
{
    /* Initialize the low-level portions of ThreadX. */
    _tx_initialize_kernel_setup();
    tx_mutex_create(&__ghLockMutex, "__ghLockMutex");
}

/* Must get called after __cpp_except_init() is called to allocate 
 * and initialize the per-thread exception handling structure */
void *__get_eh_globals(void) 
{
    /* Return thread-specific __eh_globals instead of the global
       one. */
    if (_tx_thread_current_ptr)
        return(_tx_thread_current_ptr->tx_eh_globals);
    else
        return(GlobalTLS.__eh_globals);
}

void __ghs_flock_file(void *addr) {}
void __ghs_funlock_file(void *addr) {}
int __ghs_ftrylock_file(void *addr) { return -1; }
void __ghs_flock_create(void **addr) {}
void __ghs_flock_destroy(void *addr) {}

/******************************************************************************\
*                                                                             *
*                                    txs_tc.c                                 *
*                             ThreadX Stack Checking                          *
*                                                                             *
*	                           Copyright 1998                             *
*                        Green Hills Software, Inc.                           *
*                                                                             *
*          This program is the property of Green Hills Software, Inc.	      *
*                                                                             *
*    This program contains confidential and proprietary information of        *
*    Green Hills Software, Inc.  No part of this program is to be disclosed   *
*    to anyone other than employees of Green Hills Software, Inc., or as      *
*    agreed in writing signed by the President of Green Hills Software, Inc.  *
*                                                                             *
\******************************************************************************/

ULONG _txs_thread_stack_check(TX_THREAD *thread_ptr) {
    char *i;
/* Needs to be changed for i960 */
    for(i = thread_ptr->tx_stack_start;i <= thread_ptr->tx_stack_end;++i ) {
	if (*i != (char)TX_STACK_FILL) {
	    return (((ULONG)thread_ptr->tx_stack_end) - (ULONG)i + 1);
	}
    }
    return thread_ptr->tx_stack_size;
}


int _txs_thread_stack_check_2(void) {
    char *i;
    int tx_continue;
    TX_THREAD *t;
    for(t=_tx_thread_created_ptr, tx_continue=1; tx_continue;) {
	/* Needs to be changed for i960 */
	for(i = t->tx_stack_start;
		i <= t->tx_stack_end;++i ) {
	    if (*i != (char)TX_STACK_FILL) {
		t->tx_stack_size = ((ULONG)t->tx_stack_end) - (ULONG)i + 1;
		break;
	    }
	}
	t=t->tx_created_next;
	if (t == _tx_thread_created_ptr)
	    tx_continue=0;
    }
    return 0;
}

int _txs_thread_stack_check_2_fixup(void) {
    int tx_continue;
    TX_THREAD *t;
    for(t=_tx_thread_created_ptr, tx_continue=1; tx_continue;) {
	t->tx_stack_size = (ULONG)t->tx_stack_end-(ULONG)t->tx_stack_start+1;
	t=t->tx_created_next;
	if (t == _tx_thread_created_ptr)
            tx_continue=0;
    }
    return 0;
}
