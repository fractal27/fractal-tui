#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// DEPRECATED; NOW USE dynamic size
// #define MAX_VALUE   65


#define calculate_re(state,x) (state.rMin + (x - 1) * (state.rMax - state.rMin) / (state.width - 2))
#define calculate_im(state,y) (state.iMin + (y - 1) * (state.iMax - state.iMin) / (state.height - 2))
#define SPEED 0.1

// var definition
//

struct sstate {
		int height;
		int width;
		double rMax ;//= .2;
		double rMin ;//= -1;
		double iMax ;//= 2.;
		double iMin ;//= -2.;
		bool dragging;
		struct complex_s{
				double r;
				double i;
		} dragging_start, dragging_end;
		double zoom;
		FILE* log;
} state = {0,0,
		.rMax = .2,
		.rMin = -1.,
		.iMax = 2.,
		.iMin = -2.,
		.zoom = 1.0,
		false,
};

int init();
int mainloop();
int main();
int finish();



const char charset[] = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/*tfjrxnuvczYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
const size_t charset_len = sizeof(charset)-2;

const char* help_message[] = {
		"         HELP(you've asked for it)",
		"[j] To decrease .1 from the minimum imaginary value",
		"[J] To add .1 from the minimum imaginary value",
		"[k] To decrease .1 from the maximum imaginary value",
		"[K] To add .1 from the maximum imaginary value",
		"[h] To decrease .1 from the minimum real value",
		"[h] To add .1 from the minimum real value",
		"[l] To decrease .1 from the maximum real value",
		"[L] To add .1 from the maximum real value",
		"[+] To zoom in",
		"[-] To zoom out",
		"[.] To reset the zoom",
		"[RIGHT MOUSE DRAG] To zoom in dynamically",
		"[LEFT CLICK] To center the screen to the point clicked.",
		"[?] Show this page",
};
size_t maxlen;
size_t help_message_nlines;


int 
init()
{
		FILE* fp = fopen("fractal.log","w");

		initscr();
		curs_set(0);
		cbreak();
		clear();
		mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

		//mouseinterval(0);  // Respond to mouse quickly
		keypad(stdscr, TRUE);
		printf("\033[?1003h\n"); // Enable mouse move events (drag)
		size_t i = sizeof(help_message)/sizeof(const char*);
		size_t str_len;
		help_message_nlines = sizeof(help_message)/sizeof(const char*);
		assert(i > 0 && "help message not present");
		while(i--){
				if((str_len = strlen(help_message[i])) > maxlen) 
						maxlen = str_len;
		}
		maxlen+=2;
		state.log = fp ? fp : stderr;

		return 0;
}



void
zoom(struct sstate* curr_state, double x){
		struct complex_s midpoint;
		if(x == 0)
				return;
		midpoint.r = (state.rMax+state.rMin)/2;
		midpoint.i = (state.iMax+state.iMin)/2;
		curr_state->rMax = midpoint.r + fabs(midpoint.r-curr_state->rMax)*x;
		curr_state->iMax = midpoint.i + fabs(midpoint.i-curr_state->iMax)*x;
		curr_state->rMin = midpoint.r - fabs(midpoint.r-curr_state->rMin)*x;
		curr_state->iMin = midpoint.i - fabs(midpoint.i-curr_state->iMin)*x;
		curr_state->zoom /= x;
}


inline
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

double
get_scale(int maxx, int maxy)
{
		return maxx/(state.rMax-state.rMin) < maxy/(state.iMax-state.iMin) ? maxx/(state.rMax-state.rMin): maxy/(state.iMax-state.iMin);
}

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
mainloop()
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
				getmaxyx(stdscr,state.height,state.width);
				if(prevwidth != state.width || prevheight != prevheight)
						box(stdscr,0,0);
				prevwidth = state.width;
				prevheight = state.height;
				mvprintw(0,1,"%04dx%04d",state.width,state.height);
				mvprintw(0,4+strlen("0000x0000"),"? for help");
				for(x=1;x<state.width-1;x++){
						for(y=1;y<state.height-1;y++){
								real = calculate_re(state,x);
								imag = calculate_im(state,y);

								c = get_usable_char((int)(mantelbrot(real, imag)*(charset_len+1)));
								mvprintw(y,x,"%c",c);
						}
				}
				refresh();

				if((ch = getch())){
						if(ch == KEY_MOUSE)
								processmouse(&state);
						if(!state.dragging)
						switch(ch){
								case KEY_UP:
										state.iMin-=(state.iMax-state.iMin)*SPEED;
										state.iMax-=(state.iMax-state.iMin)*SPEED;
										break;
								case KEY_DOWN:
										state.iMin+=(state.iMax-state.iMin)*SPEED;
										state.iMax+=(state.iMax-state.iMin)*SPEED;
										break;
								case KEY_LEFT:
										state.rMin-=(state.rMax-state.rMin)*SPEED;
										state.rMax-=(state.rMax-state.rMin)*SPEED;
										break;
								case KEY_RIGHT:
										state.rMin+=(state.rMax-state.rMin)*SPEED;
										state.rMax+=(state.rMax-state.rMin)*SPEED;
										break;
								case 'h':
										state.rMin-=SPEED;
										break;
								case 'H':
										state.rMin+=SPEED;
										break;
								case 'j':
										state.iMax+=SPEED;
										break;
								case 'J':
										state.iMax-=SPEED;
										break;
								case 'k':
										state.iMin-=SPEED;
										break;
								case 'K':
										state.iMin+=SPEED;
										break;
								case 'l':
										state.rMax+=SPEED;
										break;
								case 'L':
										state.rMax-=SPEED;
										break;
								case '+':
										zoom(&state,.8);
										break;
								case '-':
										zoom(&state,1.2);
										break;
								case '.':
										resetzoom(&state);
										break;
								case '?':
										refresh(); // to prevent buffer errors *** NOTE: DO NOT REMOVE ***
										helpwin = newwin(help_message_nlines+2,maxlen+2,(state.height-help_message_nlines)/2+2,(state.width-maxlen)/2+2);
										// helpwin = createwindow();


										box(helpwin,0,0);
										for(int i = 0; i < help_message_nlines; i++){ 
												mvwprintw(helpwin,i+1,3,"%s", help_message[i]);
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
finish()
{

		printf("\033[?1003l\n");
		endwin();
		return 0;
}


int 
main()
{
		int ret;

		if((ret = init()))
				return ret;

		if((ret = mainloop()))
				return ret;

		return finish();
}


