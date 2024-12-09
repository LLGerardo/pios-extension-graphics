CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = 

SOURCES = src/main.c src/framebuffer.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = pios_graphics

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)