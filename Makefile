all: redir

redir: redir.c
	gcc -Wall -Wextra -std=c11 -g -o redir redir.c

clean:
	rm -f redir
