build:
	gcc -pg -g -std=c99 src/*.c -pthread -o main
run:
	./main
	gprof main gmon.out > profile_report.txt
clean:
	rm main

.PHONY: git-push

git-push:
	@git add .
	@read -p "commit msg : " msg; \
	git commit -m "$$msg"; \
	git push origin main
