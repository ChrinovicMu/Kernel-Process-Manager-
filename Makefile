build:
	gcc -g -std=c99 src/*.c -o main
run:
	./main
clean:
	rm main
