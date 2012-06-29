
#include <iostream>
#include "LCD1202.h"

using namespace std;

int main(int argc, char** argv)
{
   LCD1202 lcd;

//   lcd.all_points_on();

//   sleep(2);
//   lcd.all_points_off();
//
//   sleep(1);
//   lcd.display_A();
//    lcd.print_A();
   lcd.clear_screen();

   lcd.print(1, 1, "A===A");

   sleep(3);

   lcd.print_direct(2, 2, "0000");


   return 0;
}
