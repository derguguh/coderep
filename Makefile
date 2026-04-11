CC = clang
CFLAGS = -I include/
EXECUTABLE_FILE = coderep

make: main.o string.o brute.o
	$(CC) $(CFLAGS) main.o string.o brute.o -o build/$(EXECUTABLE_FILE)

main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c

string.o: src/data_types/string.c
	$(CC) $(CFLAGS) -c src/data_types/string.c

brute.o: src/data_types/string.c
	$(CC) $(CFLAGS) -c src/data_types/brute.c

clean:
	rm -f *.o build/$(EXECUTABLE_FILE)

