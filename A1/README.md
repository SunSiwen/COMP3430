# COMP3430 Assignment1
##Siwen Sun(7898970)

```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$ tree
.
├── README.md
├── a1-elf.c
├── a1-procs.c
├── a1-threads.c
└── makefile

0 directories, 5 files
```
There is a makefile, please use it to compile my code by
```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$ make
clang -Wall -Wpedantic -Wextra -Werror -o a1-elf a1-elf.c
clang -Wall -Wpedantic -Wextra -Werror -o a1-threads a1-threads.c -lpthread
clang -Wall -Wpedantic -Wextra -Werror -o a1-procs a1-procs.c
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$
```

###Question1

####part1 process
The code is a1-elf which needs the path of your code. such as
```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$ ./a1-elf hello.out64
ELF elfHeader:
  * 64-bit
  * little endianness
  * compiled for 0x00 (operating system)
  * entry point address 0x0000000000401040
  * program elfHeader table starts at 0x0000000000000040
  * there are 11 program headers, each is 56 bytes
  * there are 29 section headers, each is 64 bytes
  * the section elfHeader string table is 28


Program header #0:
  * segment type 0x00000006
  * virtual address of segment 0x0040004000000000
  * size in file 2645699854336 bytes
  * first up to 32 bytes starting at 0x0040004000000000:
```

you can also use my code's path as input parameter
```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$ ./a1-elf a1-elf
ELF elfHeader:
  * 64-bit
  * little endianness
  * compiled for 0x00 (operating system)
  * entry point address 0x00000000004010a0
  * program elfHeader table starts at 0x0000000000000040
  * there are 11 program headers, each is 56 bytes
  * there are 29 section headers, each is 64 bytes
  * the section elfHeader string table is 28


Program header #0:
  * segment type 0x00000006
  * virtual address of segment 0x0040004000000000
  * size in file 2645699854336 bytes
  * first up to 32 bytes starting at 0x0040004000000000:
```

###Question2
####part1 processes
You do not need to input any parameter but create a file called config.txt as the demo shows inside the same directory by vim

```siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$ vim config.txt```
press i into edit mode and input a number and use esc + :wq to save the change

and run
```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$ ./a1-procs
Reading config.txt file
I am 71
Change setting to 8
72 is starting
73 is starting
74 is starting
75 is starting
76 is starting
77 is starting
78 is starting
79 is starting

```

in another terminal window

```
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator$ kill -HUP 71
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator$
```

```asm
Reading config.txt file
Change setting to 8
Same Same
```
You can also change the number inside config.txt
try again
```asm
Reading config.txt file
Change setting to 12
94 is starting
95 is starting
96 is starting
97 is starting
```

change to 2 and kill -HUP again
```asm
Reading config.txt file
Change setting to 2
97 is being killed
97 is exiting
96 is being killed
96 is exiting
95 is being killed
95 is exiting
94 is being killed
94 is exiting
79 is being killed
79 is exiting
78 is being killed
78 is exiting
77 is being killed
77 is exiting
76 is being killed
76 is exiting
75 is being killed
75 is exiting
74 is being killed
74 is exiting
```
finally ctrl+c to end the program
```asm
^C73 is exiting
72 is exiting
72 is being killed
73 is being killed
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$
```

####part2 threads
It is similar to part1
```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$ ./a1-threads
Reading config.txt file
I am 237
Change setting to 4
Starting 0
Starting 1
Thread 0 has started
Starting 2
Thread 1 has started
Starting 3
Thread 2 has started
Thread 3 has started
```

change the config.txt and kill -HUP
```asm
Reading config.txt file
Change setting to 12
Starting 4
Starting 5
Thread 4 has started
Starting 6
Thread 5 has started
Starting 7
Thread 6 has started
Starting 8
Thread 7 has started
Starting 9
Thread 8 has started
Starting 10
Thread 9 has started
Starting 11
Thread 10 has started
Thread 11 has started
```

change the config.txt to 2 and kill -HUP
```asm
Reading config.txt file
Change setting to 2
Stopping 11
Thread 11 is going to a better place
Stopping 10
Thread 10 is going to a better place
Stopping 9
Thread 9 is going to a better place
Stopping 8
Thread 8 is going to a better place
Stopping 7
Thread 7 is going to a better place
Stopping 6
Thread 6 is going to a better place
Stopping 5
Thread 5 is going to a better place
Stopping 4
Thread 4 is going to a better place
Stopping 3
Thread 3 is going to a better place
Stopping 2
Thread 2 is going to a better place
```

and exit code by ctrl+c
```asm
^CStopping 0
Thread 0 is going to a better place
Stopping 1
Thread 1 is going to a better place
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$
```

At the end, you can use make clean to clear codes
```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$ make clean
rm -f a1-elf a1-threads a1-procs
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/A1$
```