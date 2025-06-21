#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Uso: %s imagen archivo1 [archivo2...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];
    uint8_t buffer[BUFFER_SIZE];

    for (int i = 2; i < argc; i++) {
        const char *filename = argv[i];
        int inode_number = dir_lookup(image_path, filename);
        if (inode_number <= 0) {
            printf("Error: El archivo \"%s\" no existe.\n", filename);
            continue;
        }

        struct inode in;
        if (read_inode(image_path, inode_number, &in) != 0) {
            printf("Error al leer el inode del archivo \"%s\".\n", filename);
            continue;
        }

        if ((in.mode & 0xF000) != 0x8000) {
            printf("Error: \"%s\" no es un archivo regular.\n", filename);
            continue;
        }

        size_t offset = 0;
        while (offset < in.size) {
            size_t to_read = BUFFER_SIZE;
            if (in.size - offset < BUFFER_SIZE)
                to_read = in.size - offset;

            int bytes_read = inode_read_data(image_path, inode_number, buffer, to_read, offset);
            if (bytes_read < 0) {
                printf("Error al leer los datos de \"%s\".\n", filename);
                break;
            }

            fwrite(buffer, 1, bytes_read, stdout);
            offset += bytes_read;
        }
        printf("\n");  // Salto de línea tras mostrar cada archivo
    }

    return EXIT_SUCCESS;
}
