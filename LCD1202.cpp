
#include "LCD1202.h"
#include "version.h"
#include "font.h"


// Todo: Use static const here
#define BCM2708_PERI_BASE  0x20000000
#define GPIO_BASE          (BCM2708_PERI_BASE + 0x200000) // GPIO controller
#define SPI_BASE           (GPIO_BASE + 0x4000)           // SPI controller
#define PAGE_SIZE          (4 * 1024)
#define BLOCK_SIZE         (4 * 1024)

static const char* info1 = "RPi ~ LCD1202";
static const char* info2 = version;
static const char* info3 = "by @neavey";

enum spi_regs {SPI_CS, SPI_FIFO, SPI_CLK, SPI_DLEN, SPI_LTOH, SPI_DC};


LCD1202::LCD1202()
{
   spi = get_mmap_ptr(SPI_BASE, BLOCK_SIZE);

   // Pi SPI Initialisation
   spi[SPI_CS] = 0x30 | 0x10000; // Clear FIFOs and all status bits
   spi[SPI_CS] = 0x10000;        // Make sure done bit is cleared

   spi[SPI_CLK] = 250;           // 250 == 1MHz

   spi[SPI_CS] |= 0x2000;        // Enable LoSSI
   //spi[SPI_CS] |= 0x04;          // First SCLK transition at beginning of data bit.

   init();
}

LCD1202::~LCD1202()
{
}

volatile unsigned* LCD1202::get_mmap_ptr(unsigned pos, unsigned len)
{
   int mem_fd;
   void* ptr;

   // open /dev/mem
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      // Todo: Throw an exception
      exit(-1);
   }

   // mmap GPIO

   // Allocate MAP block
   if ((ptr = malloc(len + (PAGE_SIZE-1))) == NULL) {
      printf("allocation error \n");
      // Todo: Throw an exception
      exit(-1);
   }

   // Make sure pointer is on 4K boundary
   if ((unsigned long)ptr % PAGE_SIZE) {
      ptr += PAGE_SIZE - ((unsigned long)ptr % PAGE_SIZE);
   }

   // Now map it
   ptr = (unsigned char*)mmap(
      (caddr_t)ptr,
      len,
      PROT_READ|PROT_WRITE,
      MAP_SHARED|MAP_FIXED,
      mem_fd,
      pos
   );

   if ((long)ptr < 0) {
      printf("mmap error %ld\n", (long)ptr);
      // Todo: Throw an exception
      exit(-1);
   }

   close (mem_fd);
   // Always use volatile pointer!
   return (volatile unsigned *)ptr;
}

void LCD1202::init()
{
   write_cmd(0xe2);   // Reset
   usleep(10);
   write_cmd(0xa4);   // Power saver off
   write_cmd(0x2f);   // Power control set

   //write_cmd(0xa6);   // Display normal
   display_info();
   sleep(1);
}

void LCD1202::display_info()
{
   clear_screen();
   write(0 , 0, info1);
   write(16, 0, info2);
   write(32, 0, info3);
   update_screen();
}

void LCD1202::write_byte(unsigned data, bool cmd)
{
   // Make sure the Tx buffer is empty
   while (0 == (spi[SPI_CS] & 0x40000));

   spi[SPI_CS] |= 0x80;
   unsigned int d = data;
   if (!cmd) {
      d |= 0x100;
   }
   spi[SPI_FIFO] = d;
}

void LCD1202::all_points_on()
{
   write_cmd(0xa5);   // All points on
}

void LCD1202::all_points_off()
{
   write_cmd(0xa4);   // All points off
}

void LCD1202::clear_screen()
{
   memset(frame_buffer, 0, sizeof(frame_buffer));
}

void LCD1202::set_point(unsigned x, unsigned y)
{
   if ((x < MAX_X) && (y < MAX_Y))
   {
      frame_buffer[y / 8][x] |= 1 << (y & 0x07);
   }
}

void LCD1202::clear_point(unsigned x, unsigned y)
{
   if ((x < MAX_X) && (y < MAX_Y))
   {
      frame_buffer[y / 8][x] &= ~(1 << (y & 0x07));
   }
}

void LCD1202::update_screen()
{
   goto_xy(0, 0);

   for (unsigned char r = 0; r < MAX_ROWS; r++) {
      for (unsigned char c = 0; c < MAX_X; c++) {
         write_data(frame_buffer[r][c]);
      }
   }
   write_cmd(0xaf);
}

// The column positioning does not work on the LCD for some unknown reason.
// Looking at the original code used as the base for this version, column
// positioning was never used.
void LCD1202::goto_xy(unsigned x, unsigned y)
{
   write_cmd(0xb0 | ((y) & 0x0f)); // Set page address to 'y'
   write_cmd(0x10 | (x >> 4));     // Sets DDRAM column address - upper 3 bits
   write_cmd(0x00 | (x & 0x0f));   // lower 4 bits
}

void LCD1202::write(unsigned r, unsigned c, const char* str)
{
   int row = r >> 3;

   for ( ; *str; str++) {
      int d = (*str - ' ') * 5;

      for (int i = 0; i < 5; i++, d++) {
         frame_buffer[row][c++] = font_data[d];
      }

      frame_buffer[row][c++] = 0;
   }
}

/////////////////////////////////////////////////////////////////////////////
// draw_line
// Draw a line between any two absolute co-ords
//
void LCD1202::draw_line(unsigned x1, unsigned y1, unsigned x2, unsigned y2)
{
   // Bresenham's line drawing algorithm. Originally coded on the IBM PC
   // with EGA card in 1986.
   unsigned x, y, d, dx, dy, i1, i2;
   unsigned xend, yend, xinc, yinc;

   dx = abs(x2 - x1);
   dy = abs(y2 - y1);

   if (((y1 > y2) && (dx < dy)) || ((x1 > x2) && (dx > dy))) {
      unsigned temp = y1;
      y1 = y2;
      y2 = temp;

      temp = x1;
      x1 = x2;
      x2 = temp;
   }

   if (dy > dx) {
      d = (2 * dx) - dy;     /* Slope > 1 */
      i1 = 2 * dx;
      i2 = 2 * (dx - dy);

      if (y1 > y2) {
         x = x2;
         y = y2;
         yend = y1;
      }
      else
      {
         x = x1;
         y = y1;
         yend = y2;
      }

      if (x1 > x2)
         xinc = -1;
      else
         xinc = 1;

      set_point(x, y);

      while (y < yend) {
         y++;
         if (d < 0)
            d += i1;
         else {
            x += xinc;
            d += i2;
      }

      set_point(x, y);
      }
   }
   else {
      d = (2 * dy) - dx;  /* Slope < 1 */
      i1 = 2 * dy;
      i2 = 2 * (dy - dx);

      if (x1 > x2) {
         x = x2;
         y = y2;
         xend = x1;
      }
      else {
         x = x1;
         y = y1;
         xend = x2;
      }

      if (y1 > y2)
         yinc = -1;
      else
         yinc = 1;

      set_point(x, y);

      while (x < xend) {
         x++;
         if (d < 0)
            d += i1;
         else {
            y += yinc;
            d += i2;
      }

      set_point(x, y);
      }
   }
}

// circle --- draw a circle with edge and fill colours
//
//
// Params:
//       unsigned x0:   x coordinate
//       unsigned y0:   y coordinate
//       unsigned r :   Circle radius
//       bool border:   Draw a border
//       bool fill  :   Fill circle
//
// For the 1202 LCD the only valid edge and fill colours are 1 or 0, for on or off.
// Colour is not supported.
void LCD1202::draw_circle(int x0, int y0, int r, bool border, bool fill)
{
   // Michener's circle algorithm. Originally coded on the IBM PC
   // with EGA card in 1986.
   int x, y;
   int d;

   x = 0;
   y = r;
   d = 3 - (2 * r);

   if (fill) {
      while (x < y) {
         draw_filled_circle(x0, y0, x, y, fill);
         if (d < 0) {
            d += (4 * x) + 6;
         }
         else {
            d += (4 * (x - y)) + 10;
            y--;
         }
         x++;
      }

      if (x == y)
         draw_filled_circle(x0, y0, x, y, fill);
   }

   x = 0;
   y = r;
   d = 3 - (2 * r);

   while (x < y) {
      cpts8(x0, y0, x, y, border);
      if (d < 0) {
         d += (4 * x) + 6;
      }
      else {
         d += (4 * (x - y)) + 10;
         y--;
      }
      x++;
   }

   if (x == y)
      cpts8(x0, y0, x, y, border);
}

// draw_filled_circle --- draw horizontal lines to fill a circle
void LCD1202::draw_filled_circle(int x0, int y0, int x, int y, bool fill)
{
   if (fill) {
      draw_horizontal_line(x0 - x, x0 + x, y0 + y);
      draw_horizontal_line(x0 - x, x0 + x, y0 - y);
      draw_horizontal_line(x0 - y, x0 + y, y0 + x);
      draw_horizontal_line(x0 - y, x0 + y, y0 - x);
   }
   else {
      clear_horizontal_line(x0 - x, x0 + x, y0 + y);
      clear_horizontal_line(x0 - x, x0 + x, y0 - y);
      clear_horizontal_line(x0 - y, x0 + y, y0 + x);
      clear_horizontal_line(x0 - y, x0 + y, y0 - x);
   }
}

// cpts8 --- draw eight pixels to form the edge of a circle
void LCD1202::cpts8(int x0, int y0, int x, int y, bool border)
{
   cpts4(x0, y0, x, y, border);

// if (x != y)
   cpts4(x0, y0, y, x, border);
}

// cpts4 --- draw four pixels to form the edge of a circle
void LCD1202::cpts4(int x0, int y0, int x, int y, bool border)
{
   if (border) {
      set_point(x0 + x, y0 + y);

//  if (x != 0)
      set_point(x0 - x, y0 + y);

//  if (y != 0)
      set_point(x0 + x, y0 - y);

//  if ((x != 0) && (y != 0))
      set_point(x0 - x, y0 - y);
   }
   else {
      clear_point(x0 + x, y0 + y);

//  if (x != 0)
      clear_point(x0 - x, y0 + y);

//  if (y != 0)
      clear_point(x0 + x, y0 - y);

//  if ((x != 0) && (y != 0))
      clear_point(x0 - x, y0 - y);
   }
}

// draw_vertical_line --- draw vertical line
void LCD1202::draw_vertical_line(unsigned x, unsigned y1, unsigned y2)
{
   for (unsigned y = y1; y <= y2; y++)
      set_point(x, y);
}

// clear_vertical_line --- draw vertical line
void LCD1202::clear_vertical_line(unsigned x, unsigned y1, unsigned y2)
{
   for (unsigned y = y1; y <= y2; y++)
      clear_point(x, y);
}

// draw_horizontal_line --- set pixels in a horizontal line
void LCD1202::draw_horizontal_line(unsigned x1, unsigned x2, unsigned y)
{
   unsigned row = y / 8;
   unsigned char b = 1 << (y & 7);

   for (unsigned x = x1; x <= x2; x++)
      frame_buffer[row][x] |= b;
}

// clear_horizontal_line --- clear pixels in a horizontal line
void LCD1202::clear_horizontal_line(unsigned x1, unsigned x2, unsigned y)
{
   unsigned row = y / 8;
   unsigned char b = ~(1 << (y  & 7));

   for (unsigned x = x1; x <= x2; x++)
      frame_buffer[row][x] &= b;
}
