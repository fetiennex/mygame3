LIBS=-lcsfml-graphics -lcsfml-window -lcsfml-system -lm
CC := gcc -I /usr/local/include -g

all: thegame

build/%.o: %.c
	$(CC) -c $< -o $@
thegame: build/main.o
	@echo "** Building the game"
	$(CC) -o thegame $^ $(LIBS)
clean:
	@echo "** Removing object files and executable..."
	rm -f thegame build/*.o