# COMP3430 lab3
##Siwen Sun(7898970)

```asm
[suns5@crow ~/my_lab3]> tree
.
├── atomic-lock.c
├── condition-vars.c
├── lock-really.c
├── makefile
├── official_lock.c
├── output.txt
├── README.md
└── spin-lock.c

0 directories, 8 files
[suns5@crow ~/my_lab3]>
```
There is a makefile, please use it to compile my code by
```asm
[suns5@crow ~/my_lab3]> make
clang -Wall -Wpedantic -Wextra -Werror -o spin-lock spin-lock.c -lpthread
clang -Wall -Wpedantic -Wextra -Werror -o atomic-lock atomic-lock.c -lpthread
clang -Wall -Wpedantic -Wextra -Werror -o lock-really lock-really.c -lpthread
clang -Wall -Wpedantic -Wextra -Werror -o condition-vars condition-vars.c -lpthread
clang -Wall -Wpedantic -Wextra -Werror -o official_lock official_lock.c -lpthread
[suns5@crow ~/my_lab3]>
```

##part1 
Run the three lockers like this
```asm
[suns5@crow ~/my_lab3]> ./spin-lock
The result should be 200000000 but in fact it is 108941573

This program completed normally.
[suns5@crow ~/my_lab3]>
```
```asm
[suns5@crow ~/my_lab3]> ./atomic-lock
The result should be 200000000 but in fact it is 126865356

This program completed normally.
[suns5@crow ~/my_lab3]>
```
```asm
[suns5@crow ~/my_lab3]> time ./lock-really
The result should be 200000000 and in fact it is 200000000

This program completed normally.
36.979u 0.002s 0:18.50 199.8%   0+0k 0+0io 0pf+0w
[suns5@crow ~/my_lab3]>
```

This code use the pthread_mutex_lock which is provided by C99 to compare with my lock. So I name it as official_lock.
```asm
[suns5@crow ~/my_lab3]> time ./official_lock
The result should be 200000000 and in fact it is 200000000

This program completed normally.
7.431u 3.494s 0:05.84 186.9%    0+0k 0+0io 0pf+0w
[suns5@crow ~/my_lab3]>
```

###questions:

1.
when I switch from flag-based spin locks to atomic variables to atomic instructions, nothing changed. System calls are the same.
Yes. the pthread_mutex_lock provided by C99 will use system calls, And I wrote a file call the official_lock.c which uses pthread_mutex_lock 


2. 
I guess that the pthread_mutex_lock will be faster.

After testing : my spin lock used 0:18.50 while the pthread_mutex_lock used 0:05.84


##part2
Run the code like this
```asm
[suns5@crow ~/my_lab3]> ./condition-vars
Hello World
This is lower case printer thread ->  e  l  l  o  o  r  l  d
This is upper case printer thread ->  H  W
Today is not MY DAY
This is lower case printer thread ->  o  d  a  y  i  s  n  o  t
This is upper case printer thread ->  T  M  Y  D  A  Y

```
And you could quit the loop by ctrl+c like this
```asm
[suns5@crow ~/my_lab3]> ./condition-vars
Hello Goodbye
This is lower case printer thread ->  e  l  l  o  o  o  d  b  y  e
This is upper case printer thread ->  H  G
^C
[suns5@crow ~/my_lab3]>
```

###question
If the main thread receives a lot of new data quickly, the main thread will compete with the lower thread for the mutex lock, and the main thread is likely to repeatedly acquire the mutex lock to overwrite the unprocessed data


At the end, you can use make clean to clear codes
```asm
[suns5@crow ~/my_lab3]> make clean
rm -f spin-lock atomic-lock lock-really condition-vars official_lock
[suns5@crow ~/my_lab3]>
```