# makefile

all: dataserver client

reqchannel.o: reqchannel.H reqchannel.C
	g++ -c -g reqchannel.C

dataserver: dataserver.C reqchannel.o 
	g++ -g -o dataserver dataserver.C reqchannel.o -lpthread

client: client.C reqchannel.o semaphore.H boundbuffer.H
	g++ -g -o client client.C reqchannel.o semaphore.H boundbuffer.H -lpthread
