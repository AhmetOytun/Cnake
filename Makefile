build:
	gcc -Wall ./src/*.c -I/opt/homebrew/include -L/opt/homebrew/lib -I/opt/homebrew/Cellar/sdl2_ttf/2.22.0/include -L/opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib -lSDL2 -lSDL2_ttf -o game
run:
	./game
clean:
	rm game