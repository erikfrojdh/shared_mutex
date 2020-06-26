CC			?= gcc
CFLAGS		+= -Wall -Wextra -O3 -std=gnu99
LDLIBS		+= -pthread


all: a b

a:  A.o
	$(CC) -o a A.c $(CFLAGS)  $(LDLIBS)  
b:  B.o
	$(CC) -o b B.c $(CFLAGS)  $(LDLIBS)  

clean:
	rm a A.o b B.o