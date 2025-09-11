
LIBS=`pkgconf --libs ncurses` -lm

fractal: fractal.c
	$(CC) -O2 -o fractal fractal.c $(LIBS)


