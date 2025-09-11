#include <curses.h>
#include <stdbool.h>
#include <math.h>

#define MAX_VALUE   65


// var definition
//

int height;
int width;
double rMax = .2;
double rMin = -1;
double iMax = 2.;
double iMin = -2.;

const char charset[] = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/*tfjrxnuvczYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";




int 
init()
{
		initscr();
		curs_set(0);
		clear();
		return 0;
}

char
get_usable_char(uint16_t x){
		if(x > sizeof(charset) - 2)
				x = sizeof(charset) - 2;
		return charset[x];
}

double
get_scale(int maxx, int maxy)
{
		return maxx/(rMax-rMin) < maxy/(iMax-iMin) ? maxx/(rMax-rMin): maxy/(iMax-iMin);
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
		int x, y;
		size_t charset_len = sizeof(charset)-1;
		char c;
		double scale, real, imag;

		clear();
		box(stdscr,0,0);
		while(running){
				getmaxyx(stdscr,height,width);
				mvprintw(0,1,"height,width: %d, %d",height,width);
				for(x=1;x<width-1;x++){
						for(y=1;y<height-1;y++){
								real = rMin + (x - 1) * (rMax - rMin) / (width - 2);
								imag = iMin + (y - 1) * (iMax - iMin) / (height - 2);

								scale = get_scale(width, height);
								c = get_usable_char((int)(mantelbrot(real, imag)*charset_len));
								mvprintw(y,x,"%c",c);
								// mvprintw(0,60,"usable char: `%d`",c);
						}
				}
				refresh();
		}
		
		return 0;
}

int 
finish()
{
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


