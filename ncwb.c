#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define VER 0.2

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

	init_pair(1, -1, -1);
	init_pair(2, COLOR_WHITE, -1);
	init_pair(3, COLOR_RED, -1);
	init_pair(4, COLOR_GREEN, -1);
	init_pair(5, COLOR_BLACK, COLOR_WHITE);
	init_pair(6, COLOR_BLACK, COLOR_RED);

	return 0;
}

int prwtxt(WINDOW *lwin, char *str, int pad, int cp) {

	char *token = calloc(SBCH, sizeof(char));
	char buf[SBCH];

	token = strtok(str, "\n");
	int vloc = pad + 1;

	wattron(lwin, A_BOLD);

	while(token != NULL) {
		strcpy(buf, token);
		mvwprintw(lwin, vloc++, pad + 1, "%s", buf);
		token = strtok(NULL, "\n");
	}

	wattroff(lwin, A_BOLD);

	free(token);
	return 0;
}

int cchar(char *buf, char ch) {

	int count = 0;
	int len = strlen(buf);

	for(int a = 0; a < len; a++) { if(buf[a] == ch) count++; }

	return count;

}

int usage(char *cmd, char *err, int ret) {

	if (strlen(err) > 0 ) { printf(RED "ERROR: " RESET "%s\n", err); }
	printf("NCurses Window Bounce v%.1f - Bounces [message] around the screen\n\n", VER);
	printf("Usage: %s [-cdfhlnrstp] [message]\n", cmd);
	printf("Options:\n");
	printf("	-c [num]: Color pair 1-6 (default: 1)\n");
	printf("	-d: Show debug information\n");
	printf("	-f: Use figlet\n");
	printf("	-h: Displays this text\n");
	printf("	-l: Leave trail\n");
	printf("	-n: No window movement\n");
	printf("	-r: Random window movement\n");
	printf("	-s [ms]: Sleep per cycle (default: 200)\n");
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

int mknv(int cur, int dir, int max, int rm) {

	if (rm) {
		cur = rand() % ++max;

	} else {
		if (dir) ++cur;
		else --cur;
	}

	return cur;
}

int cdir(int cur, int dir, int max) {

	if(cur > max - 1) dir = 0;
	if(cur < 1) dir = 1;

	return dir;
}

void prdbg(int wh, int ww, int wsx, int wsy, int len, int lns) {

	mvprintw(5,5,"Window height: %d", wh);
	mvprintw(6,5,"Window width: %02d", ww);
	mvprintw(7,5,"Upper left corner, X-axis: %03d", wsx);
	mvprintw(8,5,"Upper left corner, Y-axis: %03d", wsy);
	mvprintw(9,5,"String length: %02d", len);
	mvprintw(10,5,"String height: %02d", lns);
}

int main(int argc, char *argv[]) {

	signal(SIGINT, cleanup);

	char *str = calloc(BBCH, sizeof(char));
	char *fstr = calloc(BBCH, sizeof(char));
	char *estr = calloc(SBCH, sizeof(char));

	int debug = 0, fig = 0, st = 0, nb = 0, tl = 0, rm = 0;
	int pad = 2, lns = 1, plen = 0;
	int cp = 1;
	int optc;

	unsigned long slp = 200000;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	srand((unsigned) time(&t));

	while((optc = getopt(argc, argv, "c:dfhlnp:trs:")) != -1) {
		switch (optc) {

			case 'c':
				cp = atoi(optarg);
				break;

			case 'd':
				debug++;
				break;

			case 'f':
				fig++;
				break;

			case 'h':
				return usage(argv[0], "", 0);
				break;

			case 'l':
				tl++;
				break;

			case 'n':
				nb++;
				break;

			case 'p':
				pad = atoi(optarg);
				break;

			case 'r':
				rm++;
				break;

			case 's':
				slp = atoi(optarg) * 1000;
				if (slp <= 0) return usage(argv[0], "", 0);
				break;

			case 't':
				st++;
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
			strcpy(str, mktstr(str, &t, &tm));
			strcpy(fstr, mkfstr(str, estr, str , &len));

		} else {
			strcpy(str, mkfstr(str, estr, argv[0], &len));
			str++;
		}
		lns = cchar(str, '\n');
	}

	ncinit();
	WINDOW *txwin = calloc(BBCH, sizeof(char));

	int wh = (pad * 2) + 2 + lns;
	int ww = len + (pad * 2) + 2;
	int wsy = (LINES / 2) - (pad + 1 + (lns / 2));
	int wsx = (COLS / 2) - ((len / 2) + pad);

	int xi = 1, yi = 0;

	if (LINES < (pad * 2) + 3 || COLS < (pad * 2) + len + 2) {
		curs_set(1);
		endwin();
		return usage(argv[0], "Unreasonable padding!", 1);
	}

	bkgd(COLOR_PAIR(1));
	refresh();
	txwin = newwin(wh, ww, wsy, wsx);
	wbkgd(txwin, COLOR_PAIR(cp));
	prwtxt(txwin, str, pad, cp);
	box(txwin, 0, 0);
	touchwin(txwin);

	for (;;) {

		if (!nb) {
			wsx = mknv(wsx, xi, COLS - ww, rm);
			wsy = mknv(wsy, yi, LINES - wh, rm);

			if (!rm) {
				xi = cdir(wsx, xi, COLS - ww);
				yi = cdir(wsy, yi, LINES - wh);
			}
		}

		if (st) {
			strcpy(str, mktstr(str, &t, &tm));

			if (fig) {
				strcpy(fstr, mkfstr(str, estr, str , &len));

				if (len != plen) {
					ww = len + (pad * 2) + 2;
					wresize(txwin, wh, ww);
					werase(txwin);
					plen = len;
				}

				prwtxt(txwin, fstr, pad, cp);
				wborder(txwin, '|', '|', '-', '-', '+', '+', '+', '+');
				if (!tl) refresh();

			} else {
				mvwprintw(txwin, pad + 1, pad + 1, "%s", str);
			}
		}

		if (debug) { prdbg(wh, ww, wsx, wsy, len, lns); }

		mvwin(txwin, wsy, wsx);
		bkgd(COLOR_PAIR(1));
		wbkgd(txwin, COLOR_PAIR(cp));
		if (!tl) refresh();
		wrefresh(txwin);
		usleep(slp);
	}

	return 0;
}
