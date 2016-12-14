#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define SMAXLEN 50

int ncinit() {
	
	initscr();
	noecho();
	cbreak();
	curs_set(0);
	start_color();
	use_default_colors();

	init_pair(1, COLOR_RED, -1);
	init_pair(2, COLOR_BLACK, COLOR_RED);

	return 0;
}

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

char *mktstr(char *str, time_t *t, struct tm *tm) {

	*t = time(NULL);
	*tm = *localtime(&*t);
	sprintf(str, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
	
	return str;
}

int usage(char *cmd) {

	printf("Usage: %s [-dsp] [message] - NCurses Window Bounce\n", cmd);
	printf("Bounces [message] around the screen\n\n");
	printf("Options:\n");
	printf("	-d: Show debug information\n");
	printf("	-h: Displays this text\n");
	printf("	-s [ms]: Sleep per cycle (default: 100)\n");
	printf("	-t: Use current time as message\n");
	printf("	-p [size]: # of chars padding (default: 2)\n");

	return 1;
}

int main(int argc, char *argv[]) {


	char str[SMAXLEN];
	int debug = 0;
	int pad = 2;
	int st = 0;
	unsigned long slp = 100000;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	int optc;

	while((optc = getopt(argc, argv, "dhts:p:")) != -1) {
		switch (optc) {

			case 'd':
				debug++;
				break;

			case 's':
				slp = atoi(optarg) * 1000;
				if (slp <= 0) return usage(argv[0]);
				break;

			case 't':
				st++;
				break;

			case 'h':
				return usage(argv[0]);
				break;

			case 'p':
				pad = atoi(optarg);
				break;

			case '?':
				return usage(argv[0]);
				break;

			default:
				break;
		}
	}

	if (argc > optind) {
		if (strlen(argv[optind]) > SMAXLEN || st) 
			return usage(argv[0]);
		else strcpy(str, argv[optind]); 
	} else if (st) strcpy(str, mktstr(str, &t, &tm));
	else strcpy(str, getenv("USER"));

	ncinit();
	WINDOW *txwin;

	int cp = 2;
	int yctr = LINES / 2;
	int xctr = COLS / 2;
	int len = strlen(str);

	int wh = (pad * 2) + 3;
	int ww = len + (pad * 2) + 2;
	int wsy = yctr - (pad + 1);
	int wsx = xctr - ((len / 2) + pad);

	int xi = 0, yi = 0;

	if (LINES < (pad * 2) + 3 || COLS < (pad * 2) + len + 2) {
		endwin();
		printf("Unreasonable padding!\n");
		return usage(argv[0]);
	}

	bkgd(COLOR_PAIR(1));
	refresh();

	if (debug) {
		mvprintw(5,5,"wh: %d", wh);
		mvprintw(6,5,"ww: %d", ww);
		mvprintw(7,5,"wsy: %d", wsy);
		mvprintw(8,5,"wsx: %d", wsx);
		mvprintw(9,5,"len: %d", len);
		mvprintw(10,5,"yctr: %d", yctr);
		mvprintw(11,5,"xctr: %d", xctr);
	}

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

		if (debug) {
			mvprintw(7,5,"wsy: %d", wsy);
			mvprintw(8,5,"wsx: %d", wsx);
		}

		if (st) {
			strcpy(str, mktstr(str, &t, &tm));
			mvwprintw(txwin, pad + 1, pad + 1, "%s", str);
		}

		mvwin(txwin, wsy, wsx);
		wbkgd(txwin, COLOR_PAIR(cp));
		bkgd(COLOR_PAIR(1));
		refresh();
		wrefresh(txwin);
		usleep(slp);
	}

	dwin(txwin);
	curs_set(1);
	endwin();
	return 0;
}
