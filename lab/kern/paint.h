#ifndef JOS_KERN_PAINT_H
#define JOS_KERN_PAINT_H

#ifndef JOS_KERNEL
# error "This is a JOS kernel header; user programs should not #include it"
#endif

#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

typedef uint8_t COLOR;

void set_vga_base(uint8_t *b);

uint8_t* xy_to_base(int x, int y); 

void paint_point(int x, int y, COLOR c);
void paint_char(int x, int y, char ch, COLOR c);
void paint_rect(int x, int y, int w, int h, COLOR c);

#endif
