# Assignment1

# How to run my code

According to the requirements, you can run them easily by command "**make**" like this :

```
sunsiwen@192 A1 % make
clang -Wall -Wpedantic -Wextra -Werror -o ELFReader.out ELFReader.c
clang -Wall -Wpedantic -Wextra -Werror -o thread.out thread.c -lpthread
clang -Wall -Wpedantic -Wextra -Werror -o process.out process.c
sunsiwen@192 A1 % 
```

All .c files are compiled and generated .out files

And then you could test my code

## Question 1:
If you want to use your file, please input the file path like "**./ELFReader.out yourfilepath**". For example, I put the hello.out32 in this doucument, so that I can run my code like this:

```
sunsiwen@192 A1 % ./ELFReader.out hello.out32
ELF elfHeader:
  * 32-bit
  * little endianness
  * compiled for 0x00 (operating system)
  * has type 0x02
  * compiled for 0x03 (isa)
  * entry point address 0x0000000008049060
  * program elfHeader table starts at 0x0000000000000034
  * there are 11 program headers, each is 32 bytes
  * there are 29 section headers, each is 40 bytes
  * the section elfHeader string table is 28


Program header #0:
  * segment type 0x00000006
  * virtual address of segment 0x0000000008048034
  * size in file 352 bytes
  * first up to 32 bytes starting at 0x0000000000000034:
	06 00 00 00 34 00 00 00 34 80 04 08 34 80 04 08 
	60 01 00 00 60 01 00 00 04 00 00 00 04 00 00 00 




Program header #1:
  * segment type 0x00000003
  * virtual address of segment 0x0000000008048194
  * size in file 19 bytes
  * first up to 32 bytes starting at 0x0000000000000194:
	2f 6c 69 62 2f 6c 64 2d 6c 69 6e 75 78 2e 73 6f 
	2e 32 00 




Program header #2:
  * segment type 0x00000001
  * virtual address of segment 0x0000000008048000
  * size in file 748 bytes
  * first up to 32 bytes starting at 0x0000000000000000:
	7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00 
	02 00 03 00 01 00 00 00 60 90 04 08 34 00 00 00 

.
.
.
//Too many lines, so I delete a part of them
.
.
.

Section header #23
  * section name .data
  * type 0x01
  * virtual address of section 0x000000000804c014
  * size in file 8 bytes
  * first up to 32 bytes starting at 0x0000000000003014:
	00 00 00 00 00 00 00 00 
	

Section header #24
  * section name .bss
  * type 0x08
  * virtual address of section 0x000000000804c01c
  * size in file 4 bytes
  * first up to 32 bytes starting at 0x000000000000301c:
	47 43 43 3a 
	

Section header #25
  * section name .comment
  * type 0x01
  * virtual address of section 0x0000000000000000
  * size in file 73 bytes
  * first up to 32 bytes starting at 0x000000000000301c:
	47 43 43 3a 20 28 55 62 75 6e 74 75 20 39 2e 33 
	2e 30 2d 31 37 75 62 75 6e 74 75 31 7e 32 30 2e 

Section header #26
  * section name .symtab
  * type 0x02
  * virtual address of section 0x0000000000000000
  * size in file 1024 bytes
  * first up to 32 bytes starting at 0x0000000000003068:
	00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
	00 00 00 00 94 81 04 08 00 00 00 00 03 00 01 00 

Section header #27
  * section name .strtab
  * type 0x03
  * virtual address of section 0x0000000000000000
  * size in file 506 bytes
  * first up to 32 bytes starting at 0x0000000000003468:
	00 63 72 74 73 74 75 66 66 2e 63 00 64 65 72 65 
	67 69 73 74 65 72 5f 74 6d 5f 63 6c 6f 6e 65 73 

Section header #28
  * section name .shstrtab
  * type 0x03
  * virtual address of section 0x0000000000000000
  * size in file 257 bytes
  * first up to 32 bytes starting at 0x0000000000003662:
	00 2e 73 79 6d 74 61 62 00 2e 73 74 72 74 61 62 
	00 2e 73 68 73 74 72 74 61 62 00 2e 69 6e 74 65 

```

**For hello.out 64, use "./ELFReader.out hello.out64"**

```

sunsiwen@192 A1 % ./ELFReader.out hello.out64
ELF elfHeader:
  * 64-bit
  * little endianness
  * compiled for 0x00 (operating system)
  * has type 0x02
  * compiled for 0x3e (isa)
  * entry point address 0x0000000000401040
  * program elfHeader table starts at 0x0000000000000040
  * there are 11 program headers, each is 56 bytes
  * there are 29 section headers, each is 64 bytes
  * the section elfHeader string table is 28


Program header #0:
  * segment type 0x00000006
  * virtual address of segment 0x0000000000400040
  * size in file 616 bytes
  * first up to 32 bytes starting at 0x0000000000000040:
	06 00 00 00 04 00 00 00 40 00 00 00 00 00 00 00 
	40 00 40 00 00 00 00 00 40 00 40 00 00 00 00 00 




Program header #1:
  * segment type 0x00000003
  * virtual address of segment 0x00000000004002a8
  * size in file 28 bytes
  * first up to 32 bytes starting at 0x00000000000002a8:
	2f 6c 69 62 36 34 2f 6c 64 2d 6c 69 6e 75 78 2d 
	78 38 36 2d 36 34 2e 73 6f 2e 32 00 

.
.
.
//Too many lines, so I delete a part of them
.
.
.

Section header #23
  * section name .data
  * type 0x01
  * virtual address of section 0x0000000000404020
  * size in file 16 bytes
  * first up to 32 bytes starting at 0x0000000000003020:
	00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
	

Section header #24
  * section name .bss
  * type 0x08
  * virtual address of section 0x0000000000404030
  * size in file 8 bytes
  * first up to 32 bytes starting at 0x0000000000003030:
	47 43 43 3a 20 28 55 62 
	

Section header #25
  * section name .comment
  * type 0x01
  * virtual address of section 0x0000000000000000
  * size in file 73 bytes
  * first up to 32 bytes starting at 0x0000000000003030:
	47 43 43 3a 20 28 55 62 75 6e 74 75 20 39 2e 33 
	2e 30 2d 31 37 75 62 75 6e 74 75 31 7e 32 30 2e 

Section header #26
  * section name .symtab
  * type 0x02
  * virtual address of section 0x0000000000000000
  * size in file 1464 bytes
  * first up to 32 bytes starting at 0x0000000000003080:
	00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
	00 00 00 00 00 00 00 00 00 00 00 00 03 00 01 00 

Section header #27
  * section name .strtab
  * type 0x03
  * virtual address of section 0x0000000000000000
  * size in file 459 bytes
  * first up to 32 bytes starting at 0x0000000000003638:
	00 63 72 74 73 74 75 66 66 2e 63 00 64 65 72 65 
	67 69 73 74 65 72 5f 74 6d 5f 63 6c 6f 6e 65 73 

Section header #28
  * section name .shstrtab
  * type 0x03
  * virtual address of section 0x0000000000000000
  * size in file 259 bytes
  * first up to 32 bytes starting at 0x0000000000003803:
	00 2e 73 79 6d 74 61 62 00 2e 73 74 72 74 61 62 
	00 2e 73 68 73 74 72 74 61 62 00 2e 69 6e 74 65 

```

## Qustion 2
You have to input 2 parameters, the first one is the total number of threads or processes, the second one is limit of processes or threads could run at a time.

So you can run them by "**./Process.out totalnumber limitnumber**" like this (for example, make totalnumber 100 and limitnumber 10):
```
sunsiwen@192 A1 % ./Process.out 100 10
In process experiment, system creates 100 processes in total, and run 10 processes at a time, The total time is 0:17843000
```

`and **"./Thread.out totalnumber limitnumber"**
```
sunsiwen@192 A1 % ./Thread.out 100 10
In thread experiment, system creates 100 threads in total, and run 10 threads at a time, The total time is 0:1890000
```

# Report for question 2

## Hypothesis

I hold the view that processes take longer time to be created for costing much more resources of computer.

## data


**On linux**

| Total processes/ threads (x)  | Concurrent processes/ threads (y)  | Threads time  | processes time  |
|:----------|:----------|:----------|:----------|
| 100    | 4   | 0.2184s    | 0.2214s    |
| 100    | 8    | 0.1663s    | 0.2199s    |s
| 100    | 32    | 0.1454s    | 0.1942s    |
| 1000    | 4    | 0.2186s    | 0.1939s    |
| 1000    | 8    | 0.1308s    | 0.1930s   |
| 1000    | 32    | 0.1078s    | 0.1397s    |
| 10000    | 4    | 2.179s   | 1.8641s    |
| 10000    | 8    | 1.8971s    | 1.6888s    |
| 10000    | 32    | 1.2341s    | 1.1716s    |


### Which is faster: threads or processes?

Gernrally speaking, process is faster than thread under huge anoumt of task. Threads performance better than threads when there are not much tasks. 


### Does your result match your hypothesis? Why do you think it does, or why do you think it does not? You can refer to our textbook here in terms of what happens when threads and processes are launched.

I think my hypothesis is right in some extend. Thread is a part of a process which means it is more lightweight than a process so it takes less resources.
But I did not consider the cost on context change. when os changes the context, the time are wasted, which may be longer than the time used to calculate.
And by the code, the hard-work is computing intensive. So it is really take to much time on change threads.

### Is there a point of diminishing returns on how many concurrent processes/threads are running? That is, when does execution time start to grow very quickly as you increase the number of concurrent processes/threads (y)? Try to explain what might be happening here. (Note: you’re probably going to have to use y > 32 to get to this point of diminishing returns)

Yes, it is. When I increase the current value(y), the time drops first and then increases. As I said before : when os changes the context, the time are wasted, which may be longer than the time used to calculate.
And by the code, the hard-work is computing intensive. So it is really take to much time on change threads.

**On MacOS**

| Total processes/ threads (x)  | Concurrent processes/ threads (y)  | Threads time  | processes time  |
|:----------|:----------|:----------|:----------|
| 100    | 4   | 0.4476s    | 0.3125s    |
| 100    | 8    | 0.4191s    | 0.2738s    |
| 100    | 32    | 0.4565s    | 0.2408s    |
| 1000    | 4    | 0.2544s    | 0.1896s    |
| 1000    | 8    | 0.2236s    | 0.1632s   |
| 1000    | 32    | 0.2261s    | 0.1422s    |
| 10000    | 4    | 0.1512s   | 1.8206s    |
| 10000    | 8    | 0.1308s    | 1.5417s    |
| 10000    | 32    | 0.1331s    | 1.3466s    |



**On Windows**

| Total processes/ threads (x)  | Concurrent processes/ threads (y)  | Threads time  | processes time  |
|:----------|:----------|:----------|:----------|
| 100    | 4   | 0.4493s    | 0.3295s    |
| 100    | 8    | 0.2871s    | 0.2677s    |
| 100    | 32    | 0.3165s    | 0.3002s    |
| 1000    | 4    | 0.2930s    | 0.1993s    |
| 1000    | 8    | 0.2745s    | 0.1852s   |
| 1000    | 32    | 0.2251s    | 0.1501s    |
| 10000    | 4    | 0.1642s   | 1.9517s    |
| 10000    | 8    | 0.1418s    | 1.811s    |
| 10000    | 32    | 0.1355s    | 1.671s    |

**Analysis**
MacOS take a different way to impletement pthread(), which does not use spinlock. So that threads are slower than processes on mac. But when there are many tasks and live processes or threads, mutex lock will cost less that spinlock.

And for windows, it does use createprocess() insteading of fork to create a new process, which leads to slow work.   

## How to clean my code
Please run **"make clean"** like this:

```
sunsiwen@192 A1 % ls
ELFReader.c    makefile   Process.out  Thread.c
ELFReader.out  Process.c  README.md    Thread.out
sunsiwen@192 A1 % make clean
rm -f Process.out Thread.out ELFReader.out
sunsiwen@192 A1 % ls
ELFReader.c  makefile  Process.c  README.md  Thread.c
sunsiwen@192 A1 % 
```

There are only one readme, three .c and a makefile left.

Enjoy them.

Thank you very much.
