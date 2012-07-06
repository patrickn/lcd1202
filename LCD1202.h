
#ifndef __LCD1202_H__
#define __LCD1202_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define MAX_X 96
#define MAX_Y 68
#define MAX_ROWS 9 // 9 rows of bytes, last row only half used

class LCD1202
{
public:
   LCD1202();
   virtual ~LCD1202();
   void display_info();

   // Drawing methods
   void write(unsigned, unsigned, const char*);
   void goto_xy(unsigned, unsigned);

   // Circles
   void draw_circle(int, int, int, bool=true, bool=false);
   void draw_filled_circle(int, int, int, int, bool=true);

   // Lines
   void draw_line(unsigned, unsigned, unsigned, unsigned);
   void clear_line(unsigned, unsigned, unsigned, unsigned);
   void draw_vertical_line(unsigned, unsigned, unsigned);
   void clear_vertical_line(unsigned, unsigned, unsigned);
   void draw_horizontal_line(unsigned, unsigned, unsigned);
   void clear_horizontal_line(unsigned, unsigned, unsigned);

   // Frame buffer writes
   void clear_screen();
   void update_screen();

   // Direct writes
   void all_points_on();
   void all_points_off();

private:
   void init();
   volatile unsigned* get_mmap_ptr(unsigned, unsigned);

   void write_byte(unsigned, bool);
   void write_cmd(unsigned val) {write_byte(val, true);};
   void write_data(unsigned val){write_byte(val, false);};

   // Drawing methods
   void set_point(unsigned, unsigned);
   void clear_point(unsigned, unsigned);
   void cpts8(int, int, int, int, bool);
   void cpts4(int, int, int, int, bool);

   // I/O access
   volatile unsigned* spi;
   unsigned char frame_buffer[MAX_ROWS][MAX_X];
};


#endif
