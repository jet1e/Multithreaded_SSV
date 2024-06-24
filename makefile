CC = gcc
CFLAGS = -Wall -pedantic -ansi -fsanitize=thread -g
OBJ = mssv.o child.o
EXEC = mssv

$(EXEC) : $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -fsanitize=thread

mssv.o : mssv.c mssv.h child.h
	$(CC) -c mssv.c $(CFLAGS)

child.o : child.c mssv.h child.h
	$(CC) -c child.c $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJ)