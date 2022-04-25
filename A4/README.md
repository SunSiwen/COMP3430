# COMP3430 Assignment4
##Siwen Sun(7898970)

```asm
[suns5@loon ~/my_a4]> tree
.
├── a4image.exfat
├── exfat.c
├── makefile
└── README.md

0 directories, 4 files
[suns5@loon ~/my_a4]>
```
I put a4image.exfat here for testing, but will not submit it

There is a makefile, please use it to compile my code by
```asm
[suns5@loon ~/my_a4]> make
clang -Wall -Wpedantic -Wextra -Werror -o exfat exfat.c -lm
[suns5@loon ~/my_a4]>
```

run my code as the assignment description. For example

INFO
```asm
[suns5@loon ~/my_a4]> ./exfat a4image.exfat info
Volume label is COMP34301S
Volume serial number is 443262697
Free space on the volume is 34344 in KB
The cluster size, 4 sectors and 2048 in bytes

Program completed normally.
[suns5@loon ~/my_a4]>
```
LIST
```asm
[suns5@loon ~/my_a4]> ./exfat a4image.exfat list
Directory: dirs
-Directory: in
--Directory: dirs
---Directory: in
----Directory: dirs
-----File: matryoshka.jpg
-----File: greetings.txt
Directory: ebooks
-File: mary-shelley_frankenstein.epub
-File: h-g-wells_the-time-machine.epub
-File: rudolph-erich-raspe_the-surprising-adventures-of-baron-munchausen.epub
Directory: images
-File: 103052ab.jpg
Directory: music
-File: Burpy-Burp song - Burpin' Ernie.mp3
-File: LICENSE
File: README.md
Directory: text
-Directory: numbers
--File: 291.txt
--File: 295.txt
--File: 299.txt
--File: 253.txt
--File: 254.txt
.
.
.
.
.
.
--File: 599.txt
--File: 350.txt
--File: 351.txt
-File: lorem.txt
File: tree.txt
Directory: videos
-File: Neature Walk - Episode 1-Hm3JodBR-vs.mp4

Program completed normally.
[suns5@loon ~/my_a4]>
```

GET
```asm
[suns5@loon ~/my_a4]> ./exfat a4image.exfat get dirs/in/dirs/in/dirs/greetings.txt

Program completed normally.
[suns5@loon ~/my_a4]> tree
.
├── a4image.exfat
├── exfat
├── exfat.c
├── greetings.txt
├── makefile
└── README.md

0 directories, 6 files
[suns5@loon ~/my_a4]>
```
A file called greetings.txt generates.
```asm
[suns5@loon ~/my_a4]> cat greetings.txt
Hi!

This is a pretty deeply nested file.

Thanks for finding me!
[suns5@loon ~/my_a4]>
```
At the end, you can use make clean to clear codes
```asm
[suns5@loon ~/my_a4]> make clean
rm -f exfat
[suns5@loon ~/my_a4]>
```

Thank you very much.