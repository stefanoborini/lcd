all: lcd

CC = gcc
LD = gcc
CFLAGS = -Wall -O 
LDFLAGS =  -L/usr/lib -lm
LIBS = 

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o *~ *.bak

distclean: clean
	rm -f lcd
install: 
	

OBJS = lcd.o

lcd:	$(OBJS)
	$(LD) $(LDFLAGS) -o lcd $(OBJS) $(LIBS)

lcd.o:	lcd.c
