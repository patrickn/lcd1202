
#include <iostream>
#include "LCD1202.h"

using namespace std;

int main(int argc, char** argv)
{
   LCD1202 lcd;

   lcd.clear_screen();
   lcd.write(3, 0, "========");

   lcd.write(5, 1, "++++++++");

   lcd.write(7, 1, "1 + 1 = ");
   lcd.write(7, 4, "2");
   return 0;
}
