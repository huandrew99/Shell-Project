
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>

/* 
 * Sets terminal into raw mode. 
 * This causes having the characters available
 * immediately instead of waiting for a newline. 
 * Also there is no automatic echo.
 */
struct termios tty_attr;

void disable_raw_mode(void) {
  tcsetattr(0, TCSAFLUSH, &tty_attr);
}

void tty_raw_mode(void)
{
	
     
	tcgetattr(0,&tty_attr);
  atexit(disable_raw_mode);
struct termios raw = tty_attr
	/* Set raw mode. */
	raw.c_lflag &= (~(ICANON|ECHO));
	raw.c_cc[VTIME] = 0;
	raw.c_cc[VMIN] = 1;
     
	tcsetattr(0,TCSANOW,&raw);
}
