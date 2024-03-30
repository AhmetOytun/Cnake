build:
	gcc -Wall ./src/*.c -I/opt/homebrew/include -L/opt/homebrew/lib -I/opt/homebrew/Cellar/sdl2_ttf/2.22.0/include -L/opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib -L/opt/homebrew/Cellar/sdl2_mixer/2.8.0/lib -I/opt/homebrew/Cellar/sdl2_mixer/2.8.0/include -lSDL2 -lSDL2_ttf -lSDL2_mixer -o game
run:
	./game
clean:
	rm game