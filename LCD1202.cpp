
#include "LCD1202.h"

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
   spi[SPI_CS] |= 0x04;          // First SCLK transition at beginning of data bit.

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
   spi[SPI_CS] |= 0x80;
   spi[SPI_FIFO] = 0xe2;   // Reset
   spi[SPI_FIFO] = 0xa4;   // Power saver off
   spi[SPI_FIFO] = 0x2f;   // Power control set

   spi[SPI_FIFO] = 0xb0;   // Page address set
   spi[SPI_FIFO] = 0xaf;   // LCD on

   // Need a small delay here to give the LCD time to initialise
   // otherwise we end up with a patchy display.
   usleep(1);

   spi[SPI_CS] &= ~0x80;
}

void LCD1202::lcd_write_byte(unsigned data, bool cmd)
{
   spi[SPI_CS] |= 0x80;
   unsigned int d = data;
   if (!cmd) {
      d |= 0x100;
   }
   spi[SPI_FIFO] = d;
   usleep(1);
//   spi[SPI_CS] &= ~0x80;
}

void LCD1202::all_points_on()
{
   lcd_write_byte(0xa5, 1);   // All points on
}

void LCD1202::all_points_off()
{
   lcd_write_byte(0xa4, 1);   // All points off
}

void LCD1202::print_A()
{
   /* Print 'A' character */
   lcd_write_byte(0x00);
   lcd_write_byte(0x7c);
   lcd_write_byte(0x12);
   lcd_write_byte(0x12);
   lcd_write_byte(0x7c);
   lcd_write_byte(0x00);
   usleep(1);
   spi[SPI_FIFO] = 0xaf;   // LCD on
}
