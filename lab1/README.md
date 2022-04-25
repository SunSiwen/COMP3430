# COMP3430 lab1
##Siwen Sun(7898970)

```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/lab1$ tree
.
├── README.md
├── makefile
├── pe.c
├── processes.c
└── threads.c

0 directories, 10 files
```
There is a makefile, please use it to compile my code by
```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/lab1$ make
clang -Wall -Wpedantic -Wextra -Werror -o pe.out pe.c
clang -Wall -Wpedantic -Wextra -Werror -o threads.out threads.c -lpthread -libcurl
clang -Wall -Wpedantic -Wextra -Werror -o processes.out processes.c
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/lab1$
```

###Question1

####part1 process
The code is pe.out which needs the path of your code. such as
```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/lab1$ ./pe.out putty.exe
File: putty.exe

Machine type: 0x8664
Number of sections: 7
Created: Sat Jul 10 09:51:55 2021
Symbol table start: 0x0x00000000 (should be 0)
Number of symbols: 0 (should be 0)
Size of optional header: 240
```

###Question2 processes
You need input a number as the input like this:
```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/lab1$ ./processes.out 8
I am parent process, and my pid is 211
I am child process, and my pid is 217
I am child process, and my pid is 214
I am child process, and my pid is 213
I am child process, and my pid is 219
I am child process, and my pid is 212
I am child process, and my pid is 216
I am child process, and my pid is 218
I am child process, and my pid is 215

Parent process completed normally.
```
####What system calls have you written into your program? (see the system call table in the Linux kernel)
Answer: nanosleep; kill; clone; getppid; getpid; rt_sigaction

####What system calls does your program actually make? (use the strace command to launch your program, see man strace) Based on what you’ve seen in class: where do these system calls come from? Your code isn’t making those system calls, so who is? Note: strace prints to standard error by default and prints a lot of information. Look carefully at the man page for strace to see if there’s a way to get it to write somewhere else (like a file).
I was using ```strace -o output.txt -f -T -tt -e trace=all ./processes.out 8```

execve brk mmap access stat open read fstat mprotect close arch_prctl mprotect : from terminal by enter ./processes.out 8

nanosleep : from sleep()

kill: from kill()

clone: from fork()

getpid: from getpid()

getppid: from getppid()

re_sigaction: from signal()



###question3 threads
It is similar to part1
```asm
[suns5@grouse ~/my_lab1]> ./threads.out
+d$(SR(-Q7>.8F}d]]%(,Al{\##4m^kuElzu@%"p9?{Pby5AV9GaX5_65_I"?6wc#qXBsY3-u0\Z)nx_(?B_S!rh_=i!P`aRR<rG
r%Sj20D;{<y#[;_.<Sszm]x@?\onua6gefQwst2q1-qkESx`(n[uKS5g/$X&emkJU>CI5R<C\/0$_(cgs>\@qn*"qa&XNp##0EKB
[suns5@grouse ~/my_lab1]>
```

```
[suns5@grouse ~/my_lab1]> tree
.
├── downloadFile
├── makefile
├── pe.c
├── pe.out
├── processes.c
├── processes.out
├── threads.c
└── threads.out

0 directories, 8 files

```
You can see that a downloadFile was generated.

At the end, you can use make clean to clear .out codes
```asm
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/lab1$ make clean
rm -f pe.out threads.out processes.out
siwen@DESKTOP-61TLL0I:/mnt/c/Users/Administrator/CLionProjects/comp343/lab1$
```