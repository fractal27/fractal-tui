#ifndef FRACTAL_CONFIG_H
#define FRACTAL_CONFIG_H
#include "fractal.h"
#include <ncurses.h>

//not const because of configuration
struct keybind keybinds_s[] = {
		{ .key = 'j', 			.action=CompressDOWN,	.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To compress downwards the imaginary values",},
		{ .key = 'J', 			.action=StretchDOWN,	.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To stretch downwards the imaginary values",},
		{ .key = 'k', 			.action=CompressUP,		.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To compress upwards the imaginary values",},
		{ .key = 'K', 			.action=StretchUP,		.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To stretch upwards the imaginary values",},
		{ .key = 'l', 			.action=CompressRIGHT,	.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To compress rightwards the real values",},
		{ .key = 'L', 			.action=StretchRIGHT,	.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To stretch rightwards the real values",},
		{ .key = 'h', 			.action=CompressLEFT,	.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To compress leftwards the real values",},
		{ .key = 'H', 			.action=StretchLEFT,	.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To stretch leftwards the real values",},
		{ .key = KEY_LEFT, 		.action=MoveLEFT,		.while_dragging=false,	.when=NULL,			.help_msg = NULL},
		{ .key = KEY_UP, 		.action=MoveUP,			.while_dragging=false,	.when=NULL,			.help_msg = NULL},
		{ .key = KEY_DOWN, 		.action=MoveDOWN,		.while_dragging=false,	.when=NULL,			.help_msg = NULL},
		{ .key = KEY_RIGHT, 	.action=MoveRIGHT,		.while_dragging=false,	.when=NULL,			.help_msg = NULL},
		{ .key = '+', 			.action=ZoomIn,			.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To zoom in",},
		{ .key = '-', 			.action=ZoomOut,		.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To zoom out",},
		{ .key = '.', 			.action=ResetZoom,		.while_dragging=false,	.when=NULL,			.help_msg = "[%c] To reset the zoom",},
		{ .key = KEY_MOUSE, 	.action=ProcessMouse,	.while_dragging=false,	.when=NULL,			.help_msg = NULL,},
		{ .key = KEY_MOUSE, 	.action=NULL,			.while_dragging=true,	.when=NULL,			.help_msg = "[RIGHTDRAG] To zoom in dynamically",},
		{ .key = KEY_MOUSE, 	.action=NULL,			.while_dragging=false,	.when=isleftclick,	.help_msg = "[LEFTCLICK] To center the screen to the point clicked.",},
		{ .key = '?', 			.action=ShowHelp,		.while_dragging=false,  .when=NULL,			.help_msg = "[%c] Show this page",},
		{ .key = 'c', 			.action=Clear,		    .while_dragging=false,  .when=NULL,			.help_msg = "[%c] Force clear the screen",},
		{ .key = 'q', 			.action=Finish,			.while_dragging=false,  .when=NULL,			.help_msg = "[%c] Show this page",},
};

struct keybind* keybinds = &(keybinds_s[0]);

#endif //  FRACTAL_CONFIG_H
