CC=gcc
CFLAGS=-g -Wall


all: visuald64

obj/visuald64.o : src/visuald64.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/diskimage.o : src/diskimage.c 
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

obj/bmp.o : src/bmp.c 
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

	
visuald64: obj/visuald64.o obj/diskimage.o obj/bmp.o
	$(CC) -o  $@ $^ $(LDFLAGS)

clean:
	rm -f visuald64 visuald64.exe
	rm -rf bin obj
