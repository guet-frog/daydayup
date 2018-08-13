#include "uart.h"
#include "lcd.h"

int main()
{	
	lcd_init();
	backlight_ctl(1);
	lcd_enable(1);
	clear_screen();
	draw_line(10, 10, 750, 450, RED);
	draw_circle(150, 150, 150, BLUE);
	
	return 0;
}
