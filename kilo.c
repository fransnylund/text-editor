// includes

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

// data

struct termios orig_termios;

// terminal

void die(const char *s) {
	// perror() looks at the errno variable and prints the error
	perror(s);
	exit(1);
}

void disable_raw_mode(void) {
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
		die("tcsetattr"); }
}

void enable_raw_mode(void) {
	
	// Get terminal attributes and store them in orig_termios
	if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
		die("tcgetattr");
	}

	// When exiting, call disable_raw_mode()
	atexit(disable_raw_mode);
	
	struct termios raw = orig_termios;

	// Disable Ctrl-s and Ctrl-q and some other attributes
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	
	// Turn off output processing features
	raw.c_oflag &= ~(OPOST);

	// Disable attribute
	raw.c_cflag |= (CS8);
	
	// Disable attributes
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;

	// Apply our modified attributes
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
		die("tcsetattr");
	}

}

// init

int main(void) {

	enable_raw_mode();

	while (1) {

		char c = '\0';
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) {
			die("read");
		}

		// iscntrl() checks if a character is a control character
		if (iscntrl(c)) {
			printf("%d\r\n", c);
		}
		else {
			printf("%d ('%c')\r\n", c, c);
		}

		if (c == 'q')
			break;
	}

	return 0;
}
