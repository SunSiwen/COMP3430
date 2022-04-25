//-----------------------------------------
// NAME: Siwen Sun
// STUDENT NUMBER: 7898970
// COURSE: COMP 3430, SECTION: A02
// INSTRUCTOR: Franklin Bristow
// ASSIGNMENT: Assignment 4
//
// REMARKS: What is the purpose of this program?
// The program could read an exFAT file system, and extract files from that file system
//-----------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>


#pragma pack(1)
#pragma pack(push)
typedef struct MAIN_BOOT_SECTOR {
    uint8_t jump_boot[3];
    char fs_name[8];
    uint8_t must_be_zero[53];
    uint64_t partition_offset;
    uint64_t volume_length;
    uint32_t fat_offset;
    uint32_t fat_length;
    uint32_t cluster_heap_offset;
    uint32_t cluster_count;
    uint32_t first_cluster_of_root_directory;
    uint32_t volume_serial_number;
    uint16_t fs_revision;
    uint16_t fs_flags;
    uint8_t bytes_per_sector_shift;
    uint8_t sectors_per_cluster_shift;
    uint8_t number_of_fats;
    uint8_t drive_select;
    uint8_t percent_in_use;
    uint8_t reserved[7];
    uint8_t bootcode[390];
    uint16_t boot_signature;
} main_boot_sector;

typedef struct file_directory_entry {
    uint8_t entry_type;
    uint8_t secondary_count;
    uint16_t set_check_sum;
    uint16_t file_attribute;
    uint16_t reversed1;
    uint32_t create_time;
    uint32_t last_modify_time;
    uint32_t last_access_time;
    uint32_t unused1;
    uint64_t unused2;
} file_directory_entry;

typedef struct stream_extension_directory_entry {
    uint8_t entry_type;
    uint8_t general_secondary_flags;
    uint8_t reversed1;
    uint8_t name_length;
    uint16_t name_hash;
    uint16_t reversed2;
    uint64_t valid_Data_length;
    uint32_t reversed3;
    uint32_t first_cluster;
    uint64_t data_length;
} stream_extension_directory_entry;

typedef struct file_name_directory_entry {
    uint8_t entry_type;
    uint8_t general_secondary_flags;
    uint16_t file_name[15];
} file_name_directory_entry;

typedef struct dictionary_entry {
    uint8_t type;
    uint8_t flag;
    //18 bytes to skip
    uint32_t first_cluster;
    uint64_t data_length;
} entry;
#pragma pack(pop)

/**
 * Convert a Unicode-formatted string containing only ASCII characters
 * into a regular ASCII-formatted string (16 bit chars to 8 bit
 * chars).
 *
 * NOTE: this function does a heap allocation for the string it
 *       returns, caller is responsible for `free`-ing the allocation
 *       when necessary.
 *
 * uint16_t *unicode_string: the Unicode-formatted string to be
 *                           converted.
 * uint8_t   length: the length of the Unicode-formatted string (in
 *                   characters).
 *
 * returns: a heap allocated ASCII-formatted string.
 */
static char *unicode2ascii(uint16_t *unicode_string, uint8_t length) {
    assert(unicode_string != NULL);
    assert(length > 0);

    char *ascii_string = NULL;

    if (unicode_string != NULL && length > 0) {
        // +1 for a NULL terminator
        ascii_string = calloc(sizeof(char), length + 1);

        if (ascii_string) {
            // strip the top 8 bits from every character in the
            // unicode string
            for (uint8_t i = 0; i < length; i++) {
                ascii_string[i] = (char) unicode_string[i];
            }
            // stick a null terminator at the end of the string.
            ascii_string[length] = '\0';
        }
    }

    return ascii_string;
}

/**
 * get the next cluster index of file from fat chain
 * @param header : main boot record
 * @param index : cluster index
 * @return : next index of cluster from fat chain
 */
long getFAT(main_boot_sector *header, int index) {
    return (int) (header->fat_offset) * (1 << header->bytes_per_sector_shift) + index * 4;
}

/**
 * read file directory entry
 * @param handle  : main boot record
 * @param position : offset
 * @param entry : file_directory_entry
 */
void read_file_directory_entry(int handle, int position, file_directory_entry *entry) {
    lseek(handle, 0, SEEK_SET);
    lseek(handle, position, SEEK_CUR);
    read(handle, &entry->entry_type, 1);
    read(handle, &entry->secondary_count, 1);
    read(handle, &entry->set_check_sum, 2);
    read(handle, &entry->file_attribute, 2);
    read(handle, &entry->reversed1, 2);
    read(handle, &entry->create_time, 4);
    read(handle, &entry->last_modify_time, 4);
    read(handle, &entry->last_access_time, 4);
    read(handle, &entry->unused1, 4);
    read(handle, &entry->unused2, 8);
}

/**
 * read_stream_extension_directory_entry
 * @param handle  : main boot record
 * @param position : offset
 * @param entry : stream_extension_directory_entry
 */
void read_stream_extension_directory_entry(int handle, int position, stream_extension_directory_entry *entry) {
    lseek(handle, 0, SEEK_SET);
    lseek(handle, position, SEEK_CUR);
    read(handle, &entry->entry_type, 1);
    read(handle, &entry->general_secondary_flags, 1);
    read(handle, &entry->reversed1, 1);
    read(handle, &entry->name_length, 1);
    read(handle, &entry->name_hash, 2);
    read(handle, &entry->reversed2, 2);
    read(handle, &entry->valid_Data_length, 8);
    read(handle, &entry->reversed3, 4);
    read(handle, &entry->first_cluster, 4);
    read(handle, &entry->data_length, 8);
}

/**
 * read_file_name_directory_entry
 * @param handle  : main boot record
 * @param position : offset
 * @param entry : file_name_directory_entry
 */
void read_file_name_directory_entry(int handle, int position, file_name_directory_entry *entry) {
    lseek(handle, 0, SEEK_SET);
    lseek(handle, position, SEEK_CUR);
    read(handle, &entry->entry_type, 1);
    read(handle, &entry->general_secondary_flags, 1);
    for (int i = 0; i < 15; ++i) {
        read(handle, &entry->file_name[i], 2);
    }
}

/**
 *
 * @param handle : file
 * @param header : mbr
 * @param e : entry
 */
void list(int handle, main_boot_sector *header, int index, int level) {
    // the file has a next cluster according to the fat chain
    int has_next = 1;
    // the position of this cluster
    int position = 0;
    file_directory_entry fde;
    stream_extension_directory_entry sedr;
    file_name_directory_entry fnde;
    long fat_offset = getFAT(header, index);
    lseek(handle, 0, SEEK_SET);
    lseek(handle, fat_offset, SEEK_CUR);
    uint32_t tmp;
    //read from fat chain
    read(handle, &tmp, 4);
    //if tmp == 0xFFFFFFFF which means it is the end of the file
    if (tmp == 0xFFFFFFFF) {
        has_next = 0;
    }

    //read until the end of the file
    do {
        lseek(handle, 0, SEEK_SET);
        lseek(handle, (1 << header->bytes_per_sector_shift) *
                      ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                       header->cluster_heap_offset) + position, SEEK_CUR);
        int char_index = 0;
        char *name;
        uint16_t chars[1024];
        //read_file_directory_entry
        read_file_directory_entry(handle, (1 << header->bytes_per_sector_shift) *
                                          ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                                           header->cluster_heap_offset) + position, &fde);

        //position increases 32
        position += 32;
        //check whether to switch to next cluster
        if (position == (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift)) && has_next) {
            index = tmp;
            fat_offset = getFAT(header, tmp);
            lseek(handle, 0, SEEK_SET);
            lseek(handle, fat_offset, SEEK_CUR);
            read(handle, &tmp, 4);
            if (tmp == 0xFFFFFFFF) {
                has_next = 0;
            }
            position = 0;
            lseek(handle, 0, SEEK_SET);
            lseek(handle, (1 << header->bytes_per_sector_shift) *
                          ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                           header->cluster_heap_offset) + position, SEEK_CUR);
        }
        //skip upper case table and allocation bitmap
        if (fde.entry_type != 0x85) {
            if (fde.entry_type == 0) {
                return;
            }
            continue;
        }
        //read_stream_extension_directory_entry
        read_stream_extension_directory_entry(handle, (1 << header->bytes_per_sector_shift) *
                                                      ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                                                       header->cluster_heap_offset) + position, &sedr);
        // the same with above
        position += 32;
        if (position == (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift)) && has_next) {
            index = tmp;
            fat_offset = getFAT(header, tmp);
            lseek(handle, 0, SEEK_SET);
            lseek(handle, fat_offset, SEEK_CUR);
            read(handle, &tmp, 4);
            if (tmp == 0xFFFFFFFF) {
                has_next = 0;
            }
            position = 0;
            lseek(handle, 0, SEEK_SET);
            lseek(handle, (1 << header->bytes_per_sector_shift) *
                          ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                           header->cluster_heap_offset) + position, SEEK_CUR);
        }
        //for loop to read file name
        for (int i = 0; i < fde.secondary_count - 1; ++i) {
            read_file_name_directory_entry(handle, (1 << header->bytes_per_sector_shift) *
                                                   ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                                                    header->cluster_heap_offset) + position, &fnde);
            for (int j = 0; j < 15; ++j) {
                chars[char_index++] = (fnde.file_name[j]);
            }
            position += 32;
            if (position == (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift)) && has_next) {
                index = tmp;
                fat_offset = getFAT(header, tmp);
                lseek(handle, 0, SEEK_SET);
                lseek(handle, fat_offset, SEEK_CUR);
                read(handle, &tmp, 4);
                if (tmp == 0xFFFFFFFF) {
                    has_next = 0;
                }
                position = 0;
                lseek(handle, 0, SEEK_SET);
                lseek(handle, (1 << header->bytes_per_sector_shift) *
                              ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                               header->cluster_heap_offset) + position, SEEK_CUR);
            }
        }
        //convert
        name = unicode2ascii(chars, char_index);
        if (position == (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift)) && has_next) {
            index = tmp;
            fat_offset = getFAT(header, tmp);
            lseek(handle, 0, SEEK_SET);
            lseek(handle, fat_offset, SEEK_CUR);
            read(handle, &tmp, 4);
            if (tmp == 0xFFFFFFFF) {
                has_next = 0;
            }
            position = 0;
            lseek(handle, 0, SEEK_SET);
            lseek(handle, (1 << header->bytes_per_sector_shift) *
                          ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                           header->cluster_heap_offset) + position, SEEK_CUR);
        }
        //print info
        for (int i = 0; i < level; ++i) {
            printf("-");
        }
        if (fde.file_attribute == 0x10) {
            //recursive
            printf("Directory: %s\n", name);
            list(handle, header, sedr.first_cluster, level + 1);
        } else if (fde.file_attribute == 0x20) {
            printf("File: %s\n", name);
        }

    } while (has_next || position < (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift)));
}

/**
 *
 * @param handle : file
 * @param header : mbr
 * @param e : entry
 */
void info(int handle, main_boot_sector *header, entry *e) {
    if (header->number_of_fats > 0) {
        lseek(handle, 0, SEEK_SET);
        //move to the start of first_cluster_of_root_directory
        lseek(handle, (1 << header->bytes_per_sector_shift) *
                      ((header->first_cluster_of_root_directory - 2) * (1 << (header->sectors_per_cluster_shift)) +
                       header->cluster_heap_offset), SEEK_CUR);

        uint16_t label[11];
        //search for volume entry
        do {
            read(handle, &e->type, 1);
            read(handle, &e->flag, 1);
            for (int i = 0; i < 11; ++i) {
                read(handle, &label[i], 2);
            }
            lseek(handle, 8, SEEK_CUR);
        } while (e->type != 0x83);


        lseek(handle, 0, SEEK_SET);
        //move to the start of first_cluster_of_root_directory
        lseek(handle, (1 << header->bytes_per_sector_shift) *
                      ((header->first_cluster_of_root_directory - 2) * (1 << (header->sectors_per_cluster_shift)) +
                       header->cluster_heap_offset), SEEK_CUR);
        //search for bitmap entry
        do {
            read(handle, &e->type, 1);
            read(handle, &e->flag, 1);
            lseek(handle, 18, SEEK_CUR);
            read(handle, &e->first_cluster, 4);
            read(handle, &e->data_length, 8);
        } while (e->type != 0x81);
        lseek(handle, 0, SEEK_SET);
        //jump to the bitmap
        lseek(handle, (1 << header->bytes_per_sector_shift) *
                      ((e->first_cluster - 2) * (1 << (header->sectors_per_cluster_shift)) +
                       header->cluster_heap_offset), SEEK_CUR);
        unsigned long cnt = 0;
        uint8_t tmp;
        //statistics
        for (int i = 0; i < (int) e->data_length; ++i) {
            read(handle, &tmp, 1);
            cnt += __builtin_popcount(tmp);
        }

        //print info
        printf("Volume label is %s\n", unicode2ascii(label, 11));
        printf("Volume serial number is %d\n", header->volume_serial_number);
        printf("Free space on the volume is %lu in KB\n", (header->cluster_count - cnt) * (1
                << (header->sectors_per_cluster_shift + header->bytes_per_sector_shift)) / 1024);
        printf("The cluster size, %d sectors and %d in bytes\n",
               (1 << header->sectors_per_cluster_shift),
               (1 << (header->sectors_per_cluster_shift + header->bytes_per_sector_shift)));

    }

}

/**
 * read mbr from a img
 * @param handle : file
 * @param header " mbr
 */
void read_MBR(int handle, main_boot_sector *header) {

    // skip the next 4 bytes
    lseek(handle, 0, SEEK_SET);
    // read jump_boot
    for (int i = 0; i < 3; ++i) {
        read(handle, &header->jump_boot[i], 1);
    }

    for (int i = 0; i < 8; ++i) {
        read(handle, &header->fs_name[i], 1);
    }

    for (int i = 0; i < 53; ++i) {
        read(handle, &header->must_be_zero[i], 1);
    }

    read(handle, &header->partition_offset, 8);

    read(handle, &header->volume_length, 8);

    read(handle, &header->fat_offset, 4);
    read(handle, &header->fat_length, 4);
    read(handle, &header->cluster_heap_offset, 4);
    read(handle, &header->cluster_count, 4);
    read(handle, &header->first_cluster_of_root_directory, 4);
    read(handle, &header->volume_serial_number, 4);
    read(handle, &header->fs_revision, 2);
    read(handle, &header->fs_flags, 2);
    read(handle, &header->bytes_per_sector_shift, 1);
    read(handle, &header->sectors_per_cluster_shift, 1);
    read(handle, &header->number_of_fats, 1);
    read(handle, &header->drive_select, 1);
    read(handle, &header->percent_in_use, 1);
    for (int i = 0; i < 7; ++i) {
        read(handle, &header->reserved[i], 1);
    }
    for (int i = 0; i < 390; ++i) {
        read(handle, &header->bootcode[i], 1);
    }
    read(handle, &header->boot_signature, 2);
}

/**
 * extract a file from the EXFAT file system
 * @param handle : file
 * @param header : main boot record
 * @param sedr : stream_extension_directory_entry
 * @param name : file name
 */
void extract(int handle, main_boot_sector *header, stream_extension_directory_entry *sedr,
             char *name) {
    long fat_offset = getFAT(header, sedr->first_cluster);
    int index = sedr->first_cluster;
    lseek(handle, 0, SEEK_SET);
    lseek(handle, fat_offset, SEEK_CUR);
    uint32_t tmp;
    read(handle, &tmp, 4);
    lseek(handle, 0, SEEK_SET);
    lseek(handle, (1 << header->bytes_per_sector_shift) *
                  ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                   header->cluster_heap_offset), SEEK_CUR);
    uint8_t data;
    int size = 0;
    //create another file in current directory
    FILE *fp = NULL;
    if (name != NULL) {
        fp = fopen(name, "w+");
    }

    //read by for loop, the length is data_length
    for (unsigned long i = 0; i < sedr->data_length; ++i) {
        read(handle, &data, 1);
        //write to file
        fwrite(&data, sizeof(data), 1, fp);
        size++;
        //check
        if (size == (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift))) {
            index = tmp;
            fat_offset = getFAT(header, tmp);
            lseek(handle, 0, SEEK_SET);
            lseek(handle, fat_offset, SEEK_CUR);
            read(handle, &tmp, 4);
            size = 0;
            lseek(handle, 0, SEEK_SET);
            lseek(handle, (1 << header->bytes_per_sector_shift) *
                          ((index - 2) * (1 << (header->sectors_per_cluster_shift)) + header->cluster_heap_offset),
                  SEEK_CUR);
        }
    }
    fclose(fp);

}

/**
 * DFS to find the file
 * @param handle : file
 * @param header : main boot record
 * @param index : index of cluster
 * @param paths : name path
 * @param depth : the depth of path
 */
void DFS(int handle, main_boot_sector *header, int index, char **paths, int depth) {
    if (paths[depth] == NULL) {
        return;
    }
    //this part is very similar to list function
    int has_next = 1;
    int position = 0;
    file_directory_entry fde;
    stream_extension_directory_entry sedr;
    file_name_directory_entry fnde;
    long fat_offset = getFAT(header, index);
    lseek(handle, 0, SEEK_SET);
    lseek(handle, fat_offset, SEEK_CUR);
    uint32_t tmp;
    read(handle, &tmp, 4);
    if (tmp == 0xFFFFFFFF) {
        has_next = 0;
    }

    do {
        lseek(handle, 0, SEEK_SET);
        lseek(handle, (1 << header->bytes_per_sector_shift) *
                      ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                       header->cluster_heap_offset) + position, SEEK_CUR);
        int char_index = 0;
        char *name;
        uint16_t chars[1024];
        read_file_directory_entry(handle, (1 << header->bytes_per_sector_shift) *
                                          ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                                           header->cluster_heap_offset) + position, &fde);

        position += 32;
        if (position == (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift)) && has_next) {
            index = tmp;
            fat_offset = getFAT(header, tmp);
            lseek(handle, 0, SEEK_SET);
            lseek(handle, fat_offset, SEEK_CUR);
            read(handle, &tmp, 4);
            if (tmp == 0xFFFFFFFF) {
                has_next = 0;
            }
            position = 0;
        }
        if (fde.entry_type != 0x85) {
            if (fde.entry_type == 0) {
                return;
            }
            continue;
        }
        read_stream_extension_directory_entry(handle, (1 << header->bytes_per_sector_shift) *
                                                      ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                                                       header->cluster_heap_offset) + position, &sedr);
        position += 32;
        if (position == (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift)) && has_next) {
            index = tmp;
            fat_offset = getFAT(header, tmp);
            lseek(handle, 0, SEEK_SET);
            lseek(handle, fat_offset, SEEK_CUR);
            read(handle, &tmp, 4);
            if (tmp == 0xFFFFFFFF) {
                has_next = 0;
            }
            position = 0;
        }
        for (int i = 0; i < fde.secondary_count - 1; ++i) {
            read_file_name_directory_entry(handle, (1 << header->bytes_per_sector_shift) *
                                                   ((index - 2) * (1 << (header->sectors_per_cluster_shift)) +
                                                    header->cluster_heap_offset) + position, &fnde);
            for (int j = 0; j < 15; ++j) {
                chars[char_index++] = (fnde.file_name[j]);
            }
            position += 32;
            if (position == (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift)) && has_next) {
                index = tmp;
                fat_offset = getFAT(header, tmp);
                lseek(handle, 0, SEEK_SET);
                lseek(handle, fat_offset, SEEK_CUR);
                read(handle, &tmp, 4);
                if (tmp == 0xFFFFFFFF) {
                    has_next = 0;
                }
                position = 0;
            }
        }

        name = unicode2ascii(chars, char_index);
        if (position == (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift)) && has_next) {
            index = tmp;
            fat_offset = getFAT(header, tmp);
            lseek(handle, 0, SEEK_SET);
            lseek(handle, fat_offset, SEEK_CUR);
            read(handle, &tmp, 4);
            if (tmp == 0xFFFFFFFF) {
                has_next = 0;
            }
            position = 0;
        }

        if (fde.file_attribute == 0x10) {
            if (!strcmp(name, paths[depth])) {
                DFS(handle, header, sedr.first_cluster, paths, depth + 1);
            }
        } else if (fde.file_attribute == 0x20) {
            if (!strcmp(name, paths[depth]) && paths[depth + 1] == NULL) {
                //find the file and prepare to extract it
                extract(handle, header, &sedr, name);
            }
        }
    } while (has_next || position < (1 << (header->bytes_per_sector_shift + header->sectors_per_cluster_shift)));

}

/**
 * to execute get method
 * @param handle : file
 * @param header : main boot record
 * @param index : index of first root directory
 * @param string : the path of a file
 */
void get(int handle, main_boot_sector *header, int index, char *string) {
    //generate the path array
    char **strings = malloc(128 * sizeof(char *));
    char *temp;
    int cnt = 0;
    temp = strtok(string, "/");
    while (temp != NULL) {
        strings[cnt] = malloc(128 * sizeof(char));
        strcpy(strings[cnt++], temp);
        temp = strtok(NULL, "/");
    }
    strings[cnt] = NULL;
    //use DFS to read
    DFS(handle, header, index, strings, 0);
}

/**
 * Main: the entrance of my program
 * @return
 */
int main(int argc, const char *argv[]) {
    //avoid magic number, the input should be 2 parameters
    int parameterNumber = 3;
    if (argc >= parameterNumber) {
        //read the file path
        const char *filename = argv[1];
        //create an elfHeader
        main_boot_sector mbr;
        entry e;
        //get the fd
        int handle = open(filename, O_RDONLY);
        //fill it
        read_MBR(handle, &mbr);

        //enter different method according to the command
        if (!strcmp("info", argv[2])) {
            info(handle, &mbr, &e);
        } else if (!strcmp("list", argv[2])) {
            list(handle, &mbr, mbr.first_cluster_of_root_directory, 0);
        } else if (!strcmp("get", argv[2])) {
            char path[1024] = "";
            for (int i = 3; i < argc; ++i) {
                strcat(path,argv[i]);
                if(i != argc - 1){
                    strcat(path," ");
                }
            }
//            printf("%s",path);
            get(handle, &mbr, mbr.first_cluster_of_root_directory, path);
        }
    }
    //The output of a program should contain a message to the effect that the program completed its task normally
    printf("\nProgram completed normally.\n");
}
