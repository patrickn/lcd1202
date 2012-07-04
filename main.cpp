
#include <iostream>
#include "LCD1202.h"

using namespace std;

int main(int argc, char** argv)
{
   LCD1202 lcd;

   lcd.clear_screen();
   lcd.write(3, 0, "++++++++");
//
//    lcd.write(5, 1, "++++++++");
//
//    lcd.write(7, 1, "1 + 1 = ");
//    lcd.write(7, 4, "2");

   lcd.write(1, 0, "aaaaaaa");

//    lcd.point_on(3, 3);
//    lcd.point_on(2, 3);
//    lcd.point_on(1, 3);

   lcd.write(3, 3, "X");

   lcd.update_screen();
   //lcd.draw_line(4, 0, 6, 0);

   return 0;
}
