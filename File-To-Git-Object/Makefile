CC = gcc
SRC = git-object.c
DST = mygit-object
CFLAGS = -lcrypto -lz

main: $(SRC)
	$(CC) $(SRC) -o $(DST) $(CFLAGS)

clean:
	rm -f $(DST)

.PHONY: clean
