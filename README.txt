This directory contains:
-- scripts to build a PDP-11 GCC cross compiler (version 13), binutils,
   and newlib.
-- A bare metal shell that can be downloaded (via PDP11GUI for example)
   to a PDP-11 that has a CPU, memory, and a serial console. I
   tested it using a KDJ11-DS PDP-11/53 CPU board.

The shell implements the following commands:
-- dump
-- modify
-- fill
-- thread test
-- help (type ?)

It has a command line editor with a four-line history buffer.

I build and run the toolchain in a WSL Debian instance on Windows 11, but
this should work on any Linux machine.

The contents of the directory include:

-- build-13.sh:     This script will download, build, and install GCC 13
                    for PDP-11. Before running it, examine the script
                    and make sure the paths are correct for your environment.

-- build-newlib.sh: This script will download, build, and install an
                    experimental version of newlib from
                    github.com/cptnapalm/newlib-pdp11.git. The original author
                    has not touched this project since 2018. I have only used
                    a few headers plus ctype from this newlib, but what
                    little I have used seems to work. Check the paths in
                    the script before running it. (This newlib contains
                    errors, see instructions below for how to fix them.)

-- MyLib:           This contains tiny replacements for a few libc
                    functions needed by the shell. The script build.sh
                    will build MyLib.a.

-- Sprintf:         This contains an LGPL licensed sprintf used by the
                    shell. Due to the LGPL license it is kept in a
                    separate directory.

-- include:         Include files for the shell.

-- shell:           Source files for the bare metal shell.


Derivative subdirs:

-- cross:           This will contain the sources and output files for GCC.
                    After installing the compiler you can delete this.

-- build:           This will contain the source and output files for newlib.
                    After installing newlib you can delete the build directory.

-- pdp11-aout-13:   This is the install directory for the cross compiler and newlib.




Installation:

SETUP
=====

Clone this repository to where you want it to live. I use $HOME/PDP-11.

Edit build-13.sh and build-newlib.sh to make sure the paths are appropriate for your environment.


GCC
===

Run build-13.sh. This will build and install the GCC cross compiler.

Set your PATH to include the PDP-11 compiler binaries, e.g.
PATH=$HOME/PDP-11/pdp11-aout-13/bin:$PATH
I put that line in my .profile. Building newlib assumes the PDP-11 cross compiler is on the PATH.


NEWLIB
======

Run the following command (copied from build-newlib.sh, where it is commented out):
git clone https://github.com/cptnapalm/newlib-pdp11.git

As of February 2024 this newlib repo exists in a dirty state, and the
author has not touched it since 2018. Several files contain git merge
conflicts that cause compile errors. These will have to be located and
fixed.

Run the following command:
grep -r "<<<<<" newlib-pdp11

I get the following result:

newlib-pdp11/newlib/libc/ctype/towctrans_l.c:<<<<<<< HEAD
newlib-pdp11/newlib/libc/ctype/categories.c:<<<<<<< HEAD
grep: newlib-pdp11/.git/objects/pack/pack-77dded11c4400af7bcb5446b314c9cd677f71909.pack: binary file matches
newlib-pdp11/libgloss/configure:<<<<<<< HEAD
newlib-pdp11/libgloss/configure:<<<<<<< HEAD
newlib-pdp11/libgloss/configure.in:<<<<<<< HEAD

For each of these files (except for the binary) you need to find one or more git conflict sections with the
following template:

    <<<<<<< HEAD
    the right stuff -- leave it
    =======
    the wrong stuff, delete it
    >>>>>>> dbd872f4ad8ad40bd47d8ee63cd64acc89682c31
    
For each instance, delete the markers (the lines with the <<< === and >>>),
and the code between the ==== and the >>>>.

Once that is done you can run build-newlib.sh. Ignore warnings about the target system not supporting debug.

TODO -- I should clone this newlib repo and fix the issues.


MYLIB
=====

cd to MyLib and run build.sh. This creates MyLib.a.


SHELL
=====

cd to the shell subdirectory and type make. This will build the shell.

I use PDP11GUI under Windows 11 to download main.bin to a bare PDP-11/53 CPU board.
Use its Memory Loader with a path like this: \\wsl.localhost\Debian\home\engdahl\PDP-11\shell\main.bin
You will need to "Load" the file first, then "Deposit all". It will take a while, even at 
38400 baud.

Since the PDP11GUI terminal window is dumber than a Teletype, I then
switch over to a TeraTerm window to run the shell, because the shell's
command line editor uses VT100 arrow keys.


The start address of the shell is 01000 (octal). Telling ODT to start there should give you this:
(the bear looks a lot better in TeraTerm)

@1000g

                ;iIVYti=;;;;:... :;;
            :iRBMMMMMMMMMMMMMMMMMMMMMBI=.
          ;RMMMMMMMMMMMMMMMMMMMMMMMMMMMMMRt:i.   .
        .VMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMB;IBB.
       .BMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMB
       RMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMR
       RMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMR
       BMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM=
      iMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMBMMMMMMMMMBi
     iBMMMMMMMMMMMMMBBVBBRMMMMMMRBBMMMMMB.:RRi+=tBMBY.
    RMMMMMMMMMMMMMMI     iMMMMB;  .RMMMMMi        ~
  ;BMMMMMBX=BMMMMMt      RMMMB:     RMMMMB
 BMMMBBI;   BMMMMt       BMMMt       ;BMMMV
 BMMB       BMMMR        BMMB         :BMMM;
 .BMMRt;    BMMMBIt=.    RMMB;;        .VMMBXYi.
     __   ___       __            ___ ___
    |__) |__   /\  |__)     |\/| |__   |   /\  |
    |__) |___ /~~\ |  \     |  | |___  |  /~~\ |___

hello, world!
build: Feb 17 2024 15:18:17
>?
Addresses, data, and sizes are generally in hex.
d <addr> <size>}                dump memory
m <addr> <data>...              modify memory
f <addr> <size> <data>          fill memory
>

Type ? to get the help screen.


THREADS
=======
A bare metal threading system has been implemented for the PDP-11. A thread is declared as:

    int thread(int arg1, int arg2);

This following typedef makes it easier to refer to thread functions:

    typedef int THREADFUNCTION(int arg1, int arg2);

You can pass two arguments to the thread when it is spawned. This enables you to distinguish
multiple threads that are running the same code. If a thread does not require arguments 
dummy values such as zero can be passed.

Calling spawn creates a new thread. The stack pointer should point to one past the end of an
array of int, which will serve as the thread's stack. Make sure the stack is large enough
to handle all functions that the thread might call, plus some extra for interrupts. There is
no check for stack overflow.

    void spawn(THREADFUNCTION *code, int *stack_pointer, int arg1, int arg2);     

A PORT is a pointer to a chain of threads which are waiting for an event. In the case of a PORT,
the chain is last-in-first-out (LIFO). This is probably not fair, but it is fast and simple.
PORTs are probably best suited for cases where there are one or very few threads which might
wait for an event. It is possible to implement other PORT-like objects which have FIFO
behavior, though they will have higher overhead.

    typedef int *PORT;

The wait function suspends a thread at a PORT until it is waked by another thread calling
wake. The wait function can return an argument passed from wake.

    int wait(PORT *port);

The wake function wakes a thread which is suspended at a PORT. The argument passed to the wake call
is returned from the corresponding wait, thus one thread can wait for a message, and a second thread
can wake the first thread and pass a message to it by calling wake. The call to wake returns 1 if
there was a thread waiting at the port and the message was passed, or 0 if there was no thread waiting
at the port and the message was not passed. In the latter case the calling thread must ensure that
the message is not lost; the PORT will not queue the message.

    int wake(PORT *port, int arg);

If a thread completes its task and wishes to terminate, it returns from the thread function. The value
returned can be retrieved by another thread which is waiting for the thread to terminate. Two functions
are used to rendezvous with a terminating thread.

The function rendezvous_test takes as an argument the initial stack pointer that was passed when the
thread was spawned. It returns 1 if the thread has terminated, or 0 if the thread is still running.
If this function is called before the thread is spawned or after the thread has been re-spawned the
result is indeterminate.

    int rendezvous_test(int *stack_pointer);

After a thread has terminated, rendezvous_return_value can be used to obtain the value returned from the
terminated thread. Like rendezvous_test, it takes the thread's stack pointer as an argument.

    int rendevous_return_value(int *stack_pointer);

The rendezvous functions use the last word of the stack as a flag to signal thread termination, and
the second to last word to return the value. The spawn function clears the flag before the thread is
started. Spawn passes the arguments to the new thread using the second and third from last words of
the thread's stack. The fourth from last word contains the return address which will point to the
code that handles winding down the thread and resuming the next pending thread.

A thread which spawns or wakes another thread is suspended and placed at the top of an internal
pending chain. When a thread waits at a PORT, or terminates, it resumes the next pending thread
from the top of the pending chain.


CREDITS
=======

The GCC build script was derived from https://xw.is/wiki/Bare_metal_PDP-11_GCC_9.3.0_cross_compiler_instructions

The PDP-11 newlib is from https://github.com/cptnapalm/newlib-pdp11

PDP11GUI is from https://github.com/j-hoppe/PDP11GUI
Its documentation is at https://www.retrocmp.com/tools/pdp11gui

sprintf was derived from https://www.menie.org/georges/embedded/small_printf_source_code.html

MyLib and the shell were ported from my STM32H503 project at https://github.com/jrengdahl/H503

