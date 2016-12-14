#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SMAXLEN 50

WINDOW *mkwin(char *str, int wh, int ww, int wsy, int wsx, int pad, int cp) {	

	WINDOW *lwin;

	lwin = newwin(wh, ww, wsy, wsx);
	wbkgd(lwin, COLOR_PAIR(cp));
	attron(COLOR_PAIR(cp));
	box(lwin, 0 , 0);
	mvwprintw(lwin, pad + 1, pad + 1, "%s", str);
	refresh();
	wrefresh(lwin);
	attroff(COLOR_PAIR(cp));

	return lwin;
}

void dwin(WINDOW *lwin) {	

	wborder(lwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(lwin);
	delwin(lwin);
}

int main(int argc, char *argv[]) {

	WINDOW *txwin;

	initscr();
	noecho();
	cbreak();
	curs_set(0);
	start_color();
	use_default_colors();

	init_pair(1, COLOR_RED, -1);
	init_pair(2, COLOR_BLACK, COLOR_RED);

	char str[SMAXLEN];

	if (argc == 1) {
		strcpy(str, getenv("USER")); 
	} else {
		if (strlen(argv[1]) < SMAXLEN) strcpy(str, argv[1]); 
		else return 1;
	}

	int pad = 2;
	int cp = 2;
	int yctr = LINES / 2;
	int xctr = COLS / 2;
	int len = strlen(str);

	int wh = (pad * 2) + 3;
	int ww = len + (pad * 2) + 2;
	int wsy = yctr - (pad + 1);
	int wsx = xctr - ((len / 2) + pad);

	int xi = 0;
	int yi = 0;

	bkgd(COLOR_PAIR(1));
	refresh();

	mvprintw(5,5,"wh: %d", wh);
	mvprintw(6,5,"ww: %d", ww);
	mvprintw(7,5,"wsy: %d", wsy);
	mvprintw(8,5,"wsx: %d", wsx);
	mvprintw(9,5,"len: %d", len);
	mvprintw(10,5,"yctr: %d", yctr);
	mvprintw(11,5,"xctr: %d", xctr);

	txwin = mkwin(str, wh, ww, wsy, wsx, pad, cp);

	for (;;) {

		if (xi == 0) {
			wsx--;
			if (wsx == 0) xi++;

		} else {
			wsx++;
			if (wsx > COLS - ww) xi = 0;
		}

		if (yi == 0) {
			wsy--;
			if (wsy == 0) yi++;

		} else {
			wsy++;
			if (wsy > LINES - wh) yi = 0;
		}

		mvprintw(7,5,"wsy: %d", wsy);
		mvprintw(8,5,"wsx: %d", wsx);

		mvwin(txwin, wsy, wsx);
		wbkgd(txwin, COLOR_PAIR(cp));
		bkgd(COLOR_PAIR(1));
		refresh();
		wrefresh(txwin);
		usleep(100000);
	}

	dwin(txwin);
	curs_set(1);
	endwin();
	return 0;
}
