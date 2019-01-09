/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

#define TRAPHANDLERALL(name, num)   \
    .data;                              \
        .long name;                     \
    .text;                              \
        .globl name;                    \
        .type name,@function;           \
        .align 2;                       \
        name:                           \
            .if !(num==8||num==17||(num>=10&&num<=14));     \
            pushl $0;                   \
            .endif;                     \
            pushl $(num);               \
            jmp _alltraps


/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */


.data
.globl vectors

vectors:


    TRAPHANDLERALL(th0, 0)
    TRAPHANDLERALL(th1, 1)
    TRAPHANDLERALL(th2, 2)
    TRAPHANDLERALL(th3, 3)
    TRAPHANDLERALL(th4, 4)
    TRAPHANDLERALL(th5, 5)
    TRAPHANDLERALL(th6, 6)
    TRAPHANDLERALL(th7, 7)
    TRAPHANDLERALL(th8, 8)
    TRAPHANDLERALL(th9, 9)
    TRAPHANDLERALL(th10, 10)
    TRAPHANDLERALL(th11, 11)
    TRAPHANDLERALL(th12, 12)
    TRAPHANDLERALL(th13, 13)
    TRAPHANDLERALL(th14, 14)
    TRAPHANDLERALL(th15, 15)
    TRAPHANDLERALL(th16, 16)
    TRAPHANDLERALL(th17, 17)
    TRAPHANDLERALL(th18, 18)
    TRAPHANDLERALL(th19, 19)
    TRAPHANDLERALL(th20, 20)
    TRAPHANDLERALL(th21, 21)
    TRAPHANDLERALL(th22, 22)
    TRAPHANDLERALL(th23, 23)
    TRAPHANDLERALL(th24, 24)
    TRAPHANDLERALL(th25, 25)
    TRAPHANDLERALL(th26, 26)
    TRAPHANDLERALL(th27, 27)
    TRAPHANDLERALL(th28, 28)
    TRAPHANDLERALL(th29, 29)
    TRAPHANDLERALL(th30, 30)
    TRAPHANDLERALL(th31, 31)
    TRAPHANDLERALL(th32, 32)
    TRAPHANDLERALL(th33, 33)
    TRAPHANDLERALL(th34, 34)
    TRAPHANDLERALL(th35, 35)
    TRAPHANDLERALL(th36, 36)
    TRAPHANDLERALL(th37, 37)
    TRAPHANDLERALL(th38, 38)
    TRAPHANDLERALL(th39, 39)
    TRAPHANDLERALL(th40, 40)
    TRAPHANDLERALL(th41, 41)
    TRAPHANDLERALL(th42, 42)
    TRAPHANDLERALL(th43, 43)
    TRAPHANDLERALL(th44, 44)
    TRAPHANDLERALL(th45, 45)
    TRAPHANDLERALL(th46, 46)
    TRAPHANDLERALL(th47, 47)
    TRAPHANDLERALL(th48, 48)

/*
TRAPHANDLER_NOEC(t_divide, T_DIVIDE)        // 0 divide error
TRAPHANDLER_NOEC(t_debug, T_DEBUG)          // 1 debug exception
TRAPHANDLER_NOEC(t_nmi, T_NMI)              // 2 non-maskable interrupt
TRAPHANDLER_NOEC(t_brkpt, T_BRKPT)          // 3 breakpoint
TRAPHANDLER_NOEC(t_oflow, T_OFLOW)          // 4 overflow
TRAPHANDLER_NOEC(t_bound, T_BOUND)          // 5 bounds check
TRAPHANDLER_NOEC(t_illop, T_ILLOP)          // 6 illegal opcode
TRAPHANDLER_NOEC(t_device, T_DEVICE)        // 7 device not available
TRAPHANDLER(t_dblflt, T_DBLFLT)             // 8 double fault
                                            // 9 reserved
TRAPHANDLER(t_tss, T_TSS)                   // 10 invalid task switch segment
TRAPHANDLER(t_segnp, T_SEGNP)               // 11 segment not present
TRAPHANDLER(t_stack, T_STACK)               // 12 stack exception
TRAPHANDLER(t_gpflt, T_GPFLT)               // 13 general protection fault
TRAPHANDLER(t_pgflt, T_PGFLT)               // 14 page fault
                                            // 15 reserved
TRAPHANDLER_NOEC(t_fperr, T_FPERR)          // 16 floating point error
TRAPHANDLER(t_align, T_ALIGN)               // 17 aligment check
TRAPHANDLER_NOEC(t_mchk, T_MCHK)            // 18 machine check
TRAPHANDLER_NOEC(t_simderr, T_SIMDERR)      // 19 SIMD floating point error

TRAPHANDLER_NOEC(t_syscall, T_SYSCALL)      // 48 system call
*/


/*
 * Lab 3: Your code here for _alltraps
 */
.text
_alltraps:
    pushl %ds
    pushl %es
    pushal

    movw $GD_KD, %ax
    movw %ax, %ds
    movw %ax, %es

    pushl %esp
    call trap
