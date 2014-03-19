CC := $(CROSS_COMPILE)gcc
LIBS := -lpthread

all:
	mkdir -p client_part
	mkdir -p server_part
	$(CC) -o client client.c $(LIBS)
	$(CC) -o server server.c $(LIBS)
	mv client client_part
	mv server server_part
	cp -p Document.txt server_part
clean:
	rm -r ./client_part
	rm -r ./server_part