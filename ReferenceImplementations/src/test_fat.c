#include <stdio.h>
#include <stdint.h>

#define SECTOR_SZ 512

#define MBR_PARTITIONS 0x01BE
#define MBR_SIGADDR 0x01FE
#define MBR_SIGNATURE 0xAA55

FILE* disk;
uint8_t sectorBuf[SECTOR_SZ];
void sectorFetch(uint16_t sector) {
    fseek(disk, ((long) sector) * SECTOR_SZ, SEEK_SET);
    fread(sectorBuf, 1, SECTOR_SZ, disk);
}

int main(int argc, char** argv) {
    disk = fopen("exampledisk.img", "rb+");
    // Read MBR
    sectorFetch(0);
    uint16_t sig = *((uint16_t*) &sectorBuf[MBR_SIGADDR]);
    if(sig != MBR_SIGNATURE) {
        printf("Bad signature!\n");
    } else {
        printf("Good signature\n");
    }

    for(int i = 0; i < 4; i++) {
        printf("Partition %i:\n", i);
        uint8_t* partfield = sectorBuf + MBR_PARTITIONS + i*16;
        if(partfield[0] == 0x00) {
            printf("\tInactive, type %hhx\n", partfield[4]);
        } else if(partfield[0] == 0x80) {
            printf("\tActive, type %hhx\n", partfield[4]);
        } else {
            printf("\tInvalid\n");
            continue;
        }
        uint32_t addr = *((uint32_t*) (partfield + 8));
        uint32_t len = *((uint32_t*) (partfield + 0xC));
        printf("\tStart sector %x, length %x (%u MiB)\n", addr, len, len/2048);
    }
}