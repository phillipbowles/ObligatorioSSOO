#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        
        if (inode_trunc_data(image_path, &file_inode) != 0) {
            fprintf(stderr, "Error al truncar archivo '%s'\n", filename);
            return EXIT_FAILURE;
        }
        
        if (write_inode(image_path, inode_number, &file_inode) != 0) {
            fprintf(stderr, "Error al escribir inodo actualizado para '%s'\n", filename);
            return EXIT_FAILURE;
        }
        
        DEBUG_PRINT("Archivo '%s' truncado exitosamente\n", filename);
    }
    
    return EXIT_SUCCESS;
}