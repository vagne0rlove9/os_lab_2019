all: tcp udp

tcp: tcpclient tcpserver

udp: udpclient udpserver

tcpclient: tcpclient.c
	gcc -o tcpclient.o tcpclient.c

udpclient: udpclient.c
	gcc -o udpclient.o udpclient.c

tcpserver: tcpserver.c
	gcc -o tcpserver.o tcpserver.c

udpserver: udpserver.c
	gcc -o udpserver.o udpserver.c

clean:
	rm *.out *.o -f
	clear