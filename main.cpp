
#include <iostream>
#include "LCD1202.h"

using namespace std;

int main(int argc, char** argv)
{
   LCD1202 lcd;

   lcd.clear_screen();
//   lcd.write(3, 0, "++++++++");
//
//    lcd.write(5, 1, "++++++++");
//
//    lcd.write(7, 1, "1 + 1 = ");
//    lcd.write(7, 4, "2");

   lcd.write(0 , 0, "Aa");
   lcd.write(8 , 8, "Bb");
   lcd.write(16, 16, "Cc");
   lcd.write(24, 24, "Dd");
   lcd.write(32, 32, "Ee");
   lcd.write(40, 40, "Ff");
   lcd.write(48, 48, "Gg");
   lcd.write(56, 56, "Hh");
//   lcd.draw_line(0, 0, 60, 60);
   lcd.draw_circle(96 / 2, 68 / 2, 33);
   lcd.update_screen();
//   lcd.draw_circle(96 / 2, 68 / 2, 33, 0, 1);

//    lcd.set_point(3, 3);
//    lcd.set_point(2, 3);
//    lcd.point_on(1, 3);

//   lcd.write(3, 3, "X");

//   lcd.update_screen();
   //lcd.draw_line(4, 0, 6, 0);

   return 0;
}
