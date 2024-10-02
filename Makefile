build:
	gcc -g -std=c99 src/*.c -o main
run:
	./main
clean:
	rm main

.PHONY: git-push

git-push:
	@git add .
	@read -p "commit msg : " msg; \
	git commit -m "$$msg"; \
	git push origin main
