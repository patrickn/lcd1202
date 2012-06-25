
#ifndef __LCD1202_H__
#define __LCD1202_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


class LCD1202
{
public:
   LCD1202();
   virtual ~LCD1202();

   void all_points_on();
   void all_points_off();
   void print_A();

private:
   void init_lcd();
   volatile unsigned* get_mmap_ptr(unsigned, unsigned);
   void lcd_write_byte(unsigned, bool = false);

   // I/O access
   volatile unsigned* spi;
};


#endif
