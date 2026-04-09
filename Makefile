CC = clang
CFLAGS = -I include/coderep/
EXECUTABLE_FILE = coderep

make: main.o
	$(CC) $(CFLAGS) main.o -o build/$(EXECUTABLE_FILE)

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c

clean:
	rm -f *.o build/$(EXECUTABLE_FILE)

