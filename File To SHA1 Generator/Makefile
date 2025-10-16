CC = gcc
SRC = sha-convert.c
DST = sha1_file
CFLAGS = -lcrypto

main: $(SRC)
	$(CC) $(SRC) -o $(DST) $(CFLAGS)

clean:
	rm -f $(DST)