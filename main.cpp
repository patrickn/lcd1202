
#include <iostream>
#include <ctime>

#include "LCD1202.h"


unsigned centre_x = 48, centre_y = 34;

void draw_background(LCD1202& lcd)
{
   lcd.write(8, 43, "12");
   lcd.write(32, 72, "3");
   lcd.write(58, 48, "6");
   lcd.write(32, 20, "9");
   lcd.draw_circle(centre_x, centre_y, 33);
   lcd.update_screen();
}

int main(int argc, char** argv)
{
   LCD1202 lcd;

   time_t t;
   struct tm* now;
   unsigned hour;

   while (1) {
      t = time(0);
      now = localtime(&t);
      lcd.clear_screen();
      draw_background(lcd);

      hour = (now->tm_hour >= 13 ? now->tm_hour - 12 : now->tm_hour);
      lcd.draw_vector((((hour)/12.0)*360.0) - 90, 14);
      lcd.draw_vector((((now->tm_min)/60.0)*360.0) - 90, 22);
      lcd.draw_vector((((now->tm_sec)/60.0)*360.0) - 90, 25);
      lcd.update_screen();
      sleep(1);
   }

   return 0;
}
