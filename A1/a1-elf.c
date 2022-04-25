//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A02
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: Assignment 1, QUESTION: Question 1
//
// REMARKS: What is the purpose of this program?
// The program could read an ELF-formatted file and write out its program information. Work for both 32-bit and 64-bit
//-----------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#pragma pack(push)
#pragma pack(1)
#define PARAMETER_NUMBER 2
#define ROW 2
#define COL 16
#define SIZE32 4
#define SIZE64 8
#define FIRST32BYTE 32

#pragma pack(pop)


//------------------------------------------------------
// min()
//
// PURPOSE: get the smaller one between this 2 number
// INPUT PARAMETERS:
//     get 2 integer values
// OUTPUT PARAMETERS:
//      the smaller one between this 2 number
//------------------------------------------------------
int min(int num1, int num2) {
    if (num1 < num2) {
        return num1;
    } else {
        return num2;
    }
}

//------------------------------------------------------
// printFirst32Byte()
//
// PURPOSE: print out the first 32 byte of data
// INPUT PARAMETERS:
//     fd: the file description
//     offset: the position start to read
//     size: the size of data to prevent from reading other data
// OUTPUT PARAMETERS:
//      no return value but print out the first 32 byte of data
//------------------------------------------------------
void printFirst32Byte(int fd, const uint64_t offset, const uint64_t size) {

    //malloc 32 uint8_t* array from memory and initial
    uint8_t *first32Byte = (uint8_t *) malloc(FIRST32BYTE * sizeof(uint8_t * ));
    for (int i = 0; i < FIRST32BYTE; ++i) {
        first32Byte[i] = 0;
    }

    //set the pointer to the head and move to the offset
    lseek(fd, 0, SEEK_SET);
    lseek(fd, offset, SEEK_CUR);

    //read 32 bytes if the size over 32, otherwise stop
    for (int i = 0; i < min(FIRST32BYTE, size); i++) {
        read(fd, &first32Byte[i], 1);
    }

    // As the A1 description says: 16 paris number * 2 lines
    for (int i = 0; i < ROW; i++) {
        printf("\t");
        for (int j = 0; j < COL && i * COL + j < (int) size; ++j) {
            printf("%02x ", first32Byte[i * COL + j]);
        }
        printf("\n");
    }
    printf("\n");
}

//------------------------------------------------------
// main()
//
// PURPOSE: the entrance of my code
// INPUT PARAMETERS:
//     argc: the numbers of argv
//     argv: the input
// OUTPUT PARAMETERS:
//      status code
//------------------------------------------------------
int main(int argc, const char *argv[]) {
    //check the input
    if (argc != PARAMETER_NUMBER) {
        //if the input number is not correct, exit
        printf("Please double check your input arguments number");
        return -1;
    }

    //get the file's name
    const char *filename = argv[1];
    //get the fd by open, the model is read-only
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("\nFail to open your file\n");
        return -1;
    }
    //create elfHeader parameters
    uint32_t EI_MAG = 0; // 4
    uint8_t EI_CLASS = 0; // 1
    uint8_t EI_DATA = 0;  // 1
    uint8_t EI_VERSION = 0;  // 1
    uint8_t EI_OSABI = 0;  // 1
    uint8_t EI_ABIVERSION = 0;  // 1
    uint64_t EI_PAD = 0;  // 7
    uint16_t e_type = 0;//2
    uint16_t e_machine = 0;//2
    uint32_t e_version = 0;//4
    uint64_t e_entry = 0; //32-4  64-8
    uint64_t e_phoff = 0; //32-4 64-8
    uint64_t e_shoff = 0; //32-4 64-8
    uint32_t e_flags = 0;//4
    uint16_t e_ehsize = 0;//2
    uint16_t e_phentsize = 0;//2
    uint16_t e_phnum = 0;//2
    uint16_t e_shentsize = 0;//2
    uint16_t e_shnum = 0;//2
    uint16_t e_shstrndx = 0;//2

    //assert before reading
    assert(fd >= 0);
    printf("ELF elfHeader:\n");
    //set the pointer to the start
    lseek(fd, 0, SEEK_SET);
    // 4 bytes for the EI_MAG
    read(fd, &EI_MAG, 4);
    // 1 bytes for the EI_CLASS
    read(fd, &EI_CLASS, 1);
    printf("  * %s-bit\n", EI_CLASS == 1 ? "32" : "64");
    // 1 bytes for the EI_DATA
    read(fd, &EI_DATA, 1);
    printf("  * %s\n", EI_DATA == 1 ? "little endianness" : "big endianness");
    // 1 bytes for the EI_VERSION
    read(fd, &EI_VERSION, 1);
    // 1 bytes for the EI_OSABI
    read(fd, &EI_OSABI, 1);
    printf("  * compiled for 0x%02x (operating system)\n", EI_OSABI);
    // 1 bytes for the EI_ABIVERSION
    read(fd, &EI_ABIVERSION, 1);
    // 7 bytes for the EI_PAD
    read(fd, &EI_PAD, 7);
    // 2 bytes for the e_type
    read(fd, &e_type, 2);
    // 2 bytes for the e_machine
    read(fd, &e_machine, 2);
    // 2 bytes for the e_version
    read(fd, &e_version, 4);
    // 4 or 8 bytes for the e_entry depends on EI_CLASS
    read(fd, &e_entry, EI_CLASS == 1 ? SIZE32 : SIZE64);
    // 4 or 8 bytes for the e_phoff depends on EI_CLASS
    read(fd, &e_phoff, EI_CLASS == 1 ? SIZE32 : SIZE64);
    // 4 or 8 bytes for the e_shoff depends on EI_CLASS
    read(fd, &e_shoff, EI_CLASS == 1 ? SIZE32 : SIZE64);

    printf("  * entry point address 0x%016lx\n"
           "  * program elfHeader table starts at 0x%016lx\n", e_entry,
           e_phoff);
    // 2 bytes for the e_flags
    read(fd, &e_flags, 4);
    // 2 bytes for the e_ehsize
    read(fd, &e_ehsize, 2);
    // 2 bytes for the header e_phentsize
    read(fd, &e_phentsize, 2);
    // 2 bytes for the header e_phnum
    read(fd, &e_phnum, 2);
    // 2 bytes for the header e_shentsize
    read(fd, &e_shentsize, 2);
    // 2 bytes for the header e_shnum
    read(fd, &e_shnum, 2);
    // 2 bytes for the header e_shstrndx
    read(fd, &e_shstrndx, 2);
    //print the rest parameter
    printf("  * there are %d program headers, each is %d bytes\n"
           "  * there are %d section headers, each is %d bytes\n"
           "  * the section elfHeader string table is %d",
           e_phnum, e_phentsize, e_shnum, e_shentsize, e_shstrndx);

    //read program header tables according to e_phoff and e_phentsize gotten above
    for (int k = 0; k < e_phnum; k++) {
        //create program headers' parameters
        uint32_t p_type = 0;//4
        uint32_t p_flags = 0;//4
        uint64_t p_offset = 0;//32-4 64-8
        uint64_t p_vaddr = 0;//32-4 64-8
        uint64_t p_paddr = 0;//32-4 64-8
        uint64_t p_filesz = 0; //32-4  64-8
        //set the pointer to the start
        lseek(fd, 0, SEEK_SET);
        // skip to a certain table
        lseek(fd, e_phoff + (e_phentsize * k), SEEK_CUR);

        printf("\n\n\nProgram header #%d:\n", k);
        // 4 bytes for the p_type
        read(fd, &p_type, 4);
        printf("  * segment type 0x%08x\n", p_type);
        //there is a p_flags for 64_bits system file
        EI_CLASS == 1 ? printf("") : read(fd, &p_flags, SIZE32);
        // 4 or 8  bytes for the p_offset depends on EI_CLASS
        read(fd, &p_offset, EI_CLASS == 1 ? SIZE32 : SIZE64);
        // 4 or 8  bytes for the p_vaddr depends on EI_CLASS
        read(fd, &p_vaddr, EI_CLASS == 1 ? SIZE32 : SIZE64);
        // 4 or 8  bytes for the p_paddr depends on EI_CLASS
        read(fd, &p_paddr, EI_CLASS == 1 ? SIZE32 : SIZE64);
        printf("  * virtual address of segment 0x%016lx\n", p_vaddr);
        // 4 or 8  bytes for the p_filesz depends on EI_CLASS
        read(fd, &p_filesz, EI_CLASS == 1 ? SIZE32 : SIZE64);
        printf("  * size in file %ld bytes\n"
               "  * first up to 32 bytes starting at 0x%016lx:\n", p_filesz, p_offset);
        printFirst32Byte(fd, p_offset, p_filesz);
    }


    //to calculate the section header table name's table
    uint64_t baseOff = 0; //the start of .shstrtab
    lseek(fd, 0, SEEK_SET);
    lseek(fd, e_shentsize * e_shstrndx + e_shoff, SEEK_CUR);
    lseek(fd, 4, SEEK_CUR);
    lseek(fd, 4, SEEK_CUR);
    lseek(fd, EI_CLASS == 1 ? SIZE32 : SIZE64, SEEK_CUR);
    lseek(fd, EI_CLASS == 1 ? SIZE32 : SIZE64, SEEK_CUR);
    // 4 or 8 bytes for the baseOff depends on EI_CLASS
    read(fd, &baseOff, EI_CLASS == 1 ? SIZE32 : SIZE64);

    //read every section header
    for (int k = 0; k < e_shnum; ++k) {
        //create section headers' parameters
        uint32_t sh_name = 0;//4
        uint32_t sh_type = 0;//4
        uint64_t sh_flags = 0; //32-4  64-8
        uint64_t sh_addr = 0; //32-4  64-8
        uint64_t sh_offset = 0;//32-4 64-8
        uint64_t sh_size = 0; //32-4  64-8
        //set the pointer to the start
        lseek(fd, 0, SEEK_SET);
        //skip to a certain table
        lseek(fd, k * e_shentsize + e_shoff, SEEK_CUR);
        // 4 bytes for the sh_name
        read(fd, &sh_name, 4);
        // 4 bytes for the sh_type
        read(fd, &sh_type, 4);
        // 4 or 8 bytes for the sh_flags depends on EI_CLASS
        read(fd, &sh_flags, EI_CLASS == 1 ? SIZE32 : SIZE64);
        // 4 or 8 bytes for the sh_addr depends on EI_CLASS
        read(fd, &sh_addr, EI_CLASS == 1 ? SIZE32 : SIZE64);
        // 4 or 8 bytes for the sh_offset depends on EI_CLASS
        read(fd, &sh_offset, EI_CLASS == 1 ? SIZE32 : SIZE64);
        // 4 or 8 bytes for the sh_size depends on EI_CLASS
        read(fd, &sh_size, EI_CLASS == 1 ? SIZE32 : SIZE64);

        //calculate the start of this section table
        uint64_t off = baseOff + sh_name;
        //move pointer to the start of this section table
        lseek(fd, 0, SEEK_SET);
        lseek(fd, off, SEEK_CUR);
        //read the section name
        char name[128];
        int i = 0;
        do {
            read(fd, &name[i], 1);
        } while (name[i++] != '\0');
        //print out all info together
        printf("Section header #%d\n"
               "  * section name %s\n"
               "  * type 0x%02x\n"
               "  * virtual address of section 0x%016lx\n"
               "  * size in file %ld bytes\n"
               "  * first up to 32 bytes starting at 0x%016lx:\n",
               k, name, sh_type, sh_addr, sh_size, sh_offset);
        printFirst32Byte(fd, sh_offset, sh_size);
        printf("\n");
    }
    //close file
    close(fd);
    //The output of a program should contain a message to the effect that the program completed its task normally
    printf("\nProgram completed normally.\n");
    //exit_success
    return 0;
}
