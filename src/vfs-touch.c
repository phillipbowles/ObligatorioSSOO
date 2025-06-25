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
        
        if (!name_is_valid(filename)) {
            fprintf(stderr, "Error: nombre inválido '%s'\n", filename);
            return EXIT_FAILURE;
        }
        
        if (dir_lookup(image_path, filename) != 0) {
            fprintf(stderr, "Error: el archivo '%s' ya existe\n", filename);
            return EXIT_FAILURE;
        }
        
        int new_inode = create_empty_file_in_free_inode(image_path, DEFAULT_PERM);
        if (new_inode < 0) {
            fprintf(stderr, "Error: no se pudo crear el archivo '%s'\n", filename);
            return EXIT_FAILURE;
        }
        
        if (add_dir_entry(image_path, filename, new_inode) != 0) {
            fprintf(stderr, "Error: no se pudo agregar '%s' al directorio\n", filename);
            free_inode(image_path, new_inode);
            return EXIT_FAILURE;
        }
        
        DEBUG_PRINT("Archivo '%s' creado exitosamente (inode %d)\n", filename, new_inode);
    }
    
    return EXIT_SUCCESS;
}