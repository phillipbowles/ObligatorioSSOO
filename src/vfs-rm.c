#include <stdio.h>
#include <stdlib.h>
#include "vfs.h"

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Uso: %s imagen archivo1 [archivo2...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];

    for (int i = 2; i < argc; i++) {
        const char *filename = argv[i];
        int inode_number = dir_lookup(image_path, filename);
        if (inode_number <= 0) {
            fprintf(stderr, "Error: El archivo \"%s\" no existe.\n", filename);
            continue;
        }

        struct inode in;
        if (read_inode(image_path, inode_number, &in) != 0) {
            fprintf(stderr, "Error al leer el inode de \"%s\".\n", filename);
            continue;
        }

        if ((in.mode & 0xF000) != 0x8000) {
            fprintf(stderr, "Error: \"%s\" no es un archivo regular.\n", filename);
            continue;
        }

        if (inode_trunc_data(image_path, &in) != 0) {
            fprintf(stderr, "Error al truncar \"%s\".\n", filename);
            continue;
        }

        if (free_inode(image_path, inode_number) != 0) {
            fprintf(stderr, "Error al liberar inode de \"%s\".\n", filename);
            continue;
        }

        if (remove_dir_entry(image_path, filename) != 0) {
            fprintf(stderr, "Error al eliminar la entrada de \"%s\".\n", filename);
            continue;
        }

        printf("Archivo \"%s\" eliminado exitosamente.\n", filename);
    }

    return EXIT_SUCCESS;
}
