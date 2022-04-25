out:
	clang -Wall -Wpedantic -Wextra -Werror -o spin-lock spin-lock.c -lpthread
	clang -Wall -Wpedantic -Wextra -Werror -o atomic-lock atomic-lock.c -lpthread
	clang -Wall -Wpedantic -Wextra -Werror -o lock-really lock-really.c -lpthread
	clang -Wall -Wpedantic -Wextra -Werror -o condition-vars condition-vars.c -lpthread
	clang -Wall -Wpedantic -Wextra -Werror -o official_lock official_lock.c -lpthread

clean:
	rm -f spin-lock atomic-lock lock-really condition-vars official_lock
