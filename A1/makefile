out:
	clang -Wall -Wpedantic -Wextra -Werror -o a1-elf a1-elf.c
	clang -Wall -Wpedantic -Wextra -Werror -o a1-before before.c
	clang -Wall -Wpedantic -Wextra -Werror -o a1-threads a1-threads.c -lpthread
	clang -Wall -Wpedantic -Wextra -Werror -o a1-procs a1-procs.c

clean:
	rm -f a1-elf a1-threads a1-procs
