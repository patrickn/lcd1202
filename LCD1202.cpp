
#include "LCD1202.h"
#include "version.h"
#include "font.h"


// Todo: Use static const here
#define BCM2708_PERI_BASE  0x20000000
#define GPIO_BASE          (BCM2708_PERI_BASE + 0x200000) // GPIO controller
#define SPI_BASE           (GPIO_BASE + 0x4000)           // SPI controller
#define PAGE_SIZE          (4 * 1024)
#define BLOCK_SIZE         (4 * 1024)

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
   lcd_write_cmd(0xaf);

   //lcd_write_cmd(0xa1);   // Display flipped
   //lcd_write_cmd(0x80 | 16); // Set VLCD Voltage

   lcdClr();
}

void LCD1202::lcd_write_byte(unsigned data, bool cmd)
{
   // Make sure the Tx buffer is empty
   while (0 == (spi[SPI_CS] & 0x40000));
//
   spi[SPI_CS] |= 0x80;
   unsigned int d = data;
   if (!cmd) {
      d |= 0x100;
   }
   spi[SPI_FIFO] = d;
}

void LCD1202::lcdClr(void)
{
   int i;
   lcdHome();
   for(i = 0; i < 16 * 6 * 9; i++)
   {
      lcd_write_data(0x00); // fill DDRAM with Zeros
   }
}

void LCD1202::lcdHome(void)
{
   lcd_write_cmd(0xB0);
   lcd_write_cmd(0x10);
   lcd_write_cmd(0x00);
}

void LCD1202::lcdRow(char r)
{
   lcd_write_cmd(0xB0 | (r & 0x0F));
}

void LCD1202::lcdCol(char c)
{
   lcd_write_cmd(0x10 | (c >> 4));     // Sets the DDRAM column addr - upper 3-bit
   lcd_write_cmd(0x00 | (c & 0x0F));   // lower 4-bit
}

void LCD1202::xputs(const char* s, char line)
{
	lcdHome();
	lcdRow(line);
	while(*s)
	{
		xputc(*s++);
	}
	lcd_write_cmd(0xaf);
}

void LCD1202::xputc(char r)
{
   char i;
   int d = r - ' ';
   d *= 5;

   for (i = 0; i < 5; i++)
   {
      lcd_write_data(font_data[d++]);
   }

   lcd_write_data(0); // 1px gap after each letter
}
