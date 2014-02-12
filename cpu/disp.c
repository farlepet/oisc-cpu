#include <curses.h>
#include <signal.h>
#include <stdlib.h>

#include <disp.h>

static void disp_exit(); // Called by a signal

void disp_init()
{
	signal(SIGINT, disp_exit);

	initscr();
	keypad(stdscr, TRUE);
	nonl();
	cbreak();
	nodelay(stdscr, TRUE);
}

void disp_stop()
{
	endwin();
}

static void disp_exit()
{
	disp_exit();
	exit(0);
}
