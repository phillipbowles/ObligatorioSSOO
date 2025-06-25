#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vfs.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s imagen\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];
    
    struct superblock sb_struct, *sb = &sb_struct;
    if (read_superblock(image_path, sb) != 0) {
        fprintf(stderr, "Error al leer superblock\n");
        return EXIT_FAILURE;
    }

    struct inode root_inode;
    if (read_inode(image_path, ROOTDIR_INODE, &root_inode) != 0) {
        fprintf(stderr, "Error al leer el inodo del directorio raíz\n");
        return EXIT_FAILURE;
    }

    printf("INOD TYPE PERMS      USER       GROUP      BLKS     SIZE CREATED             MODIFIED            ACCESSED            NAME\n");
    printf("---- ---- ---------- ---------- ---------- ---- -------- ------------------- ------------------- ------------------- ----\n");

    for (uint16_t i = 0; i < root_inode.blocks; i++) {
        int block_num = get_block_number_at(image_path, &root_inode, i);
        if (block_num <= 0) {
            fprintf(stderr, "Error al obtener bloque %d del directorio raíz\n", i);
            return EXIT_FAILURE;
        }

        uint8_t data_buf[BLOCK_SIZE];
        if (read_block(image_path, block_num, data_buf) != 0) {
            fprintf(stderr, "Error al leer bloque %d\n", block_num);
            return EXIT_FAILURE;
        }

        struct dir_entry *entries = (struct dir_entry *)data_buf;

        for (uint32_t j = 0; j < DIR_ENTRIES_PER_BLOCK; j++) {
            if (entries[j].inode == 0) {
                continue;
            }

            struct inode file_inode;
            if (read_inode(image_path, entries[j].inode, &file_inode) != 0) {
                fprintf(stderr, "Error al leer inodo %u\n", entries[j].inode);
                continue;
            }

            print_inode(&file_inode, entries[j].inode, entries[j].name);
        }
    }

    return EXIT_SUCCESS;
}