#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"

struct file_info {
    uint32_t inode_number;
    char name[FILENAME_MAX_LEN];
    struct inode inode_data;
};

int compare_files(const void *a, const void *b) {
    const struct file_info *fa = (const struct file_info *)a;
    const struct file_info *fb = (const struct file_info *)b;
    return strcmp(fa->name, fb->name);
}

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

    struct file_info *files = malloc(sizeof(struct file_info) * sb->inode_count);
    if (files == NULL) {
        fprintf(stderr, "Error al asignar memoria\n");
        return EXIT_FAILURE;
    }
    
    int file_count = 0;

    for (uint16_t i = 0; i < root_inode.blocks; i++) {
        int block_num = get_block_number_at(image_path, &root_inode, i);
        if (block_num <= 0) {
            fprintf(stderr, "Error al obtener bloque %d del directorio raíz\n", i);
            free(files);
            return EXIT_FAILURE;
        }

        uint8_t data_buf[BLOCK_SIZE];
        if (read_block(image_path, block_num, data_buf) != 0) {
            fprintf(stderr, "Error al leer bloque %d\n", block_num);
            free(files);
            return EXIT_FAILURE;
        }

        struct dir_entry *entries = (struct dir_entry *)data_buf;

        for (uint32_t j = 0; j < DIR_ENTRIES_PER_BLOCK; j++) {
            if (entries[j].inode == 0) {
                continue;
            }

            if (read_inode(image_path, entries[j].inode, &files[file_count].inode_data) != 0) {
                fprintf(stderr, "Error al leer inodo %u\n", entries[j].inode);
                continue;
            }

            files[file_count].inode_number = entries[j].inode;
            strncpy(files[file_count].name, entries[j].name, FILENAME_MAX_LEN);
            file_count++;
        }
    }

    qsort(files, file_count, sizeof(struct file_info), compare_files);

    printf("INOD TYPE PERMS      USER       GROUP      BLKS     SIZE CREATED             MODIFIED            ACCESSED            NAME\n");
    printf("---- ---- ---------- ---------- ---------- ---- -------- ------------------- ------------------- ------------------- ----\n");

    for (int i = 0; i < file_count; i++) {
        print_inode(&files[i].inode_data, files[i].inode_number, files[i].name);
    }

    free(files);
    return EXIT_SUCCESS;
}