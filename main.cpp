
#include <iostream>
#include "LCD1202.h"

using namespace std;

int main(int argc, char** argv)
{
   LCD1202 lcd;

	// Some test strings

	lcd.xputs(" G`Day world!",    7);
	lcd.xputs("ABCDEFGHUJKLMNOP", 5);
	lcd.xputs("QRSTUVWXYZ~!@#$'", 4);
	lcd.xputs("abcdefghijklmnop", 3);
	lcd.xputs("qrstuvwxyz%^&*()", 2);
	lcd.xputs("`1234567890_+-=/", 1);
	lcd.xputs("[]{}|\\<>,.?",     0);

   return 0;
}
