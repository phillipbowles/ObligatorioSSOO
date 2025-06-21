#include "vfs.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <imagen>\n", argv[0]);
        return 1;
    }
    const char *image_path = argv[1];

    // TODO: implementar listado de archivos del directorio raíz
    
    printf("vfs-ls: función no implementada aún\n");
    return 0;
}
