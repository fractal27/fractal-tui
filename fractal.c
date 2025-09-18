#include <ncurses.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "fractal.h"
#include "keybindingsreader/keybindingsreader.h"
#include "config.h"
#include <stdlib.h>

// DEPRECATED; NOW USE dynamic size
// #define MAX_VALUE   65
#define LOG(fp, s, ...)     fprintf(fp, "%.4lf: " s "\n", 1000*(double)clock()/CLOCKS_PER_SEC, __VA_ARGS__)


const char charset[] = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/*tfjrxnuvczYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
const size_t charset_len = sizeof(charset)-2;

const char* help_message_header = "HELP(you've asked for it)";
const char* configfile = "/home/anon/.config/fractal-tui/keybinds.conf";
bool using_configfile = false;

struct KeybindingsReader kbreader;

void* keybinds_values_assoc[] = {
		CompressDOWN,
		StretchDOWN,
		CompressUP,
		StretchUP,
		CompressRIGHT,
		StretchRIGHT,
		CompressLEFT,
		StretchLEFT,
		ZoomIn,
		ZoomOut,
		ResetZoom,
		ProcessMouse,
		ShowHelp,	
		Finish,
		MoveLEFT,
		MoveUP,
		MoveDOWN,
		MoveRIGHT,
		NULL
};

const char* keybinds_values_acc[] = {
		"CompressDOWN",
		"StretchDOWN",
		"CompressUP",
		"StretchUP",
		"CompressRIGHT",
		"StretchRIGHT",
		"CompressLEFT",
		"StretchLEFT",
		"ZoomIn",
		"ZoomOut",
		"ResetZoom",
		"ProcessMouse",
		"ShowHelp",	
		"Finish",
		"NULL"
};

const char* keybinds_whens_acc[] = {
		"NULL"
};

void* keybinds_whens_assoc[] = {
		NULL
};


size_t maxlen;
size_t bindings_n = sizeof(keybinds_s)/sizeof(struct keybind);
/* Binding Actions */

void StretchUP(struct sstate* pstate)  		{	pstate->iMax-=SPEED; }
void StretchDOWN(struct sstate* pstate)		{	pstate->iMin+=SPEED; }
void StretchLEFT(struct sstate* pstate)		{	pstate->rMax-=SPEED; }
void StretchRIGHT(struct sstate* pstate)	{	pstate->rMin+=SPEED; }

void CompressUP(struct sstate* pstate)  	{	pstate->iMax+=SPEED; }
void CompressDOWN(struct sstate* pstate)	{	pstate->iMin-=SPEED; }
void CompressLEFT(struct sstate* pstate)	{	pstate->rMax+=SPEED; }
void CompressRIGHT(struct sstate* pstate)	{	pstate->rMin-=SPEED; }

void ZoomIn(struct sstate* pstate){			zoom(pstate, .8);}
void ZoomOut(struct sstate* pstate){		zoom(pstate, 1.2);}
void ResetZoom(struct sstate* pstate){		zoom(pstate, pstate->zoom);}
void Clear(struct sstate* pstate){			clear(); }
void Finish(struct sstate* pstate){			exit(finish(pstate)); }

void MoveUP(struct sstate* pstate){
		pstate->iMin-=(pstate->iMax-pstate->iMin)*SPEED;
		pstate->iMax-=(pstate->iMax-pstate->iMin)*SPEED;
}
void MoveDOWN(struct sstate* pstate){
		pstate->iMin+=(pstate->iMax-pstate->iMin)*SPEED;
		pstate->iMax+=(pstate->iMax-pstate->iMin)*SPEED;
}

void MoveLEFT(struct sstate* pstate) {
		pstate->rMin-=(pstate->rMax-pstate->rMin)*SPEED;
		pstate->rMax-=(pstate->rMax-pstate->rMin)*SPEED;
}

void MoveRIGHT(struct sstate* pstate) {
		pstate->rMin+=(pstate->rMax-pstate->rMin)*SPEED;
		pstate->rMax+=(pstate->rMax-pstate->rMin)*SPEED;
}

void ShowHelp(struct sstate* pstate){
		WINDOW* helpwin;
		refresh(); // to prevent buffer errors *** NOTE: DO NOT REMOVE ***
		size_t tot_nulls = 0;
		for(int i = 0; i < bindings_n; i++) if(!keybinds[i].help_msg) tot_nulls++;

		helpwin = newwin(bindings_n+2-tot_nulls,
						maxlen+2,
						(pstate->height-bindings_n)/2+2,
						(pstate->width-maxlen)/2+2);


		box(helpwin,0,0);
		mvwprintw(helpwin,0,(maxlen-strlen(help_message_header))/2,"%s", help_message_header);
		size_t curr_nulls = 0;
		for(int i = 0; i < bindings_n; i++){ 
				if(keybinds[i].help_msg != NULL){ // it's meant to be displayed
						mvwprintw(helpwin,i+1-curr_nulls,3,keybinds[i].help_msg, keybinds[i].key);
				} else curr_nulls++;
		}
		wrefresh(helpwin);

		getch();

		delwin(helpwin);
}
/* Actual program functions */
int 
init(struct sstate* pstate)
{
		assert(pstate && "pstate cannot be null!");
		struct sstate state = {0,0,
				.rMax = START_RMAX,
				.rMin = START_RMIN,
				.iMax = START_IMAX,
				.iMin = START_IMIN,
				.zoom = 1.0,
				false,
		};
		*pstate = state;

		FILE* fp_log = fopen("fractal.log","w");
		FILE* fp_keybinds;
		bool keybind_done;

		if((fp_keybinds = fopen(configfile,"r")))
		{
				LOG(fp_log,"using keybinds from file `%s`", configfile);
				fclose(fp_keybinds); // this was just to check if the file exists.
				using_configfile = true;
				keybind_done = false;

				keybindsreader_init(&kbreader,configfile);
				keybindsreader_parse(&kbreader, keybinds_values_acc, keybinds_values_assoc,
												keybinds_whens_acc,   keybinds_whens_assoc);
				for(int i=0; i < kbreader.ndest;i++){
						for(int j=0; j < bindings_n; j++){
								if(keybinds[j].key == kbreader.dest[i].key){
										keybinds[j].action = kbreader.dest[i].value;
										keybinds[j].help_msg = kbreader.dest[i].helpmsg;
										keybinds[j].when = kbreader.dest[i].when;
										keybind_done = true;
										LOG(fp_log,"substituted keybind in character `%c`", keybinds[j].key);
										break;
								}
						}
						if(!keybind_done){
								keybinds = realloc(keybinds,sizeof(keybinds)+sizeof(struct keybind));
								assert(keybinds != NULL && "malloc failed");
								keybinds[bindings_n].action = kbreader.dest[i].value;
								keybinds[bindings_n].help_msg = kbreader.dest[i].helpmsg;
								keybinds[bindings_n].when = kbreader.dest[i].when;
								bindings_n++;
						}
				}
		}

		LOG(fp_log, "file opened", NULL);
		initscr();
		curs_set(0);
		cbreak();
		clear();
		mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

		//mouseinterval(0);  // Respond to mouse quickly
		keypad(stdscr, TRUE);
		printf("\033[?1003h\n"); // Enable mouse move events (drag)
		size_t i = bindings_n;
		size_t str_len;
		// assert(i > 0 && "help messages/ not present");
		while(i--){
				if(keybinds[i].help_msg && (str_len = strlen(keybinds[i].help_msg)) > maxlen) 
						maxlen = str_len;
		}
		maxlen+=2;
		pstate->log = fp_log ? fp_log : stderr;
		LOG(fp_log, "initialization complete", NULL);
		return 0;
}


bool isrightdrag(struct sstate state)
{
		return state.dragging;
}

bool isleftclick(struct sstate state)
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
ProcessMouse(struct sstate* curr_state)
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
						if(!pstate->dragging){
								for(int i = 0; i < bindings_n; i++){
										if(ch == keybinds[i].key &&
											pstate->dragging == keybinds[i].while_dragging &&
											(!keybinds[i].when || keybinds[i].when(*pstate))){
												keybinds[i].action(pstate);
										}
								}
						}
				}
		}

		return 0;
}

int 
finish(struct sstate* pstate)
{

		printf("\033[?1003l\n");

		if(using_configfile) { 
				keybindsreader_free(&kbreader);
		}
		/*if(pstate->log != stderr && pstate->log != stdout && pstate->log != NULL){
				fclose(pstate->log);
		}*/

		endwin();
		return 0;
}





