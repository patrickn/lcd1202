

#include "LCD1202.h"

int main(int argc, char** argv)
{
   LCD1202 lcd;
   lcd.all_points_on();
//
   sleep(5);
   lcd.all_points_off();
//
//    sleep(5);
//    lcd.print_A();
//    lcd.print_A();
//    lcd.print_A();
//    lcd.print_A();

   return 0;
}
