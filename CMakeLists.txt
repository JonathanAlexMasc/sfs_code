cmake_minimum_required(VERSION 3.27)
project(sfs_code C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(sfs_code
        bitmap.c
        bitmap.h
        driver.c
        driver.h
        example.c
        sfs_dir.h
        sfs_inode.h
        sfs_superblock.h
        dils.c
)
