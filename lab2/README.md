# COMP3430 lab2
##Siwen Sun(7898970)

```asm
[suns5@goose ~/my_lab2]> tree
.
├── makefile
├── octopipe.c
└── README.md

0 directories, 3 files

```
There is a makefile, please use it to compile my code by
```asm
[suns5@goose ~/my_lab2]> make
clang -Wall -Wpedantic -Wextra -Werror -o octopipe octopipe.c
```

###Question1

####part1 process
Run the code like this and exit by ctrl+d
```asm
[suns5@goose ~/my_lab2]> ./octopipe "tr a-z A-Z" "wc -c"
14554 is about to start [tr a-z A-Z]
14555 is about to start [wc -c]
hello world
HELLO WORLD
HELLO WORLD
HELLO WORLD
24

Parent process completed normally.

```


At the end, you can use make clean to clear codes
```asm
[suns5@goose ~/my_lab2]> make clean
rm -f octopipe
[suns5@goose ~/my_lab2]>
```