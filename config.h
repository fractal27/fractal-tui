#ifndef FRACTAL_CONFIG_H
#define FRACTAL_CONFIG_H
#include "fractal.h"
#include <ncurses.h>

struct keybind keybinds[] = {
		{ .key = 'j', 		.action=StretchDOWN,		.when=NULL,			.help_msg = "[%c] To compress downwards the imaginary values",},
		{ .key = 'J', 		.action=StretchDOWN,		.when=NULL,			.help_msg = "[%c] To stretch downwards the imaginary values",},
		{ .key = 'k', 		.action=StretchUP,			.when=NULL,			.help_msg = "[%c] To compress upwards the imaginary values",},
		{ .key = 'K', 		.action=StretchUP,			.when=NULL,			.help_msg = "[%c] To stretch upwards the imaginary values",},
		{ .key = 'l', 		.action=StretchRIGHT,		.when=NULL,			.help_msg = "[%c] To compress rightwards the real values",},
		{ .key = 'L', 		.action=StretchRIGHT,		.when=NULL,			.help_msg = "[%c] To stretch rightwards the real values",},
		{ .key = 'h', 		.action=StretchLEFT,		.when=NULL,			.help_msg = "[%c] To compress leftwards the real values",},
		{ .key = 'H', 		.action=StretchLEFT,		.when=NULL,			.help_msg = "[%c] To stretch leftwards the real values",},
		{ .key = '+', 		.action=ZoomIn,				.when=NULL,			.help_msg = "[%c] To zoom in",},
		{ .key = '-', 		.action=ZoomOut,			.when=NULL,			.help_msg = "[%c] To zoom out",},
		{ .key = '.', 		.action=ResetZoom,			.when=NULL,			.help_msg = "[%c] To reset the zoom",},
		{ .key = KEY_MOUSE, .action=ProcessMouse,		.when=NULL,			.help_msg = NULL,},
		{ .key = KEY_MOUSE, .action=LimitsTo,			.when=isrightdrag,	.help_msg = "[RIGHTDRAG] To zoom in dynamically",},
		{ .key = KEY_MOUSE, .action=GotoXY,				.when=isleftclick,	.help_msg = "[LEFTCLICK] To center the screen to the point clicked.",},
		{ .key = '?', 		.action=StretchLEFT,		.when=NULL,			.help_msg = "[%c] Show this page",},
};



#endif //  FRACTAL_CONFIG_H
