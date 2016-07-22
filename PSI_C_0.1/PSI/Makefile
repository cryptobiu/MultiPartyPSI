CFLAGS = -arch x86_64 -g -w -Os -std=c99 -I/usr/local/ssl/include  -I./headers -I/usr/local/lib  -I/usr/local/include -pthread -fno-stack-protector
LD=-L /usr/lib  -lssl -lcrypto  -L /usr/local/lib/
CC = gcc
CLEANFILES = ./*.o
PROGS= psiClient psiServer
OFS=AESRandom.o BF.O BitMatrix.o BuilderTasks.o chtbl.o GarbledBF.o list.o MessageDigest.o MTBuilder.o networkComm.o NPOTRcvr.o NPOTSndr.o OTExtRcvr.o OTExtSndr.o RandomSource.o RangeHash.o Util.o

all: struc psiClient psiServer

struc:./PSI/*.c ./OT/*.c ./Threads/*.c ./client/client.c ./server/server.c
	${CC} ${CFLAGS} -c $^

clientMain.o: ./client/main.c
	${CC} ${CFLAGS} -c -o $@ $^

psiClient:clientMain.o client.o clientTasks.o ${OFS}
	${CC} ${CFLAGS} -o $@ $^ $(LD)

serverMain.o: ./server/main.c
	${CC} ${CFLAGS} -c -o $@ $^

psiServer:serverMain.o server.o serverTasks.o ${OFS}
	${CC} ${CFLAGS} -o $@ $^ $(LD)

clean:
	rm -f ${CLEANFILES}
	rm -f ${PROGS}
	rm -f *~
