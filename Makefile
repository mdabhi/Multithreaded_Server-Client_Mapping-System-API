EXEC=runmap
#MAIN=main
MAIN=multiserver
CLIENT=clients
MAPPER=roadmap

CC=gcc
CFLAGS= -Wall -Wextra

all: $(EXEC) $(CLIENT)

$(EXEC): $(MAIN).c $(MAPPER).c
	$(CC) $(CFLAGS) -o $(EXEC) $(MAIN).c $(MAPPER).c -lpthread

$(CLIENT): $(CLIENT).c
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT).c

clean:
	rm -f $(EXEC)
	rm -f $(CLIENT)
