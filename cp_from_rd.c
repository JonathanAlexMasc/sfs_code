//
// Created by Jonathan Mascarenhas on 4/15/24.
//

#include <driver.h>
#include <sfs_superblock.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main()
{
    /* declare a buffer that is the same size as a filesystem block */
    char raw_superblock[128];

    /* Create a pointer to the buffer so that we can treat it as a
       superblock struct. The superblock struct is smaller than a block,
       so we have to do it this way so that the buffer that we read
       blocks into is big enough to hold a complete block. Otherwise the
       driver_read function will overwrite something that should not be
       overwritten. */
    sfs_superblock *super = (sfs_superblock *)raw_superblock;

    /* open the disk image and get it ready to read/write blocks */
    driver_attach_disk_image("initrd", 128);

    int block_number;
    int found_superblock = 0;

    /* Search for the superblock in blocks 0 to 10 */
    for (block_number = 0; block_number <= 10; block_number++)
    {
        /* read the block from the disk image */
        driver_read(super, block_number);

        /* Check if it's the filesystem superblock */
        if (super->fsmagic == VMLARIX_SFS_MAGIC &&
            !strcmp(super->fstypestr, VMLARIX_SFS_TYPESTR))
        {
            printf("Superblock found at block %d!\n", block_number);
            found_superblock = 1;
            break; // Exit loop once superblock is found
        }
    }

    if (!found_superblock)
    {
        printf("Superblock is not found in blocks 0 to 10.\n");
    }

    /* close the disk image */
    driver_detach_disk_image();

    return 0;
}
