out:
	clang -Wall -Wpedantic -Wextra -Werror -o pe.out pe.c
	clang -Wall -Wpedantic -Wextra -Werror -o threads.out threads.c -lpthread -lcurl
	clang -Wall -Wpedantic -Wextra -Werror -o processes.out processes.c
clean:
	rm -f pe.out threads.out processes.out
