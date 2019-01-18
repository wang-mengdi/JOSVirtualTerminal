#include <kern/consol.h>
#include <kern/paint.h>

#define VGABIAS 0xa0000
#define VGABASE ((uint8_t) KERNBASE + VGABIAS)

uint8_t xy_to_base(int x, int y) {
	return (uint8_t) VGABASE + x * VGA_WIDTH + y;
}

void paint_point(int x, int y, COLOR c) {
	uint8_t *i = xy_to_base(x, y);
	*i = c;
}
