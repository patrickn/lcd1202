
#ifndef __LCD1202_H__
#define __LCD1202_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define LCD_MAX_X 96
#define LCD_MAX_Y 68
#define LCD_MAX_ROWS 9 // 9 rows of bytes, last row only half used

class LCD1202
{
public:
   LCD1202();
   virtual ~LCD1202();

   void all_points_on();
   void all_points_off();
   void goto_xy(unsigned, unsigned);
   void goto_rc(unsigned, unsigned);
   void clear_screen();
   void display_info();

   // Direct writes
   void write(unsigned, unsigned, const char*);
   void point_on(unsigned, unsigned);
   void point_off(unsigned, unsigned);
   void draw_line(int, int, int, int);
   void update_screen();

   void set_pixel(unsigned, unsigned);

private:
   void init_lcd();
   volatile unsigned* get_mmap_ptr(unsigned, unsigned);
   void lcd_write_byte(unsigned, bool);
   void lcd_write_cmd(unsigned val) {lcd_write_byte(val, true);};
   void lcd_write_data(unsigned val){lcd_write_byte(val, false);};

   // I/O access
   volatile unsigned* spi;
};


#endif
