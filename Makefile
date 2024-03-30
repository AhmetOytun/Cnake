build:
	gcc -Wall ./src/*.c -I/opt/homebrew/include -L/opt/homebrew/lib -lSDL2 -o game
run:
	./game
clean:
	rm game