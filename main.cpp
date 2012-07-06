
#include <iostream>
#include "LCD1202.h"

using namespace std;

int main(int argc, char** argv)
{
   LCD1202 lcd;
   lcd.clear_screen();

   lcd.write(32, 32, "12");
   lcd.write(40, 40, "3");
   lcd.write(48, 48, "6");
   lcd.write(56, 56, "9");
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
