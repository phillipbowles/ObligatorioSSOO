#include <stdio.h>
#include <stdlib.h>
#include "vfs.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Uso: %s imagen\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];
    struct inode root_inode;

    // Leer el inode del directorio raíz
    if (read_inode(image_path, ROOTDIR_INODE, &root_inode) != 0) {
        fprintf(stderr, "Error al leer el inode del directorio raíz.\n");
        return EXIT_FAILURE;
    }

    uint8_t buffer[BLOCK_SIZE];
    int bytes_read = inode_read_data(image_path, ROOTDIR_INODE, buffer, sizeof(buffer), 0);
    if (bytes_read <= 0) {
        fprintf(stderr, "Error al leer datos del directorio raíz.\n");
        return EXIT_FAILURE;
    }

    // Definir estructura de entrada de directorio acorde a la especificación del proyecto
    struct dir_entry {
        uint16_t inode;           // generalmente 2 bytes
        char name[FILENAME_MAX_LEN];  // nombre con longitud fija
    };
    struct dir_entry *entries = (struct dir_entry *)buffer;

    uint32_t total_entries = root_inode.size / sizeof(struct dir_entry);
    for (uint32_t i = 0; i < total_entries; i++) {
        if (entries[i].inode != 0) {
            struct inode in;
            if (read_inode(image_path, entries[i].inode, &in) == 0) {
                print_inode(&in, entries[i].inode, entries[i].name);
            } else {
                fprintf(stderr, "Error al leer inode del archivo %s.\n", entries[i].name);
            }
        }
    }

    return EXIT_SUCCESS;
}
