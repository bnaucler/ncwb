#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define SMAXLEN 50
#define SBCH 256
#define BBCH 1024

#define RED				"\x1B[1m\033[31m"
#define RESET			"\033[0m"

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

	char *token = calloc(SBCH, sizeof(char));

	lwin = newwin(wh, ww, wsy, wsx);
	wbkgd(lwin, COLOR_PAIR(cp));
	attron(COLOR_PAIR(cp));
	box(lwin, 0 , 0);

	if (strstr(str, "\n") == NULL) {
		mvwprintw(lwin, pad + 1, pad + 1, "%s", str);

	} else {
		token = strtok(str, "\n");
		int vloc = pad + 1;

		while(token != NULL) {
			strcpy(str, token);
			mvwprintw(lwin, vloc++, pad + 1, "%s", str);
			token = strtok(NULL, "\n");
		}
	}

	refresh();
	wrefresh(lwin);
	attroff(COLOR_PAIR(cp));

	free(token);
	return lwin;
}

void dwin(WINDOW *lwin) {

	wborder(lwin, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(lwin);
	delwin(lwin);
}

int cchar(char *buf, char ch) {

	int count = 0;
	int len = strlen(buf);

	for(int a = 0; a < len; a++) { if(buf[a] == ch) count++; }

	return count;

}

int usage(char *cmd, char *err, int ret) {

	if (strlen(err) > 0 ) { printf(RED "ERROR: " RESET "%s\n", err); }
	printf("NCurses Window Bounce - Bounces [message] around the screen\n\n");
	printf("Usage: %s [-dfstp] [message]\n", cmd);
	printf("Options:\n");
	printf("	-d: Show debug information\n");
	printf("	-f: Use figlet\n");
	printf("	-h: Displays this text\n");
	printf("	-s [ms]: Sleep per cycle (default: 100)\n");
	printf("	-t: Use current time as message\n");
	printf("	-p [size]: # of chars padding (default: 2)\n");

	exit(ret);
}

void cleanup(int sig) {

	signal(sig, SIG_IGN);
	curs_set(1);
	endwin();

	exit(0);
}

char *mkfstr(char *str, char *estr, char *cmd, int *len) {

	char cstr[SBCH];
	char sbuf[SBCH];
	char bbuf[BBCH];

	strcpy(cstr, "figlet ");
	strcat(cstr, str);

	FILE *fout = popen(cstr, "r");
	if (fout == NULL) {
		sprintf(estr,"Could not execute %s\n", cstr);
		usage(cmd, estr, 1);

	} else {
		while(fgets(sbuf, SBCH, fout)) {
			*len = strlen(sbuf);
			if (*len > SMAXLEN) {
				usage(cmd, "String too long", 1);
			}
			strcat(bbuf, sbuf);
		}
		bbuf[(strlen(bbuf) + 1)] = '\0';
	}
	pclose(fout);

	strcpy(str, bbuf);
	return str;
}

char *mktstr(char *str, time_t *t, struct tm *tm) {

	*t = time(NULL);
	*tm = *localtime(&*t);
	sprintf(str, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);

	return str;
}

int main(int argc, char *argv[]) {

	signal(SIGINT, cleanup);

	char *str = calloc(BBCH, sizeof(char));
	char *estr = calloc(SBCH, sizeof(char));

	int debug = 0, fig = 0, st = 0;
	int pad = 2, lns = 1;
	unsigned long slp = 100000;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	int optc;

	while((optc = getopt(argc, argv, "dfhts:p:")) != -1) {
		switch (optc) {

			case 'd':
				debug++;
				break;

			case 'f':
				fig++;
				break;

			case 's':
				slp = atoi(optarg) * 1000;
				if (slp <= 0) return usage(argv[0], "", 0);
				break;

			case 't':
				st++;
				break;

			case 'h':
				return usage(argv[0], "", 0);
				break;

			case 'p':
				pad = atoi(optarg);
				break;

			case '?':
				return usage(argv[0], "", 0);
				break;

			default:
				break;
		}
	}

	if (argc > optind) {
		if (strlen(argv[optind]) > SMAXLEN) {
			sprintf(estr, "Message is %ld chars long. Maximal length allowed is %d", strlen(argv[optind]), SMAXLEN);
			return usage(argv[0], estr, 1);
		} else if (st) {
			return usage(argv[0], "-t cannot be combined with a custom message.\n", 1);
		} else strcpy(str, argv[optind]);
	} else if (st) strcpy(str, mktstr(str, &t, &tm));
	else strcpy(str, getenv("USER"));

	int len = strlen(str);

	if (fig) {
		if (st) { 
			return usage(argv[0], "-t and -f cannot be combined", 1);
		} else { 
			strcpy(str, mkfstr(str, estr, argv[0], &len));
			lns = cchar(str, '\n');
		}
	}

	ncinit();
	WINDOW *txwin;

	int cp = 2;

	int wh = (pad * 2) + 2 + lns;
	int ww = len + (pad * 2) + 2;
	int wsy = (LINES / 2) - (pad + 1 + (lns / 2));
	int wsx = (COLS / 2) - ((len / 2) + pad);

	int xi = 0, yi = 0;

	if (LINES < (pad * 2) + 3 || COLS < (pad * 2) + len + 2) {
		curs_set(1);
		endwin();
		return usage(argv[0], "Unreasonable padding!", 1);
	}

	bkgd(COLOR_PAIR(1));
	refresh();

	if (debug) {
		mvprintw(5,5,"Window height: %03d", wh);
		mvprintw(6,5,"Window width: %03d", ww);
		mvprintw(7,5,"Upper left corner, X-axis: %03d", wsx);
		mvprintw(8,5,"Upper left corner, Y-axis: %03d", wsy);
		mvprintw(9,5,"String length: %02d", len);
		mvprintw(10,5,"String height: %02d", lns);
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
			mvprintw(7,5,"Upper left corner, X-axis: %03d", wsx);
			mvprintw(8,5,"Upper left corner, Y-axis: %03d", wsy);
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

	return 0;
}
