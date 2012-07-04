
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

   init_lcd();
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

void LCD1202::init_lcd()
{
   lcd_write_cmd(0xe2);   // Reset
   usleep(10);
   lcd_write_cmd(0xa4);   // Power saver off
   lcd_write_cmd(0x2f);   // Power control set

   //lcd_write_cmd(0xa6);   // Display normal
   display_info();
   sleep(1);
}

void LCD1202::display_info()
{
   clear_screen();
   write(0, 0, info1);
   write(1, 0, info2);
   write(2, 0, info3);
   lcd_write_cmd(0xaf);
}

void LCD1202::lcd_write_byte(unsigned data, bool cmd)
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
   lcd_write_cmd(0xa5);   // All points on
}

void LCD1202::all_points_off()
{
   lcd_write_cmd(0xa4);   // All points off
}

void LCD1202::goto_xy(unsigned x, unsigned y)
{
   lcd_write_cmd(0xb0 | ((y) & 0x0f)); // Set page address to 'y'
   lcd_write_cmd(0x10 | (x >> 4));   // Sets DDRAM column address - upper 3 bits
   lcd_write_cmd(0x00 | (x & 0x0f)); // lower 4 bits
}

void LCD1202::goto_rc(unsigned r, unsigned c)
{
   lcd_write_cmd(0xb0 | (r & 0x0f)); // Set page address to 'r'
   lcd_write_cmd(0x10 | (c >> 4));   // Sets DDRAM column address - upper 3 bits
   lcd_write_cmd(0x00 | (c & 0x0f)); // lower 4 bits
}

void LCD1202::clear_screen()
{
   goto_rc(0, 0);

   for (int i = 0; i < 16 * 6 * 9; i++) {
      // fill DDRAM with Zeros
      lcd_write_data(0x00);
   }
   lcd_write_cmd(0xaf);
}

void LCD1202::write(unsigned r, unsigned c, const char* str)
{
   goto_rc(r, c);

   for ( ; *str; str++) {
      int d = (*str - ' ') * 5;

      for (int i = 0; i < 5; i++, d++) {
         lcd_write_data(font_data[d]);
      }
   }
//   lcd_write_cmd(0xaf);
}

void LCD1202::point_on(unsigned x, unsigned y)
{
   goto_xy(x, y);
   lcd_write_data(0x01);
//   lcd_write_cmd(0xaf);
}

void LCD1202::point_off(unsigned x, unsigned y)
{
   goto_xy(x, y);
   lcd_write_data(0xff);
//   lcd_write_cmd(0xaf);
}

void LCD1202::set_pixel(unsigned, unsigned)
{

}

void LCD1202::update_screen()
{
   lcd_write_cmd(0xaf);
}

/////////////////////////////////////////////////////////////////////////////
// draw_line
// Draw a line between any two absolute co-ords
//
void LCD1202::draw_line(int x1, int y1, int x2, int y2)
{
   // Bresenham's line drawing algorithm. Originally coded on the IBM PC
   // with EGA card in 1986.
   int x, y, d, dx, dy, i1, i2;
   int xend, yend, xinc, yinc;

   dx = abs(x2 - x1);
   dy = abs(y2 - y1);

   if (((y1 > y2) && (dx < dy)) || ((x1 > x2) && (dx > dy))) {
      int temp = y1;
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

      point_on(x, y);

      while (y < yend) {
         y++;
         if (d < 0)
            d += i1;
         else {
            x += xinc;
            d += i2;
      }

      point_on(x, y);
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

      point_on(x, y);

      while (x < xend) {
         x++;
         if (d < 0)
            d += i1;
         else {
            y += yinc;
            d += i2;
      }

      point_on(x, y);
      }
   }
}
