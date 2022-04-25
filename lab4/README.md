# COMP3430 Lab4
##Siwen Sun(7898970)

```asm

[suns5@loon ~/my_lab4]> tree
.
├── fsck-exfat.c
├── makefile
└── README.md

0 directories, 3 files
[suns5@loon ~/my_lab4]>

```
There is a makefile, please use it to compile my code by
```asm
[suns5@loon ~/my_lab4]> make
clang -Wall -Wpedantic -Wextra -Werror -o fsck-exfat fsck-exfat.c -lm
[suns5@loon ~/my_lab4]>
```

run my code by "./fsck-exfat filePath". For example
```asm
[suns5@loon ~/my_lab4]> ./fsck-exfat exfat-with-files-inconsistent-bitmap.img
MBR appears to be consistent.
Inconsistent file system: PercentInUse is 7%, allocation bitmap is 730/2422 bits => 30%.
[suns5@loon ~/my_lab4]>
```

At the end, you can use make clean to clear codes
```asm
[suns5@loon ~/my_lab4]> make clean
rm -f fsck-exfat
[suns5@loon ~/my_lab4]>
```

Thank you very much.