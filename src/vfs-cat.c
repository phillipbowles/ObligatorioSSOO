#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vfs.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s imagen archivo1 [archivo2...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];
    
    struct superblock sb_struct, *sb = &sb_struct;
    if (read_superblock(image_path, sb) != 0) {
        fprintf(stderr, "Error al leer superblock\n");
        return EXIT_FAILURE;
    }

    for (int i = 2; i < argc; i++) {
        const char *filename = argv[i];
        
        int inode_number = dir_lookup(image_path, filename);
        if (inode_number <= 0) {
            fprintf(stderr, "Error: archivo '%s' no encontrado\n", filename);
            return EXIT_FAILURE;
        }
        
        struct inode file_inode;
        if (read_inode(image_path, inode_number, &file_inode) != 0) {
            fprintf(stderr, "Error al leer inodo %d del archivo '%s'\n", inode_number, filename);
            return EXIT_FAILURE;
        }
        
        if ((file_inode.mode & INODE_MODE_FILE) != INODE_MODE_FILE) {
            fprintf(stderr, "Error: '%s' no es un archivo regular\n", filename);
            return EXIT_FAILURE;
        }
        
        if (file_inode.size == 0) {
            continue;
        }
        
        uint8_t buffer[BLOCK_SIZE];
        size_t total_read = 0;
        
        while (total_read < file_inode.size) {
            size_t to_read = BLOCK_SIZE;
            if (total_read + to_read > file_inode.size) {
                to_read = file_inode.size - total_read;
            }
            
            int bytes_read = inode_read_data(image_path, inode_number, buffer, to_read, total_read);
            if (bytes_read < 0) {
                fprintf(stderr, "Error al leer datos del archivo '%s'\n", filename);
                return EXIT_FAILURE;
            }
            
            if (write(STDOUT_FILENO, buffer, bytes_read) != bytes_read) {
                fprintf(stderr, "Error al escribir a stdout\n");
                return EXIT_FAILURE;
            }
            
            total_read += bytes_read;
        }
    }
    
    return EXIT_SUCCESS;
}