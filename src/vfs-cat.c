#include "vfs.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <imagen> <archivo1> [archivo2...]\n", argv[0]);
        return 1;
    }
    const char *image_path = argv[1];

    // TODO: implementar mostrar contenido concatenado
    
    printf("vfs-cat: función no implementada aún\n");
    return 0;
}
