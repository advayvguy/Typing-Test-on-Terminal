CC = clang
CFLAGS = -w -Iv1/include

.PHONY: clean

typing-test:
	@$(CC) $(CFLAGS) v1/src/*.c -o typing-test

clean:
	@rm -f typing-test
