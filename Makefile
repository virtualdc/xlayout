all: xlayout

clean:
	rm -vf xlayout

xlayout: main.c
	gcc -o xlayout -O2 main.c -lX11

