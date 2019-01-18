#include <inc/memlayout.h>

#include <kern/console.h>
#include <kern/paint.h>

#define VGABIAS 0xa0000
#define VGABASE ((uint8_t*) KERNBASE + VGABIAS)

#define CHAR_WIDTH 5
#define CHAR_HEIGHT 8

const bool char_lattice[1][40] = { 
{ // 0: exclamation
0, 0, 0, 0, 0,
0, 0, 1, 0, 0,
0, 0, 1, 0, 0,
0, 0, 1, 0, 0,
0, 0, 1, 0, 0,
0, 0, 0, 0, 0,
0, 0, 1, 0, 0,
0, 0, 0, 0, 0
}
};

uint8_t* xy_to_base(int x, int y) {
	return (uint8_t *) VGABASE + x * VGA_WIDTH + y;
}

void paint_point(int x, int y, COLOR c) {
	uint8_t *i = xy_to_base(x, y);
	*i = c;
}

void paint_char(int x, int y, int ch, COLOR c) {
	int i, j;
	for (i = 0; i < CHAR_HEIGHT; ++i)
		for (int j = 0; j < CHAR_WIDTH; ++j) {
			if (char_lattice[ch][i * CHAR_WIDTH + j])
				paint_point(x + i, y + j, c);
		}
}
