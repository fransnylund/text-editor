#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void disable_raw_mode(void) {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode(void) {
	
	// Get terminal attributes and store them in orig_termios
	tcgetattr(STDIN_FILENO, &orig_termios);

	// When exiting, call disable_raw_mode()
	atexit(disable_raw_mode);
	
	struct termios raw = orig_termios;

	// Disable Ctrl-s and Ctrl-q and some other attributes
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	
	// Turn off output processing features
	raw.c_oflag &= ~(OPOST);

	// Disable attribute
	raw.c_cflag |= (CS8);
	
	// Disable attributes
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

	// Apply our modified attributes
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

}

int main(void) {

	enable_raw_mode();

	char c;
	while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
		// iscntrl() checks if a character is a control character
		if (iscntrl(c)) {
			printf("%d\r\n", c);
		}
		else {
			printf("%d ('%c')\r\n", c, c);
		}
	}

	return 0;
}
