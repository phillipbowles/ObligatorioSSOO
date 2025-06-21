#include "vfs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <imagen> <archivo1> [archivo2...]\n", argv[0]);
        return 1;
    }

    const char *image_path = argv[1];
    // Recorremos cada archivo que queremos crear
    for (int i = 2; i < argc; ++i) {
        const char *filename = argv[i];

        // 1️⃣ Verificamos si el nombre de archivo es válido
        if (!name_is_valid(filename)) {
            fprintf(stderr, "Error: El nombre de archivo \"%s\" no es válido.\n", filename);
            continue;
        }

        // 2️⃣ Verificamos si el archivo existe
        int inode_num = dir_lookup(image_path, filename);
        if (inode_num != -1) {
            fprintf(stderr, "Error: El archivo \"%s\" ya existe.\n", filename);
            continue;
        }

        // 3️⃣ Crear un inodo vacío para el nuevo archivo
        uint32_t new_inode_num = create_empty_file_in_free_inode(image_path, 0644);
        if (new_inode_num == (uint32_t)-1) {
            fprintf(stderr, "Error: No hay inodos disponibles para \"%s\".\n", filename);
            continue;
        }

        // 4️⃣ Añadir la entrada al directorio raíz
        if (add_dir_entry(image_path, filename, new_inode_num) != 0) {
            fprintf(stderr, "Error al crear la entrada de directorio para \"%s\".\n", filename);
            // Aquí podríamos considerar liberar el inode si falla
            continue;
        }

        printf("Se creó el archivo \"%s\" exitosamente.\n", filename);
    }

    return 0;
}
