ASM_SOURCES = $(wildcard src/*.asm)
ASM_OBJECTS = $(ASM_SOURCES:.asm=.o)
C_SOURCES = $(wildcard src/*.c)
C_OBJECTS = $(C_SOURCES:.c=.o)

build: $(ASM_OBJECTS) $(C_OBJECTS)
	gcc -g $(ASM_OBJECTS) $(C_OBJECTS) -o main

%.o: %.c
	gcc -g -std=c99 -c $< -o $@

%.o: %.asm
	nasm -f elf64 $< -o $@

run:
	./main

clean:
	rm -f main src/*.o

test: build 
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all --verbose ./main

.PHONY: git-push
git-push:
	@git add .
	@read -p "commit msg : " msg; \
	git commit -m "$$msg"; \
	git push origin main
