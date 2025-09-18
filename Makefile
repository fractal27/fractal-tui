
LOGFILE_NAME="fractal.log"
LIBS:=`pkgconf --libs ncurses` -lm
SRCS:=main.c fractal.c keybindingsreader/keybindingsreader.c
OBJS:=main.o fractal.o keybindingsreader.o
EXEC:=fractal-tui
CFLAGS:= -Wno-format-extra-args -DLOGFILE_NAME="$(LOGFILE_NAME)" -O2
all: $(EXEC)

$(OBJS): $(SRCS)
	for i in $(SRCS);do $(CC) -c $(CFLAGS) $$i $(LIBS);done

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

clean:
	rm -f $(LOGFILE_NAME) $(OBJS) $(EXEC)

.PHONY: all clean

#test: test.c
# $(CC) -O2 -o test test.c $(LIBS)
