
#include <iostream>
#include "LCD1202.h"

using namespace std;

int main(int argc, char** argv)
{
   LCD1202 lcd;
   lcd.clear_screen();

   unsigned centre_x = 48, centre_y = 34;

   lcd.write(8, 43, "12");
   lcd.write(32, 72, "3");
   lcd.write(58, 48, "6");
   lcd.write(32, 20, "9");
   lcd.draw_circle(centre_x, centre_y, 33);
   lcd.update_screen();

   lcd.draw_line(4, 0, 6, 0);

   return 0;
}
