
SRCS:=keybindingsreader.test.c keybindingsreader.c
OBJS:=keybindingsreader.test.o keybindingsreader.o
EXEC:=keybindingsreader_test
CFLAGS:= -O2
all: $(EXEC)

$(OBJS): $(SRCS)
	for i in $(SRCS);do $(CC) -c $(CFLAGS) $$i $(LIBS);done

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(EXEC)

.PHONY: all clean

#test: test.c
# $(CC) -O2 -o test test.c $(LIBS)
