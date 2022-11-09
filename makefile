CC=       	gcc
CFLAGS= 	-Wall -Wextra -pedantic -ggdb -I.
DEPS= 		$(wildcard *.h)

practicum1: practicum1.c
	@echo "Compiling program..."
	$(CC) practicum1.c -o practicum1 $(CFLAGS)

clean:
	@echo "Removing extraneous files..."
	rm *.o A5.4
