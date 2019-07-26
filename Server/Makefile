Server:	Server.o parser.o command.o
	gcc -o  Server Server.o parser.o command.o

Server.o: Server.c parser.h command.h 
	gcc $(CFLAGS) -Werror -Wall -pedantic -c -w Server.c

parser.o: parser.h parser.c
	gcc $(CFLAGS) -Werror -Wall -pedantic -c -w parser.c

command.o: command.h command.c parser.h
	gcc $(CFLAGS) -Werror -Wall -pedantic -c -w command.c

clean:
	rm	-f	*.o	Server.o parser.o command.o

