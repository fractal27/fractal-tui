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

enum Keybind {
		StretchUP,
		StretchDOWN,
		StretchLEFT,
		StretchRIGHT,
		CompressUP,
		CompressDOWN,
		CompressLEFT,
		CompressRIGHT,
		MoveUP,
		MoveDOWN,
		MoveLEFT,
		MoveRIGHT,
		GotoXY,
		LimitsTo,
		ZoomIn,
		ZoomOut,
		ResetZoom,
		ProcessMouse
};

struct keybind {
		int key;
		enum Keybind action;
		bool (*when)(struct sstate state, int key); // CAN BE NULL(it means that it will be all the time)
		const char* help_msg;
};






extern int init(struct sstate* pstate);
extern int mainloop(struct sstate* pstate);
extern int main(int argc, char** argv);
extern int finish(struct sstate* pstate);
extern bool isrightdrag(struct sstate state, int key);
extern bool isleftclick(struct sstate state, int key); 


extern const char charset[];
extern const size_t charset_len;


extern const char* help_message_header;
extern struct keybind keybinds[];
extern size_t maxlen;
extern size_t help_message_nlines;





#endif // FRACTALTUI_FRACTAL_H
