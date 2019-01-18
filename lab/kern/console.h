/* See COPYRIGHT for copyright information. */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#include <inc/types.h>

#define MONO_BASE	0x3B4
#define MONO_BUF	0xB0000
#define CGA_BASE	0x3D4
#define CGA_BUF		0xB8000

#define CRT_ROWS	25
#define CRT_COLS	80
#define CRT_SIZE	(CRT_ROWS * CRT_COLS)

/***** VGA ports *****/

#define VGA_GRAPHIC_BUF 0xA0000

// External registers
#define EXT_INPUT0_READ 0x3C2
#define EXT_MISC_READ 0x3CC
#define EXT_MISC_WRITE 0x3C2
#define ATTR_ADDR_DATA_PORT 0x3C0
#define ATTR_DATA_READ_PORT 0x3C1
#define INPUT_STAT1 0x3DA

// Sequencer, graphics, and CRT controller registers
#define SEQ_ADDR_PORT 0x3C4
#define SEQ_DATA_PORT 0x3C5
#define GC_ADDR_PORT 0x3CE
#define GC_DATA_PORT 0x3CF
#define CRTC_ADDR_PORT 0x3D4
#define CRTC_DATA_PORT 0x3D5

#define VGA_WIDTH 320
#define VGA_HEIGHT 200
#define VGA_SIZE (VGA_WIDTH*VGA_HEIGHT)

void cons_init(void);
int cons_getc(void);

void kbd_intr(void); // irq 1
void serial_intr(void); // irq 4

#endif /* _CONSOLE_H_ */
