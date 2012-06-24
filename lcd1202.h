
#ifndef __LCD1202_H__
#define __LCD1202_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


class lcd1202
{
public:
   lcd1202();
   virtual ~lcd1202();

   void all_points_on();
   void all_points_off();

private:
   void init_lcd();
   volatile unsigned* get_mmap_ptr(unsigned, unsigned);
   void lcd_write_byte(unsigned, bool);

   // I/O access
   volatile unsigned *gpio, *spi;
};


#endif
