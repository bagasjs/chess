CC := clang
CFLAGS := -Wall -Wextra

main.exe: ./main.c ./chess.c
	$(CC) $(CFLAGS) -o $@ $^
