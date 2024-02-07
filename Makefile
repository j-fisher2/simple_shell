all: myshell
myshell: myshell.c utility.c
	gcc -o myshell myshell.c utility.c

clean:
	rm -f myshell