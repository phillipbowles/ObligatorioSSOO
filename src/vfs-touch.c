#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vfs.h"

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Uso: %s imagen archivo1 [archivo2...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *image_path = argv[1];

    // Iteramos sobre cada argumento (nombre de archivo) para crear
    for (int i = 2; i < argc; i++) {
        const char *filename = argv[i];

        // Verificamos si el nombre de archivo es válido
        if (!name_is_valid(filename)) {
            printf("Error: El nombre de archivo \"%s\" no es válido.\n", filename);
            continue;
        }

        // Verificamos si el archivo ya existe
        if (dir_lookup(image_path, filename) > 0) {
            printf("Error: El archivo \"%s\" ya existe.\n", filename);
            continue;
        }


        // Crear el nodo-i para el nuevo archivo
        uint32_t inode_number = create_empty_file_in_free_inode(image_path, 0644);
        if (inode_number == 0) {
            printf("Error: No hay inodes disponibles para crear \"%s\".\n", filename);
            continue;
        }

        // Añadir la entrada al directorio raíz
        if (add_dir_entry(image_path, filename, inode_number) != 0) {
            printf("Error: No se pudo añadir la entrada para \"%s\".\n", filename);
            // Aquí podríamos considerar liberar el inode en caso de error
            continue;
        }

        printf("Archivo \"%s\" creado exitosamente.\n", filename);
    }

    return EXIT_SUCCESS;
}
