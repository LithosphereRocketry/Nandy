#include <stdio.h>
#include <stdint.h>

#define SECTOR_SZ 512

#define MBR_PARTITIONS 0x01BE
#define MBR_SIGADDR 0x01FE
#define MBR_SIGNATURE 0xAA55

FILE* disk;

// Driver functions
struct __attribute__((packed)) {
    uint8_t mbractive : 1;
    uint8_t dirty : 1;
    uint8_t partsactive : 4;
    uint8_t part : 2;
} diskStatus;

struct {
    uint16_t start;
    uint16_t length;
} partitions[4];

uint16_t currentSector;
uint8_t sectorBuf[SECTOR_SZ];

void fetch(uint16_t sector) {
    fseek(disk, ((long) sector) * SECTOR_SZ, SEEK_SET);
    fread(sectorBuf, 1, SECTOR_SZ, disk);
    currentSector = sector;
    diskStatus.dirty = 0;
}
void flush() {
    fseek(disk, ((long) currentSector) * SECTOR_SZ, SEEK_SET);
    fwrite(sectorBuf, 1, SECTOR_SZ, disk);
    diskStatus.dirty = 0;
}

// in asm, will return diskStatus
void diskInit() {
    disk = fopen("exampledisk.img", "rb+");
    fetch(0);
    diskStatus.partsactive = 0;
    diskStatus.part = 0;

    uint16_t sig = *((uint16_t*) &sectorBuf[MBR_SIGADDR]);
    if(sig == MBR_SIGNATURE) {
        diskStatus.mbractive = 1;
        for(int i = 0; i < 4; i++) {
            uint8_t* partfield = sectorBuf + MBR_PARTITIONS + i*16;
            if((partfield[0] == 0x00 || partfield[0] == 0x80) && partfield[4] == 0x04) {
                partitions[i].start = (((uint16_t) partfield[9]) << 8) | partfield[8];
                partitions[i].length = (((uint16_t) partfield[0xD]) << 8) | partfield[0xC];
                diskStatus.partsactive |= 1 << i;
            }
        }
    } else {
        diskStatus.mbractive = 0;
    }
}

void loadSector(uint16_t sector) {
    if(sector != currentSector) {
        if(diskStatus.dirty) {
            flush();
        }
        fetch(sector);
    }
}

int main(int argc, char** argv) {
    diskInit();
    if(diskStatus.mbractive) {
        printf("Good signature\n");
    } else {
        printf("Bad signature!\n");
    }

    for(int i = 0; i < 4; i++) {
        if(diskStatus.mbractive & (1 << i)) {
            printf("Partition %i:\n", i);
            printf("\tStart sector %x, length %x (%u MiB)\n",
                partitions[i].start, partitions[i].length, partitions[i].length/2048);
        }
    }
    int partnum;
    while(1) {
        printf("Select a partition: ");
        scanf("%i", &partnum);
        if(partnum < 0 || partnum >= 4) {        
            printf("Invalid partition number\n");
        } else if(!(diskStatus.partsactive & (1 << partnum))) {
            printf("Partition has unrecognized filesystem\n");
        } else {
            break;
        }
    }
    loadSector(partitions[partnum].start);
    printf("OEM name: %8s\n", sectorBuf + 3);
    int sectorsz = *((uint16_t*) (sectorBuf + 0xB));
    int clustersz = sectorBuf[0xD];
    int reserved = *((uint16_t*) (sectorBuf + 0xE));
    int nfats = sectorBuf[0x10];
    int rootents = *((uint16_t*) (sectorBuf + 0x11)); // unaligned ewwww
    int totalsectors = *((uint16_t*) (sectorBuf + 0x13)); // unaligned ewwww
    int meddesc = sectorBuf[0x15];
    int fatsz = *((uint16_t*) (sectorBuf + 0x16));
    printf("%i\n", fatsz);

    // printf("Contents of FAT:\n");
    // for(int i = 0; i < partitions[partnum].length; i++) {
    //     loadSector(partitions[partnum].start + i);
    //     for(int j = 0; j < SECTOR_SZ; j += 32) {
    //         for(int k = 0; k < 32; k++) {
    //             printf("%.2hhx ", sectorBuf[j + k]);
    //         }
    //         printf("\n");
    //     }
    //     printf("\n");
    // }
}