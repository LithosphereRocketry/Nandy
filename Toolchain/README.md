## Requirements

* C compiler
* GNU Make
* GNU `mtools`

## Notes on Disk Format

The NANDy code relies on the MBR's "partition type" byte to identify a FAT16
partition. I've observed that formatting very small disks (e.g. the 8MB CF card
I was testing with) doesn't always set this byte directly, and expects the OS to
deduce the filesystem type from the partition contents. Probably for related
reasons, gparted on Linux really doesn't like making tiny FAT16 partitions. I
found that you can reliably get a correctly labeled partition by using mkfs.fat:

```
mkfs.fat -F 16 --mbr=y -s 1 -S 512 /dev/sda
```

-s 1 sets the sectors per cluster to 1, to ensure there are enough clusters to
reliably detect the disk as FAT16. Above a certain size this is no longer
necessary but I have not narrowed down exactly where - in general mkfs will
complain if it thinks the cluster count is insufficient.