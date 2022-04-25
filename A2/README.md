# COMP3430 Assignment2
##Siwen Sun(7898970)

```asm

[suns5@grouse ~/my_a2]> tree
.
├── makefile
├── README.md
├── script.sh
└── tmabsh.c

0 directories, 4 files
```
There is a makefile, please use it to compile my code by
```asm
[suns5@grouse ~/my_a2]> make
clang -Wall -Wpedantic -Wextra -Werror -o tmabsh tmabsh.c
[suns5@grouse ~/my_a2]>
```

script.sh is the commands provided in assignment description
ypu can also create your script file
Run the code like this
```asm
[suns5@grouse ~/my_a2]> ./tmabsh script.sh
sunsiwen@shell: touch touched.txt
sunsiwen@shell: ln -sf /usr/share/dict/words
sunsiwen@shell: head -5 words > first5words.txt
sunsiwen@shell: cat /proc/self/status | grep nonvol > nonvoluntaryswitches.txt
sunsiwen@shell: sort -R < words | head -5 > rand5words.txt
sunsiwen@shell: sort -R < words | head -5 | sort -d > randsort5words.txt
sunsiwen@shell: head -5 <(sort -R words) | sort -d > randsort5wordsprocsub.txt
[suns5@grouse ~/my_a2]>
```

At the end, you can use make clean to clear codes
```asm
[suns5@grouse ~/my_a2]> make clean
rm -f tmabsh
[suns5@grouse ~/my_a2]>
```