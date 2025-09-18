#ifndef FRACTALTUI_FRACTAL_H
#define FRACTALTUI_FRACTAL_H
#include <stdio.h>
#include <stdbool.h>


#define calculate_re(state,x) (state.rMin + (x - 1) * (state.rMax - state.rMin) / (state.width - 2))
#define calculate_im(state,y) (state.iMin + (y - 1) * (state.iMax - state.iMin) / (state.height - 2))
#define SPEED 0.1

#ifndef LOGFILE_NAME
#define LOGFILE_NAME "fractal.log"
#endif


#ifndef START_RMAX
#define START_RMAX  0.5
#endif

#ifndef START_RMIN
#define START_RMIN -1.5
#endif

#ifndef START_IMAX
#define START_IMAX  1.
#endif

#ifndef START_IMIN
#define START_IMIN -1.
#endif
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
};

struct keybind {
		int key;
		void (*action)(struct sstate* pstate);
		bool while_dragging;
		bool (*when)(struct sstate state); // CAN BE NULL(it means that it will be all the time)
		const char* help_msg;
};



/* Keybind Actions */

extern void StretchUP(struct sstate* pstate);
extern void StretchDOWN(struct sstate* pstate);
extern void StretchLEFT(struct sstate* pstate);
extern void StretchRIGHT(struct sstate* pstate);
extern void CompressUP(struct sstate* pstate);
extern void CompressDOWN(struct sstate* pstate);
extern void CompressLEFT(struct sstate* pstate);
extern void CompressRIGHT(struct sstate* pstate);
extern void MoveUP(struct sstate* pstate);
extern void MoveDOWN(struct sstate* pstate);
extern void MoveLEFT(struct sstate* pstate);
extern void MoveRIGHT(struct sstate* pstate);
extern void GotoXY(struct sstate* pstate);
extern void LimitsTo(struct sstate* pstate);
extern void ZoomIn(struct sstate* pstate);
extern void ZoomOut(struct sstate* pstate);
extern void ResetZoom(struct sstate* pstate);
extern void ProcessMouse(struct sstate* pstate);
extern void ShowHelp(struct sstate* pstate);
extern void Clear(struct sstate* pstate);
extern void Finish(struct sstate* pstate);






/* Functions relative to the program */

extern int init(struct sstate* pstate);
extern int mainloop(struct sstate* pstate);
extern int main(int argc, char** argv);
extern int finish(struct sstate* pstate);
extern bool isrightdrag(struct sstate state);
extern bool isleftclick(struct sstate state); 
extern void zoom(struct sstate* pstate, double x); 


/* global constants */

// relative to charset used to display ascii
extern const char charset[];
extern const size_t charset_len;

// relative to helpmsgs/keybinds
extern const char* help_message_header;
extern struct keybind keybinds_s[];
extern struct keybind* keybinds;
extern size_t keybinds_n;
extern size_t maxlen; // the only non-constant that has to be calculated in a function




#endif // FRACTALTUI_FRACTAL_H
