                            Express Logic's ThreadX for 68332 

                               Using the Green Hills Tools

1.  Installation

ThreadX for the 68332 is delivered on a single CD-ROM compatible disk.  The 
entire distribution can be found in the sub-directory:

\THREADX

To install ThreadX to your hard-disk, make a THREADX\68332\GREEN directory 
on your hard-disk and copy all the contents of the THREADX sub-directory on the 
distribution disk.  The following is an example MS-DOS copy command
(assuming source is D: and C: is your hard-drive):

D:\THREADX>XCOPY /S *.* C:\THREADX\68332\GREEN


2.  Building the ThreadX run-time Library

First make sure you are in the ThreadX directory you have created on 
your hard-drive.  Also, make sure that you have setup your path and other 
environment variables necessary for the Green Hills development environment.  
At this point you may run the BUILD_TX.BAT batch file.  This will 
build the ThreadX run-time environment in the THREADX directory.

C:\THREADX\68332\GREEN> build_tx 

You should observe assembly and compilation of a series of ThreadX source 
files.  At the end of the batch file, they are all combined into the 
run-time library file: TX.OLB.  This file must be linked with your 
application in order to use ThreadX.


3.  Demonstration System

The ThreadX demonstration is designed to execute on the 68332 QUADS evaluation
board and under the MULTI environment using MULTI's 68000 simulator.  The 
instructions that follow will show you how to get the ThreadX demonstration 
running in these environments.

Building the demonstration is easy, simply load the MULTI project file 
DEMO.BLD, which is located inside your ThreadX directory:

C:\THREADX\68332\GREEN\DEMO.BLD

You should observe the following files in the project view area:

DEMO.BLD
   TX_ILL.68
   DEMO.C
   DEMO.LNK
   README.TXT

At this point, select the "Project Build" operation and observe the 
compilation, assembly, and linkage of the ThreadX application. After 
the demonstration is built, you are now ready for debugging!


To run on under MULTI's 68000 simulator, select the "Remote" button 
from the MULTI builder (you should see "s68 68020" in the remote window).  
This invokes MULTI's 68020 simulator and you should observe several new 
windows for it.  Next, select the "Debug" button from the MULTI builder.  
You should now observe the demonstration program in the debugger window.  
At this point, you are free to debug with breakpoints, data watches, etc.
The demonstration system attempts to setup a 68332 periodic timer.   Since
the 68020 simulator does not simulate the 68332 peripherals, the timer 
interrupts will not work under simulation until the following commands
are entered in the simulation control window:

> interrupt dev1 vector 16
> interrupt dev1 priority 2
> interrupt dev1 time 10000 every 10000
> trap 16

To run on the 68332 Evaluation board, simply convert the demo output image
to S-records and download it to the evaluation board.  The following 
command can be used to do the conversion:

> gsrec demo.cfe -o demo.mot

3.1  EventAnalyzer Demonstration

To build a demonstration system that will also log events for the
MULTI EventAnalyzer, perform the same steps as the regular demo, 
except build the ThreadX library with BUILD_TXE.BAT file and use
the DEMO_EL.BLD build file to build the demonstration that logs
all system events.


4.  System Initialization

The system entry point using Green Hills tools is at the label _start.  
This is defined within the CRT0.O file supplied by Green Hills.  In addition, 
this is where all static and global pre-set C variable initialization 
processing is called from.

After the Green Hills startup function returns, ThreadX initialization is
is called.  The 68332 specific initialization is done in the function
_tx_initialize_low_level, which is located in the file TX_ILL.68.  This 
function is responsible for setting up various system data structures, 
interrupt vectors, and a periodic timer interrupt source for ThreadX.

In addition, _tx_initialize_low_level defines the system timer thread’s stack 
and determines the first available address for use by the application.  

By default free memory is assumed to start at the linker defined symbol 
__ghsbegin_freemem.  The ThreadX system timer's stack is allocated here and 
the memory address after the timer thread's stack is passed to the 
application definition function, tx_application_define.


5.  Assembler / Compiler / Linker Switches

The following are Green Hills switches used in building the demonstration 
system:

Compiler/Assembler 					Meaning
    Switches

    -G                  Specifies debug information
    -c                  Specifies object code output
    -68332              Specifies 68332 code generation

There are additional linker commands inside of the file DEMO.LNK.  Please 
use this file as a template for your application.

5.1  User defines

The following defines and their associated action are as follows:

            Define                      Meaning

    TX_DISABLE_ERROR_CHECKING       If defined before tx_api.h is included,
                                    this define causes basic ThreadX error
                                    checking to be disabled.  Please see
                                    Chapter 4 in the "ThreadX User Guide" 
                                    for more details.
    TX_DISABLE_STACK_CHECKING       By default, the thread create function
                                    fills the thread's stack with a 0xEF
                                    data pattern, which is used by the MULTI
                                    debugger to calculate stack usage.  This
                                    can be bypassed by compiling tx_tc.c with
                                    this define.
    TX_ENABLE_EVENT_LOGGING         This define enables event logging for any
                                    or all of the ThreadX source code.  If this
                                    option is used anywhere, the tx_ihl.c file
                                    must be compiled with it as well, since this
                                    is where the event log is initialized.
    TX_NO_EVENT_INFO                This is a sub-option for event logging.
                                    If this is enabled, only basic information
                                    is saved in the log.
    TX_ENABLE_EVENT_FILTERS         This is also a sub-option for event-logging.
                                    If this is enabled, run-time filtering logic
                                    is added to the event logging code.


6.  Register Usage and Stack Frames

The Green Hills 68K compiler assumes that registers d0-d1, and a0-a1 are 
scratch registers for each function.  All other registers used by a C function
must be preserved by the function.  ThreadX takes advantage of this in 
situations where a context switch happens as a result of making a ThreadX 
service call (which is itself a C function).  In such cases, the saved 
context of a thread is only the non-scratch registers.

The following defines the saved context stack frames for context switches
that occur as a result of interrupt handling or from thread-level API calls.
All suspended threads have one of these two types of stack frames.  The top
of the suspended thread's stack is pointed to by tx_stack_ptr in the 
associated thread control block TX_THREAD.



    Offset        Interrupted Stack Frame        Non-Interrupt Stack Frame

     0x00                   1                           0
     0x02                   d0                          d2
     0x06                   d1                          d3
     0x0A                   d2                          d4
     0x0E                   d3                          d5
     0x12                   d4                          d6
     0x16                   d5                          d7
     0x1A                   d6                          a2
     0x1E                   d7                          a3
     0x22                   a0                          a4
     0x26                   a1                          a5
     0x2A                   a2                          a6
     0x2E                   a3                          Saved SR
     0x30                                               unused
     0x32                   a4                          Return PC
     0x36                   a5                          
     0x3A                   a6
     0x3E                   Reserved 6 bytes
     0x44                   Interrupted SR
     0x46                   Interrupted PC


7.  Improving Performance

The distribution version of ThreadX is built without any compiler 
optimizations.  This makes it easy to debug because you can trace or set 
breakpoints inside of ThreadX itself.  Of course, this costs some 
performance.  To make it run faster, you can change the BUILD_TX.BAT file to 
enable all compiler optimizations.  In addition, you can eliminate the 
ThreadX basic API error checking by compiling your application code with the 
symbol TX_DISABLE_ERROR_CHECKING defined.


8.  Interrupt Handling

ThreadX provides complete and high-performance interrupt handling for 68xxx 
targets.  There are a certain set of requirements that are defined in the 
following sub-sections:


8.1  Initial Interrupt Vectors

All ISRs (except for the highest-priority level ISRs) must have their entry 
between the labels: 

__tx_initialize_ISR_start:

my_example_ISR:
    [your ISR processing here]

__tx_initialize_ISR_end:

This is required because a lower-priority interrupt can get interrupted 
before ThreadX knows about it.  Hence, it could get saved as part of an 
executing thread’s context instead of actually being processed once the 
nested interrupt is finished.  It is possible, however, to simply have 
your front-ISR processing in this area simply lockout interrupts and jump 
to the actual ISR processing somewhere else, e.g.:

my_example_ISR_1:

    move.w	%SR,-(%A7)				; Save SR
    ori.w	#$0700,%SR			    ; Lockout interrupts
    jmp	    _my_ISR_processing		; Jump to actual ISR processing


8.2  Managed ISRs

Managed ISRs are ones where ThreadX is performing all of the register 
saving and restoring and thread preemption if necessary.  Such interrupts must
conform to the following template:

my_example_ISR_2:

    move.w	%SR,-(%A7)				    ; Save SR
    ori.w	#$0700,%SR			        ; Lockout interrupts
    jsr	    _tx_thread_context_save     ; Save current system context

    ; Your ISR processing here.  NOTE: Only Green Hills Compiler scratch 
    ;   registers (d0,d1,a0, and a1) can be used in the assembly portion of 
    ;   your ISR.  If your ISR is written in C, the compiler ensures that it 
    ;   does not corrupt any of the preserved registers.  When your ISR 
    ;   completes its processing, it should return and jump to the 
    ;   ThreadX context restore function.

    Jmp	    _tx_thread_context_restore  ; This does not return!


8.3  Fast Interrupts

For high-frequency interrupts, it may be impractical to call context save 
and restore on each interrupt occurrence.  ThreadX supports such ISRs 
providing that they do not corrupt any registers, their entrance is in the 
designated ISR area, and they jump to _tx_thread_preempt_check when 
finished.  The following is a good example of fast ISR processing:

my_fast_ISR:

    ; Perform your fast, assembly language ISR processing being careful to 
    ;   save/restore any registers used.  It is also assumed that this 
    ;   processing is within the ISR area mentioned before.

    move.w  %SR,-(%A7)                  ; Save SR
    ori.w	#$0700,%SR                  ; Lockout interrupts
    jmp	    _tx_thread_preempt_check    ; Check for thread preemption caused 
                                        ;   by higher-priority interrupts 
                                        ;   that might have happened during 
                                        ;   this processing


Of course, if the fast ISR is also the highest-priority, a simple RTE can be 
used in place of the three instruction sequence to check for preemption.


9.  Revision History

12/02/1999  ThreadX update of 68332/Green Hills port and new Generic Code. The
            following files were changed for version G3.0f.3.0a:

            TX_API.H        Added tx_eh_globals field in thread control block for
                            thread safe libraries and added logic to bring in 
                            the event logging constants.
            TX_EL.H         Added file to build for event logging constants.
            TX_GHS.H        Added file to build for thread-safe library support.
            TX_PORT.H       Changed version ID and added stack filling constant.
            TX_EFS.C        Added optimization for setting event flags with only
                            one thread suspended.
            TX_EL.C         Added file to build for event logging constants.
            TX_GHS.C        Added file to library for thread-safe C library support 
                            and stack analysis services.
            TX_ILL.ASM      Added optional event logging code to track interrupts.
            TX_TC.C         Added thread-safe library support, optional
                            stack filling for debugging, and corrected a problem
                            associated with un-initialized thread timer blocks.
            TX_TD.C         Added thread-safe library support.
            TX_TIMIN.ASM    Added optional event logging code to track interrupts.
            TX_TSB.ASM      Changed initial stack pointer to guarantee long word
                            alignment.
            TX_TS.ASM       Removed pad word in solicited context restore.
            TX_TSR.ASM      Added pad word in solicited context frame for long 
                            word stack alignment
            TXE_TMCH.C      Removed code to allow timer change calls from ISRs.
            TXE_EFG.C       Removed code to allow getting event flags from ISRs.
            DEMO.BLD        Modified demo build options.
            DEMO_EL.BLD     Added file for building event logging demonstration.
            DEMO_EL.LNK     Added file for linking event logging demonstration.
            BUILD_AP.BAT    Modified compiler options.
            BUILD_TX.BAT    Added library safe support and changed extensions of
                            assembly files.
            BUILD_TXE.BAT   Added file for event log build.
            TX_*.C          Added event logging to all thread state change 
                            services and to all kernel calls.
            TX*.C           Changed comments and copyright header.
            TX*.H           Changed comments and copyright header.
            TX*.ASM         Changed comments, copyright header, and changed
                            extensions to .68 to utilize the Green Hills preprocessor
                            for conditional event log logic.

09/07/1999  Initial ThreadX version for 68332 using Green Hills.


Copyright(c) 1996-2000 Express Logic, Inc.


Express Logic, Inc.
11440 West Bernardo Court
Suite 366
San Diego, CA  92127

www.expresslogic.com
