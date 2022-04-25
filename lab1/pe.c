//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A01
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: lab 1, QUESTION: question 1
//
// REMARKS: What is the purpose of this program?
// Now write a small program that will read the COFF File Header from a PE-formatted binary file
// (an .exe file on Windows). Microsoft has detailed documentation about the PE format.
//-----------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#pragma pack(push)
#pragma pack(1)

//define the structure to record the PE header
typedef struct COFF_HEADER {
    long offset;//4
    uint16_t Machine; // 2
    uint16_t NumberOfSections;  // 2
    time_t TimeDateStamp;  // 4
    uint32_t PointerToSymbolTable;//4
    uint32_t NumberOfSymbols;//4
    uint16_t SizeOfOptionalHeader;//2
    uint16_t Characteristics;//2

} coff_header;

#pragma pack(pop)

//------------------------------------------------------
// read_coff_header()
//
// PURPOSE: read bytes of PE header data
// INPUT PARAMETERS:
//     fd: the file descriptor
//     header: the structure of the PE header
// OUTPUT PARAMETERS:
//     int : 1 success; 2 fail
//------------------------------------------------------
int read_coff_header(int fd, coff_header *header) {
    //assert
    assert(header != NULL);
    assert(fd >= 0);
    //initial
    header->offset = 0;
    header->Machine = 0;
    header->NumberOfSections = 0;
    header->TimeDateStamp = 0;
    header->PointerToSymbolTable = 0;
    header->NumberOfSymbols = 0;
    header->SizeOfOptionalHeader = 0;
    header->Characteristics = 0;

    // skip the next 0x3c bytes
    lseek(fd, 0, SEEK_SET);
    lseek(fd, 0x3c, SEEK_CUR);

    read(fd, &header->offset, 4);
    // skip the next 0x3c bytes
    lseek(fd, 0, SEEK_SET);
    lseek(fd, header->offset, SEEK_CUR);

    //read the name
    char str[4];
    for (int j = 0; j < 4; ++j) {
        read(fd, &str[j], 1);
    }

    if (str[0] == 'P' && str[1] == 'E' && str[2] == '\0' && str[3] == '\0') {
        // 2 bytes for the header->Machine
        read(fd, &header->Machine, 2);
        // 2 bytes for the header->NumberOfSections
        read(fd, &header->NumberOfSections, 2);
        // 4 bytes for the header->TimeDateStamp
        read(fd, &header->TimeDateStamp, 4);
        // 4 bytes for the header->PointerToSymbolTable
        read(fd, &header->PointerToSymbolTable, 4);
        // 4 bytes for the header->NumberOfSymbols
        read(fd, &header->NumberOfSymbols, 4);
        // 2 bytes for the header->SizeOfOptionalHeader
        read(fd, &header->SizeOfOptionalHeader, 2);
        // 2 bytes for the header->Characteristics
        read(fd, &header->Characteristics, 2);
        return 1;
    } else {
        return 0;
    }
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
    //avoid magic number, the input should be 2 parameters
    int parameterNumber = 2;
    if (argc == parameterNumber) {
        //read the file path
        const char *filename = argv[1];
        //create an coffHeader
        coff_header coffHeader;
        //get the fd
        int fd = open(filename, O_RDONLY);
        //fill it
        if (read_coff_header(fd, &coffHeader)) {
            printf("File: %s\n"
                   "\n"
                   "Machine type: 0x%04x\n"
                   "Number of sections: %d\n"
                   "Created: %s"
                   "Symbol table start: 0x0x%08x (should be 0)\n"
                   "Number of symbols: %d (should be 0)\n"
                   "Size of optional header: %d\n"
                   "Characteristics: 0x%04x", filename, coffHeader.Machine, coffHeader.NumberOfSections,
                   asctime(gmtime(&coffHeader.TimeDateStamp)), coffHeader.PointerToSymbolTable,
                   coffHeader.NumberOfSymbols, coffHeader.SizeOfOptionalHeader, coffHeader.Characteristics);
        } else {
            printf("This is not a PE file");
        }
        //close file
        close(fd);
        //exit
        return EXIT_SUCCESS;
    } else {
        //if the input is not correct
        printf("Wrong arguments input");
        return -1;
    }
}

