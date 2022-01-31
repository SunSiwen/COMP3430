//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A01
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: assignment 1, QUESTION: question 1
//
// REMARKS: What is the purpose of this program?
// The program read a file and analysis its headers.
//-----------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#pragma pack(push)
#pragma pack(1)
typedef struct ELF_HEADER {
    //lseek 4 bytes
    uint8_t file_class; // 1
    uint8_t file_data;  // 1
    //lseek 1 byte
    uint8_t file_ABI;  // 1
    //lseek 8 byte
    uint16_t file_type;//2
    uint16_t file_ISA;//2
    //lseek 4 bytes
    uint64_t file_entry; //32-4  64-8
    uint64_t file_phoff; //32-4 64-8
    uint64_t file_shoff; //32-4 64-8
    // lseek 32 -6  64 - 10
    uint16_t file_phentsize;//2
    uint16_t file_phnum;//2
    uint16_t file_shentsize;//2
    uint16_t file_shnum;//2
    uint16_t file_shstrndx;//2
} elf_header;

typedef struct PROGRAM_HEADER {
    uint32_t file_type;//4
    //lseek 32-4 64-12
    uint64_t file_offset;//32-4 64-8
    uint64_t file_vaddr;//32-4 64-8
    //lseek 32-4 64-8
    uint64_t file_filesz; //32-4  64-8
    uint8_t file_data[32]; //32
} program_header;

typedef struct SECTION_HEADER {
    uint32_t file_name;//4
    uint32_t file_type;//4
    //lseek 32-4 64-8
    uint64_t file_addr; //32-4  64-8
    uint64_t file_offset;//32-4 64-8
    uint64_t file_size; //32-4  64-8
    uint8_t file_data[32];
} section_header;
#pragma pack(pop)

/**
 * to compare two numbers, and return the min one
 * @param num1
 * @param num2
 * @return : the min number
 */
int min(int num1, int num2) {
    if (num1 < num2) {
        return num1;
    } else {
        return num2;
    }
}

/**
 *
 * @param handle : fd
 * @param header : elf_header to store data
 */
void read_elf_header(int handle, elf_header *header);

/**
 *
 * @param handle : fd
 * @param header : a program header to store data
 * @param offset : an offset to locate the start of program header table
 * @param class : 32bit(1) or 64bit(2)
 */
void read_program_header(int handle, program_header *header, int offset, int class);

/**
 *
 * @param handle : fd
 * @param header : a section header to store data
 * @param offset : an offset to locate the start of section header table
 * @param class : 32bit(1) or 64bit(2)
 */
void read_section_header(int handle, section_header *header, int offset, int class);

/**
 *
 * @param handle : fd
 * @param offset : an offset to the file start, so that we could find the start of this section
 * @param file_data : an array to store data
 * @param file_size : the size of this section
 */
void readFirst32Byte(int handle, int offset, uint8_t *file_data, int file_size);

int main(int argc, const char *argv[]) {
    //avoid magic number, the input should be 2 parameters
    int parameterNumber = 2;
    if (argc == parameterNumber) {
        //read the file path
        const char *filename = argv[1];
        //create an elfHeader
        elf_header elfHeader;
        //get the fd
        int handle = open(filename, O_RDONLY);
        //fill it
        read_elf_header(handle, &elfHeader);
        //print the elfheader
        printf("ELF elfHeader:\n"
               "  * %s-bit\n"
               "  * %s\n"
               "  * compiled for 0x%02x (operating system)\n"
               "  * has type 0x%02x\n"
               "  * compiled for 0x%02x (isa)\n"
               "  * entry point address 0x%016llx\n"
               "  * program elfHeader table starts at 0x%016llx\n"
               "  * there are %d program headers, each is %d bytes\n"
               "  * there are %d section headers, each is %d bytes\n"
               "  * the section elfHeader string table is %d", elfHeader.file_class == 1 ? "32" : "64",
               elfHeader.file_data == 1 ? "little endianness" : "big endianness", elfHeader.file_ABI,
               elfHeader.file_type,
               elfHeader.file_ISA, (unsigned long long) elfHeader.file_entry, (unsigned long long) elfHeader.file_phoff,
               elfHeader.file_phnum,
               elfHeader.file_phentsize, elfHeader.file_shnum, elfHeader.file_shentsize, elfHeader.file_shstrndx);
        //record some import value
        int pnum = elfHeader.file_phnum; //how many program headers
        int psize = elfHeader.file_phentsize; // how large a program header is
        int snum = elfHeader.file_shnum; //how many section headers
        int ssize = elfHeader.file_shentsize; // how large a section header is
        int poff = elfHeader.file_phoff; //the offset of program segment
        int soff = elfHeader.file_shoff; //the offset of section data
        int row = 2; // to print data -- 2 rows
        int col = 16; // // to print data -- 16 columns per row
        //read every program header table
        for (int k = 0; k < pnum; k++) {
            program_header programHeader;
            // the offset = (poff + psize * k)
            read_program_header(handle, &programHeader, (poff + psize * k), elfHeader.file_class);
            //print info
            printf("\n\n\nProgram header #%d:\n"
                   "  * segment type 0x%08x\n"
                   "  * virtual address of segment 0x%016llx\n"
                   "  * size in file %llu bytes\n"
                   "  * first up to 32 bytes starting at 0x%016llx:\n", k, programHeader.file_type,
                   (unsigned long long) programHeader.file_vaddr,
                   (unsigned long long) programHeader.file_filesz, (unsigned long long) programHeader.file_offset);
            // 2 lines and 16 paris number every line
            for (int i = 0; i < row; i++) {
                printf("\t");
                for (int j = 0; j < col && i * col + j < (int) programHeader.file_filesz; ++j) {
                    printf("%02x ", programHeader.file_data[i * col + j]);
                }
                printf("\n");
            }
            printf("\n");
        }
        //find the section header table name's table
        section_header sectionHeaderTable;
        read_section_header(handle, &sectionHeaderTable, (ssize * elfHeader.file_shstrndx + soff),
                            elfHeader.file_class);
        uint64_t baseOff = sectionHeaderTable.file_offset; //the start of .shstrtab

        //read section header
        for (int k = 0; k < snum; ++k) {
            section_header sectionHeader;
            read_section_header(handle, &sectionHeader, k * ssize + soff, elfHeader.file_class);
            uint64_t off = baseOff + sectionHeader.file_name;
            //move pointer
            lseek(handle, 0, SEEK_SET);
            lseek(handle, off, SEEK_CUR);
            //read the name, I like this number 1024
            char str[1024];
            int i = 0;
            do {
                read(handle, &str[i], 1);
            } while (str[i++] != '\0');
            //print the info
            printf("Section header #%d\n"
                   "  * section name %s\n"
                   "  * type 0x%02x\n"
                   "  * virtual address of section 0x%016llx\n"
                   "  * size in file %llu bytes\n"
                   "  * first up to 32 bytes starting at 0x%016llx:\n",
                   k,
                   str,
                   sectionHeader.file_type,
                   (unsigned long long) sectionHeader.file_addr,
                   (unsigned long long) sectionHeader.file_size,
                   (unsigned long long) sectionHeader.file_offset);
            // 2 lines and 16 pairs number every line
            for (int h = 0; h < row; h++) {
                printf("\t");
                for (int j = 0; j < col && h * col + j < (int) sectionHeader.file_size; ++j) {
                    printf("%02x ", sectionHeader.file_data[h * col + j]);
                }
                printf("\n");
            }
            printf("\n");
        }
        //close file
        close(handle);
        //exit
        return EXIT_SUCCESS;
    } else {
        //if the input is not correct
        printf("Wrong arguments input");
        return -1;
    }
}

void read_elf_header(int handle, elf_header *header) {
    //assert
    assert(header != NULL);
    assert(handle >= 0);
    //initial
    header->file_class = 0;
    header->file_data = 0;
    header->file_ABI = 0;
    header->file_type = 0;
    header->file_ISA = 0;
    header->file_entry = 0;
    header->file_phoff = 0;
    header->file_shoff = 0;
    header->file_phentsize = 0;
    header->file_phnum = 0;
    header->file_shentsize = 0;
    header->file_shnum = 0;
    header->file_shstrndx = 0;

    // skip the next 4 bytes
    lseek(handle, 0, SEEK_SET);
    lseek(handle, 4, SEEK_CUR);
    // 1 bytes for the header class 32 or 64 bits
    read(handle, &header->file_class, 1);
    // 1 bytes for the header endianness
    read(handle, &header->file_data, 1);
    //lseek 1 byte
    lseek(handle, 1, SEEK_CUR);
    // 1 bytes for the header ABI
    read(handle, &header->file_ABI, 1);
    // skip the next 8 bytes
    lseek(handle, 8, SEEK_CUR);
    // 2 bytes for the header type
    read(handle, &header->file_type, 2);
    // 2 bytes for the header ISA
    read(handle, &header->file_ISA, 2);
    // skip the next 4 bytes
    lseek(handle, 4, SEEK_CUR);
    if (header->file_class == 1) {
        // 4 bytes for the header entry
        read(handle, &header->file_entry, 4);
        // 4 bytes for the header phoff
        read(handle, &header->file_phoff, 4);
        // 4 bytes for the header shoff
        read(handle, &header->file_shoff, 4);
    } else {
        // 8 bytes for the header entry
        read(handle, &header->file_entry, 8);
        // 8 bytes for the header phoff
        read(handle, &header->file_phoff, 8);
        // 8 bytes for the header shoff
        read(handle, &header->file_shoff, 8);
    }
    // skip the next 6 bytes
    lseek(handle, 6, SEEK_CUR);
    // 2 bytes for the header phentsize
    read(handle, &header->file_phentsize, 2);
    // 2 bytes for the header phnum
    read(handle, &header->file_phnum, 2);
    // 2 bytes for the header shentsize
    read(handle, &header->file_shentsize, 2);
    // 2 bytes for the header shnum
    read(handle, &header->file_shnum, 2);
    // 2 bytes for the header shstrndx
    read(handle, &header->file_shstrndx, 2);
}


void read_program_header(int handle, program_header *header, int offset, int class) {
    //assert
    assert(header != NULL);
    assert(handle >= 0);
    //initial
    header->file_type = 0;
    header->file_offset = 0;
    header->file_vaddr = 0;
    header->file_filesz = 0;
    // skip
    lseek(handle, 0, SEEK_SET);
    lseek(handle, offset, SEEK_CUR);
    // 4 bytes for the header type
    read(handle, &header->file_type, 4);
    if (class == 1) {
        read(handle, &header->file_offset, 4);
        // 4 bytes for the header address
        read(handle, &header->file_vaddr, 4);
        lseek(handle, 4, SEEK_CUR);
        // 4 bytes for the header file size
        read(handle, &header->file_filesz, 4);
    } else {
        lseek(handle, 4, SEEK_CUR);
        read(handle, &header->file_offset, 8);
        // 8 bytes for the header address
        read(handle, &header->file_vaddr, 8);
        lseek(handle, 8, SEEK_CUR);
        // 8 bytes for the header file size
        read(handle, &header->file_filesz, 8);
    }
    readFirst32Byte(handle, header->file_offset, header->file_data, header->file_filesz);
}

void read_section_header(int handle, section_header *header, int offset, int class) {
    //assert
    assert(header != NULL);
    assert(handle >= 0);
    //initial
    header->file_name = 0;
    header->file_type = 0;
    header->file_addr = 0;
    header->file_offset = 0;
    header->file_size = 0;
    // skip and move pointer to the start of section header table
    lseek(handle, 0, SEEK_SET);
    lseek(handle, offset, SEEK_CUR);
    // 4 bytes for the header name
    read(handle, &header->file_name, 4);
    // 4 bytes for the header type
    read(handle, &header->file_type, 4);
    if (class == 1) {
        lseek(handle, 4, SEEK_CUR);
        // 4 bytes for the header address
        read(handle, &header->file_addr, 4);
        // 4 bytes for the header offset
        read(handle, &header->file_offset, 4);
        // 4 bytes for the header file size
        read(handle, &header->file_size, 4);
    } else {
        lseek(handle, 8, SEEK_CUR);
        // 8 bytes for the header address
        read(handle, &header->file_addr, 8);
        // 8 bytes for the header offset
        read(handle, &header->file_offset, 8);
        // 8 bytes for the header file size
        read(handle, &header->file_size, 8);
    }
    readFirst32Byte(handle, header->file_offset, header->file_data, header->file_size);
//    //move the pointer
//    lseek(handle, 0, SEEK_SET);
//    lseek(handle, header->file_offset, SEEK_CUR);
//    int first32Bytes = 32;
//    //read data, the size depends on min(32, file_sizw)
//    for (int i = 0; i < min(first32Bytes, header->file_size); i++) {
//        read(handle, &header->file_data[i], 1);
//    }
}

/**
 *
 * @param handle : fd
 * @param offset : an offset to the file start, so that we could find the start of this section
 * @param file_data : an array to store data
 * @param file_size : the size of this section
 */
void readFirst32Byte(int handle, int offset, uint8_t *file_data, int file_size) {
    //move the pointer
    lseek(handle, 0, SEEK_SET);
    lseek(handle, offset, SEEK_CUR);
    // avoid magic number, it is used to get the first 32 byte of a section
    int first32Bytes = 32;
    //read data, the size depends on min(32, file_size)
    for (int i = 0; i < min(first32Bytes, file_size); i++) {
        read(handle, &file_data[i], 1);
    }
}
