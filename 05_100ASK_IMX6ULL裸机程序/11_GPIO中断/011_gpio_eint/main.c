#include "key.h"
#include "led.h"
#include "interrupt.h"
#include "my_printf.h"

void system_init()
{
	led_init();
	led_ctl(0);
	buttons_init();
	key_exit_init();
}

int  main()
{
	unsigned char c;

	while(1)
	{
		c = getchar();
		if (c == '\r')
		{
			putchar('\n');
		}

		if (c == '\n')
		{
			putchar('\r');
		}

		putchar(c);

	}
					
	return 0;
}


