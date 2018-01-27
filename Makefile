CFLAGS=-c -Wall -O2
LIBS = -lm -lpthread

all: libaccel.a

libaccel.a: accel.o
	ar -rc libaccel.a accel.o ;\
	sudo cp libaccel.a /usr/local/lib ;\
	sudo cp accel.h /usr/local/include

accel.o: accel.c
	$(CC) $(CFLAGS) accel.c

clean:
	rm *.o libaccel.a
