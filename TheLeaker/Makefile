CC	= gcc
CFLAGS	= -Wall -std=c17 -pedantic
LIBS	= -lm
SDIR 	= ./src
INC = ./inc
ODIR 	= ./obj
BDIR	= ./bin

all: $(BDIR)/theleaker-client $(BDIR)/theleaker-server

$(BDIR)/theleaker-client: $(ODIR)/client.o $(ODIR)/icmphdr.o $(ODIR)/iphdr.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(BDIR)/theleaker-server: $(ODIR)/server.o $(ODIR)/icmphdr.o $(ODIR)/iphdr.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) $< -c -o $@  $(LIBS) -I$(INC)

clean:
	rm $(ODIR)/*.o
