out:
	clang -Wall -Wpedantic -Wextra -Werror -o ELFReader.out ELFReader.c
	clang -Wall -Wpedantic -Wextra -Werror -o Thread.out Thread.c -lpthread
	clang -Wall -Wpedantic -Wextra -Werror -o Process.out Process.c

clean:
	rm -f Process.out Thread.out ELFReader.out
