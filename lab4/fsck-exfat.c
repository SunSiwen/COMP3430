#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>


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

typedef struct dictionary_entry {
    uint8_t type;
    uint8_t flag;
    //18 bytes to skip
    uint32_t first_cluster;
    uint64_t data_length;
} entry;
#pragma pack(pop)


/**
 *
 * @param handle : file
 * @param header : mbr
 * @param e : entry
 */
void check_bitmap(int handle, main_boot_sector *header, entry *e) {
    if (header->number_of_fats > 0) {
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
        //compare and output
        if (cnt * 100 / header->cluster_count != header->percent_in_use) {
            printf("Inconsistent file system: PercentInUse is %d%%, allocation bitmap is %lu/%d bits => %lu%%.\n",
                   header->percent_in_use, cnt, header->cluster_count,
                   cnt * 100 / header->cluster_count);
        } else {
            printf("File system appears to be consistent.\n");
        }
    }

}

/**
 * check the mbr
 * @param header : a mbr
 */
void check_MBR(main_boot_sector *header) {
    //check JumpBoot
    if (header->jump_boot[0] != 235 || header->jump_boot[1] != 118 || header->jump_boot[2] != 144) {
        printf("Inconsistent file system: JumpBoot must be [0xEB, 0x76, 0x90], value is [0x%X, 0x%X, 0x%X].\n",
               header->jump_boot[0], header->jump_boot[1], header->jump_boot[2]);
        exit(0);
    }

    //FileSystemName
    if (strcmp("EXFAT   ", header->fs_name) != 0) {
        printf("Inconsistent file system: FileSystemName must be 'EXFAT   ', value is \'%s\'.\n", header->fs_name);
        exit(0);
    }

    //MustBeZero
    for (int i = 0; i < 53; ++i) {
        if (header->must_be_zero[i] != 0) {
            printf("Inconsistent file system: MustBeZero must be all 0s, value is ");
            for (int j = 0; j < 53; ++j) {
                printf("%d", header->must_be_zero[j]);
            }
            printf("\n");
            exit(0);
        }
    }

    //VolumeLength llu
    if (header->volume_length < (1 << (20 - header->bytes_per_sector_shift))) {
        printf("Inconsistent file system: VolumeLength should be >= %d, value is %llu.\n",
               (1 << (20 - header->bytes_per_sector_shift)), (unsigned long long) header->volume_length);
        printf("bytes_per_sector_shift is %x\n", header->bytes_per_sector_shift);
        exit(0);
    }
    if (header->volume_length > ((1UL << 63) - 1 + (1UL << 63))) {
        printf("Inconsistent file system: VolumeLength should be <= %llu, value is %llu.\n",
               (unsigned long long) ((1UL << 63) - 1 + (1UL << 63)), (unsigned long long) header->volume_length);
        exit(0);
    }

    //FatOffset
    if (header->fat_offset < 24) {
        printf("Inconsistent file system: FatOffset should be >= 24, value is %d.\n", header->fat_offset);
        exit(0);
    }
    if (header->fat_offset > (header->cluster_heap_offset - (header->fat_length * header->number_of_fats))) {
        printf("Inconsistent file system: FatOffset should be <= %llu, value is %llu.\n",
               (unsigned long long) (header->cluster_heap_offset - (header->fat_length * header->number_of_fats)),
               (unsigned long long) header->fat_offset);
        exit(0);
    }

    //FatLength
    if (header->fat_length < ceil((header->cluster_count + 2)) * 4 / (1 << header->bytes_per_sector_shift)) {
        printf("Inconsistent file system: FatLength should be >= %llu, value is %llu.\n",
               (unsigned long long) ceil((header->cluster_count + 2)) * 4 / (1 << header->bytes_per_sector_shift),
               (unsigned long long) header->fat_length);
        exit(0);
    }
    if (header->fat_length > floor((header->cluster_heap_offset - header->fat_length) / header->number_of_fats)) {
        printf("Inconsistent file system: FatLength should be <= %llu, value is %llu.\n",
               (unsigned long long) ceil((header->cluster_count + 2)) * 4 / (1 << header->bytes_per_sector_shift),
               (unsigned long long) header->fat_length);
        exit(0);
    }

    //FirstClusterOfRootDirectory
    if (header->first_cluster_of_root_directory < 2) {
        printf("Inconsistent file system: FirstClusterOfRootDirectory should be >= 2, value is %llu.\n",
               (unsigned long long) header->first_cluster_of_root_directory);
        exit(0);
    }
    if (header->first_cluster_of_root_directory > header->cluster_count + 1) {
        printf("Inconsistent file system: FirstClusterOfRootDirectory should be <= %llu, value is %llu.\n",
               (unsigned long long) header->cluster_count + 1,
               (unsigned long long) header->first_cluster_of_root_directory);
        exit(0);
    }

    //BootSignature
    if (43605 != header->boot_signature) {
        printf("Inconsistent file system: BootSignature must be 0xAA55, value is 0x%X.\n",
               (int) header->boot_signature);
        exit(0);
    }
    printf("MBR appears to be consistent.\n");
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
 * Main: the entrance of my program
 * @return
 */
int main(int argc, const char *argv[]) {

    //avoid magic number, the input should be 2 parameters
    int parameterNumber = 2;
    if (argc == parameterNumber) {
        //read the file path
        const char *filename = argv[1];
        //create an elfHeader
        main_boot_sector mbr;
        entry e;
        //get the fd
        int handle = open(filename, O_RDONLY);
        //fill it
        read_MBR(handle, &mbr);
        //check the boot sector
        check_MBR(&mbr);
        check_bitmap(handle, &mbr, &e);
    }
}
