
#include "LCD1202.h"
#include "font.h"


#define BCM2708_PERI_BASE  0x20000000
#define GPIO_BASE          (BCM2708_PERI_BASE + 0x200000) // GPIO controller
#define SPI_BASE           (GPIO_BASE + 0x4000)           // SPI controller
#define PAGE_SIZE          (4 * 1024)
#define BLOCK_SIZE         (4 * 1024)


enum spi_regs {SPI_CS, SPI_FIFO, SPI_CLK, SPI_DLEN, SPI_LTOH, SPI_DC};



LCD1202::LCD1202()
{
   spi = get_mmap_ptr(SPI_BASE, BLOCK_SIZE);

   // SPI Initialisation
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
      exit (-1);
   }

   // mmap GPIO

   // Allocate MAP block
   if ((ptr = malloc(len + (PAGE_SIZE-1))) == NULL) {
      printf("allocation error \n");
      exit (-1);
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
      exit (-1);
   }

   close (mem_fd);
   // Always use volatile pointer!
   return (volatile unsigned *)ptr;
}

void LCD1202::init_lcd()
{
   lcd_write_cmd(0xe2);   // Reset
   lcd_write_cmd(0xa4);   // Power saver off
   lcd_write_cmd(0x2f);   // Power control set

   //lcd_write_cmd(0xb0);   // Page address set
   //lcd_write_cmd(0xaf);   // LCD on
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

// void LCD1202::print_A()
// {
//    clear_screen();
// //    goto_rc(8, 0);
// //
// //    for (int i = 0; i < 2; i++) {
// //       // Print 'A' character
// //       lcd_write_data(0x00);
// //       lcd_write_data(0x7c);
// //       lcd_write_data(0x12);
// //       lcd_write_data(0x12);
// //       lcd_write_data(0x7c);
// //       lcd_write_data(0x00);
// //    }
// //
// //    write_text(1, 2, "=");
//
//    write_text(15, 23, "000");
//
//
//    goto_xy(2, 3);
//
//    for (int i = 0; i < 3; i++) {
//       // Print 'A' character
//       lcd_write_data(0x00);
//       lcd_write_data(0x7c);
//       lcd_write_data(0x12);
//       lcd_write_data(0x12);
//       lcd_write_data(0x7c);
//       lcd_write_data(0x00);
//    }
//
// //   update_screen();
//    lcd_write_cmd(0xaf);
// }

void LCD1202::write_text(int x, int y, const char* str)
{
   int row = y >> 3;

   for ( ; *str; str++) {
      int d = (*str - ' ') * 5;

      for (int i = 0; i < 5; i++, d++) {
         frame_buffer[row][x++] = font_data[d];
      }

      frame_buffer[row][x++] = 0;
   }
}

void LCD1202::update_screen(void)
{
   goto_xy(0, 0);

   for (unsigned char r = 0; r < LCD_MAX_ROWS; r++) {
      for (unsigned char c = 0; c < LCD_MAX_X; c++) {
         lcd_write_data(frame_buffer[r][c]);
      }
   }
}

void LCD1202::goto_xy(int x, int y)
{
   lcd_write_cmd(0xb0 | (x & 0x0f)); // Set page address to 'x'
   lcd_write_cmd(0x10 | (y >> 4));   // Sets DDRAM column address - upper 3 bits
   lcd_write_cmd(0x00 | (y & 0x0f)); // lower 4 bits
}

void LCD1202::clear_screen()
{
   goto_xy(0, 0);

   for (int i = 0; i < 16 * 6 * 9; i++) {
      // fill DDRAM with Zeros
      lcd_write_data(0x00);
   }
}

void LCD1202::clear_frame(void)
{
   memset(frame_buffer, 0, sizeof(frame_buffer));
}

void LCD1202::print_direct(int x, int y, const char*)
{
   goto_xy(x, y);

   for (int i = 0; i < 3; i++) {
      // Print 'A' character
      lcd_write_data(0x00);
      lcd_write_data(0x7c);
      lcd_write_data(0x12);
      lcd_write_data(0x12);
      lcd_write_data(0x7c);
      lcd_write_data(0x00);
   }

   lcd_write_cmd(0xaf);
}

void LCD1202::print(int x, int y, const char* str)
{
   clear_frame();
   write_text(x, y, str);
   update_screen();
   lcd_write_cmd(0xaf);
}
