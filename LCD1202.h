
#ifndef __LCD1202_H__
#define __LCD1202_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>


namespace constants
{
   // RPI constants
   const unsigned BCM2708_PERI_BASE = 0x20000000;
   const unsigned GPIO_BASE         = (BCM2708_PERI_BASE + 0x200000); // GPIO controller
   const unsigned SPI_BASE          = (GPIO_BASE + 0x4000);           // SPI controller
   const unsigned PAGE_SIZE         = (4 * 1024);
   const unsigned BLOCK_SIZE        = (4 * 1024);

   // LCD constants
   const unsigned MAX_X = 96;
   const unsigned MAX_Y = 68;
   const unsigned MAX_ROWS = 9; // 9 rows of bytes, last row only half used

   const double RADTODEG = 57.29578;
}


class LCD1202
{
public:
   LCD1202();
   virtual ~LCD1202();
   void display_info();

   unsigned max_x() const { return constants::MAX_X; };
   unsigned max_y() const { return constants::MAX_Y; };

   // Drawing methods
   void write(unsigned, unsigned, const char*);
   void goto_xy(unsigned, unsigned);
   void set_point(unsigned, unsigned);
   void clear_point(unsigned, unsigned);

   // Circles
   void draw_circle(int, int, int, bool=true, bool=false);

   // Lines
   void draw_line(int, int, int, int);
   void clear_line(int, int, int, int);
   void draw_vertical_line(unsigned, unsigned, unsigned);
   void clear_vertical_line(unsigned, unsigned, unsigned);
   void draw_horizontal_line(unsigned, unsigned, unsigned);
   void clear_horizontal_line(unsigned, unsigned, unsigned);

   void draw_vector(int, int, bool=true);

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
   void cpts8(int, int, int, int, bool);
   void cpts4(int, int, int, int, bool);

   // Circles
   void draw_filled_circle(int, int, int, int, bool=true);

   // I/O access
   volatile unsigned* spi;
   unsigned char frame_buffer[constants::MAX_ROWS][constants::MAX_X];

   // SPI Registers
   enum {SPI_CS, SPI_FIFO, SPI_CLK, SPI_DLEN, SPI_LTOH, SPI_DC};
};


#endif
