#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include "fractal.h"
#include "config.h"

// DEPRECATED; NOW USE dynamic size
// #define MAX_VALUE   65



const char charset[] = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/*tfjrxnuvczYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
const size_t charset_len = sizeof(charset)-2;

const char* help_message_header = "HELP(you've asked for it)";



size_t maxlen;
size_t help_message_nlines;


int 
init(struct sstate* pstate)
{
		struct sstate state = {0,0,
				.rMax = START_RMAX,
				.rMin = START_RMIN,
				.iMax = START_IMAX,
				.iMin = START_IMIN,
				.zoom = 1.0,
				false,
		};
		*pstate = state;

		FILE* fp = fopen("fractal.log","w");

		initscr();
		curs_set(0);
		cbreak();
		clear();
		mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

		//mouseinterval(0);  // Respond to mouse quickly
		keypad(stdscr, TRUE);
		printf("\033[?1003h\n"); // Enable mouse move events (drag)
		size_t i = sizeof(keybinds)/sizeof(struct keybind);
		size_t str_len;
		help_message_nlines = i;
		assert(i > 0 && "help messages/ not present");
		while(i--){
				if((str_len = strlen(keybinds[i].help_msg)) > maxlen) 
						maxlen = str_len;
		}
		maxlen+=2;
		pstate->log = fp ? fp : stderr;

		return 0;
}


bool isrightdrag(struct sstate state, int key)
{
		return state.dragging;
}

bool isleftclick(struct sstate state, int key)
{
		MEVENT event;
		if (getmouse(&event) == OK) {
				if(event.bstate & BUTTON1_CLICKED){
						return true;
				}
		}
		return false;
}

void
zoom(struct sstate* curr_state, double x){
		struct complex_s midpoint;
		if(x == 0)
				return;
		midpoint.r = (curr_state->rMax+curr_state->rMin)/2;
		midpoint.i = (curr_state->iMax+curr_state->iMin)/2;
		curr_state->rMax = midpoint.r + fabs(midpoint.r-curr_state->rMax)*x;
		curr_state->iMax = midpoint.i + fabs(midpoint.i-curr_state->iMax)*x;
		curr_state->rMin = midpoint.r - fabs(midpoint.r-curr_state->rMin)*x;
		curr_state->iMin = midpoint.i - fabs(midpoint.i-curr_state->iMin)*x;
		curr_state->zoom /= x;
}


void
resetzoom(struct sstate* curr_state){
		zoom(curr_state, curr_state->zoom);
}

void
processmouse(struct sstate* curr_state)
{
		MEVENT event;
		struct complex_s z, midpoint, zdiff, max_z, min_z;

		if (getmouse(&event) == OK) {
				if(event.bstate & BUTTON1_CLICKED){
						z.r = calculate_re((*curr_state),event.x);
						z.i = calculate_im((*curr_state),event.y);

						midpoint.r = (curr_state->rMax+curr_state->rMin)/2;
						midpoint.i = (curr_state->iMax+curr_state->iMin)/2;

						zdiff.r = midpoint.r - z.r;
						zdiff.i = midpoint.i - z.i;

						curr_state->rMin-=zdiff.r;
						curr_state->rMax-=zdiff.r;
						curr_state->iMin-=zdiff.i;
						curr_state->iMax-=zdiff.i;
						mvprintw(0,1,"clicked on %d, %d", event.x, event.y);
						refresh();

				}
				if(event.bstate & BUTTON3_PRESSED){
						curr_state->dragging = true;
						curr_state->dragging_start.r = calculate_re((*curr_state),event.x);
						curr_state->dragging_start.i = calculate_im((*curr_state),event.y);
						mvprintw(0,1,"dragging..");
						refresh();
				}
				if(event.bstate & BUTTON3_RELEASED){
						clear();
						box(stdscr,0,0);
						curr_state->dragging = false;

						curr_state->dragging_end.r = calculate_re((*curr_state),event.x);
						curr_state->dragging_end.i = calculate_im((*curr_state),event.y);

						max_z.r = curr_state->dragging_end.r > curr_state->dragging_start.r ?
								curr_state->dragging_end.r : curr_state->dragging_start.r;
						max_z.i = curr_state->dragging_end.i > curr_state->dragging_start.i ?
								curr_state->dragging_end.i : curr_state->dragging_start.i;

						min_z.r = curr_state->dragging_end.r < curr_state->dragging_start.r ?
								curr_state->dragging_end.r : curr_state->dragging_start.r;
						min_z.i = curr_state->dragging_end.i < curr_state->dragging_start.i ?
								curr_state->dragging_end.i : curr_state->dragging_start.i;

						double scale;
						if((max_z.r - min_z.r) > (max_z.i - max_z.i)){
								scale = (max_z.r - min_z.r) / curr_state->width;
						} else {
								scale = (max_z.i - min_z.i) / curr_state->height;
						}

						curr_state->rMin = min_z.r;
						curr_state->iMin = min_z.i;
						curr_state->rMax = max_z.r;
						curr_state->iMax = max_z.i;
						curr_state->zoom *= scale;

						mvprintw(0,1,"dragged limits to (%.2lf,%.2lf) (%.2lf,%.2lf)",
										curr_state->rMin, curr_state->iMin, curr_state->rMax, curr_state->iMax);
						refresh();
				}
		}
}


char
get_usable_char(uint16_t x){
		if(x > charset_len)
				x = charset_len;
		return charset[x];
}

/* // UNUSED
double
get_scale(struct sstate state, int maxx, int maxy)
{
		return maxx/(state.rMax-state.rMin) < maxy/(state.iMax-state.iMin) ? maxx/(state.rMax-state.rMin): maxy/(state.iMax-state.iMin);
}*/

double
mantelbrot(double r, double i){
		double zr = 0.0, zi = 0.0;
		double tmpr, tmpi;
		int it = 0;

		for(;zr*zr+zi*zi<4 && it < 100; it++){
				tmpr = zr*zr-zi*zi;
				tmpi = 2*zr*zi;

				zr = tmpr+r;
				zi = tmpi+i;
		}

		return (double)it/100;
}

int 
mainloop(struct sstate* pstate)
{
		bool running = true;
		
		int x, y, ch;
		int prevheight, prevwidth;
		char c;
		double scale, real, imag;
		WINDOW* helpwin;


		clear();
		box(stdscr,0,0);
		while(running){
				getmaxyx(stdscr,pstate->height,pstate->width);
				if(prevwidth != pstate->width || prevheight != prevheight)
						box(stdscr,0,0);
				prevwidth = pstate->width;
				prevheight = pstate->height;
				mvprintw(0,1,"%04dx%04d",pstate->width,pstate->height);
				mvprintw(0,4+strlen("0000x0000"),"? for help");
				for(x=1;x<pstate->width-1;x++){
						for(y=1;y<pstate->height-1;y++){
								real = calculate_re((*pstate),x);
								imag = calculate_im((*pstate),y);

								c = get_usable_char((int)(mantelbrot(real, imag)*(charset_len+1)));
								mvprintw(y,x,"%c",c);
						}
				}
				refresh();

				if((ch = getch())){
						if(ch == KEY_MOUSE)
								processmouse(pstate);
						if(!pstate->dragging)
						switch(ch){
								case KEY_UP:
										pstate->iMin-=(pstate->iMax-pstate->iMin)*SPEED;
										pstate->iMax-=(pstate->iMax-pstate->iMin)*SPEED;
										break;
								case KEY_DOWN:
										pstate->iMin+=(pstate->iMax-pstate->iMin)*SPEED;
										pstate->iMax+=(pstate->iMax-pstate->iMin)*SPEED;
										break;
								case KEY_LEFT:
										pstate->rMin-=(pstate->rMax-pstate->rMin)*SPEED;
										pstate->rMax-=(pstate->rMax-pstate->rMin)*SPEED;
										break;
								case KEY_RIGHT:
										pstate->rMin+=(pstate->rMax-pstate->rMin)*SPEED;
										pstate->rMax+=(pstate->rMax-pstate->rMin)*SPEED;
										break;
								case 'h':
										pstate->rMin-=SPEED;
										break;
								case 'H':
										pstate->rMin+=SPEED;
										break;
								case 'j':
										pstate->iMax+=SPEED;
										break;
								case 'J':
										pstate->iMax-=SPEED;
										break;
								case 'k':
										pstate->iMin-=SPEED;
										break;
								case 'K':
										pstate->iMin+=SPEED;
										break;
								case 'l':
										pstate->rMax+=SPEED;
										break;
								case 'L':
										pstate->rMax-=SPEED;
										break;
								case '+':
										zoom(pstate,.8);
										break;
								case '-':
										zoom(pstate,1.2);
										break;
								case '.':
										resetzoom(pstate);
										break;
								case '?':
										refresh(); // to prevent buffer errors *** NOTE: DO NOT REMOVE ***
										helpwin = newwin(help_message_nlines+2,maxlen+2,(pstate->height-help_message_nlines)/2+2,(pstate->width-maxlen)/2+2);
										// helpwin = createwindow();


										box(helpwin,0,0);
										mvwprintw(helpwin,0,(pstate->width-strlen(help_message_header))/2,"%s", help_message_header);
										for(int i = 0; i < help_message_nlines; i++){ 
												if(keybinds[i].help_msg != NULL) // this means it's not meant to be displayed
														mvwprintw(helpwin,i+1,3,keybinds[i].help_msg, keybinds[i].key);
										}
										wrefresh(helpwin);

										getch();
										
										delwin(helpwin);
										break;
								case 'q': /* FALLTHROUGH */
										running = false;
								case 'c':
										clear();
										break;
								default:
										break;

						}
				}
		}

		return 0;
}

int 
finish(struct sstate* pstate)
{

		printf("\033[?1003l\n");

		if(pstate->log != stderr && pstate->log != stdout && pstate->log != NULL){
				fclose(pstate->log);
		}

		endwin();
		return 0;
}





