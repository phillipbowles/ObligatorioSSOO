#ifndef VFS_H
#define VFS_H

#ifdef DEBUG
  #define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
  #define DEBUG_PRINT(fmt, ...) ((void)0)
#endif

#include <stdint.h>
#include <stddef.h>

// Número mágico para identificar un filesystem válido
#define MAGIC_NUMBER 0x20250604

// Tamaño de bloque fijo del filesystem (en bytes)
#define BLOCK_SIZE 1024

// Límites para el tamaño del filesystem
#define VFS_MIN_BLOCKS 50     // 50K
#define VFS_MAX_BLOCKS (64*BLOCK_SIZE) // 64M

// Número máximo de bloques del filesystem usados para el bitmap
// Este valor limita el tamaño máximo del filesystem
#define MAX_INODE_BLOCKS 8
#define MAX_VFS_BLOCKS (MAX_INODE_BLOCKS*BLOCK_SIZE*8)

// Modos posibles de un inodo
#define INODE_MODE_FILE 0x8000  // Archivo regular
#define INODE_MODE_DIR  0x4000  // Directorio

#define DEFAULT_PERM 0640       // Permisos por defecto para nuevos archivos

// Longitud máxima del nombre de un archivo o entrada de directorio
#define FILENAME_MAX_LEN 28

// Número de Nodo-I del directorio raiz, se usa el 1, no el 0
// El 0 no se usa, porque se interpreta como entrada libre
#define ROOTDIR_INODE (1)

// Número de bloque del superblock es 0
// Lo declaramos como constante para simplificar la lectura del código
#define SB_BLOCK_NUMBER (0)

// Estructura del superbloque (bloque 0)
struct superblock {
    uint32_t magic;         // Número mágico del filesystem
    uint32_t block_size;    // Tamaño del bloque (siempre BLOCK_SIZE)
    uint32_t total_blocks;  // Cantidad total de bloques en la imagen
    uint32_t superblock_blocks;  // Cantidad total de bloques del superblock en la imagen (siempre 1)
    uint32_t inode_blocks;  // Cantidad total de bloques de inodos en la imagen
    uint32_t bitmap_blocks;  // Cantidad total de bloques de bitmap en la imagen
    uint32_t free_blocks;  // Cantidad total de bloques libres en la imagen
    uint32_t inode_size;   // Tamaño en bytes del inodo
    uint32_t inode_count;   // Cantidad total de inodos
    uint32_t free_inodes;   // Cantidad de inodos sin usar
    uint16_t bitmap_zeroes[MAX_INODE_BLOCKS]; // Cuanto queda libre en bloques de bitmap
    uint32_t inode_start;   // Bloque de inicio de la tabla de inodos
    uint32_t bitmap_start;  // Bloque de inicio del bitmap de bloques de datos
    uint32_t data_start;    // Primer bloque de datos disponible
};

// Inodo: información sobre un archivo o directorio

// Cantidad de punteros de bloque que caben en un bloque indirecto
#define NUM_INDIRECT_PTRS (BLOCK_SIZE / sizeof(uint32_t))

// Cantidad de punteros de bloque en el nodo-I
#define NUM_DIRECT_PTRS 7

struct inode {
    uint16_t mode;          //  2 4 bits de tipo y 12 bits de permisos, estilo Unix
    uint16_t uid;           //  2 UID del propietario
    uint16_t gid;           //  2 GID del grupo
    uint16_t blocks;        //  2 Cantidad de bloques ocupados
    uint32_t size;          //  4 Tamaño en bytes
    uint32_t direct[NUM_DIRECT_PTRS];     // 28 - 7 Punteros directos a bloques de datos
    uint32_t indirect;      //  4 Puntero a un bloque indirecto
    uint32_t atime;         //  4 Último acceso (timestamp Unix)
    uint32_t mtime;         //  4 Última modificación
    uint32_t ctime;         //  4 Creación
    uint8_t reserved[6];    //  8 Espacio reservado para alinear a 64 bytes
};

#define INODE_SIZE (sizeof(struct inode)) // Tamaño del nodo-I
#define INODES_PER_BLOCK (BLOCK_SIZE / INODE_SIZE) // Cantidad de nodos-I en un bloque
#define BITS_PER_BLOCK (BLOCK_SIZE * 8) // Cantidad de bits en un bloque

// Bloque de indirección: contiene punteros a bloques de datos
struct indirect_block {
    uint32_t blocks[NUM_INDIRECT_PTRS];
};

// Entrada en un directorio
struct dir_entry {
    uint32_t inode;                 // 4 - Número de inodo al que apunta el nombre
    char name[FILENAME_MAX_LEN];    // 28 - Nombre del archivo
};

#define DIR_ENTRIES_PER_BLOCK (BLOCK_SIZE / sizeof(struct dir_entry)) // Cantidad de entradas en un bloque

// Funciones

// read-write-block.c
int read_block(const char *image_path, int block_number, void *buffer);
int write_block(const char *image_path, int block_number, const void *buffer);
int create_block_device(const char *image_path, int total_blocks, int block_size);

// superblock.c
int init_superblock(const char *image_path, uint32_t total_blocks, uint32_t total_inodes);
int read_superblock(const char *image_path, struct superblock *sb);
int write_superblock(const char *image_path, struct superblock *sb);
void print_superblock(const struct superblock *sb);

// inode.c
int read_inode(const char *image_path, uint32_t inode_number, struct inode *in);
int write_inode(const char *image_path, uint32_t inode_number, const struct inode *in);
int free_inode(const char *image_path, uint32_t inode_number);
int get_block_number_at(const char *image_path, struct inode *in, uint16_t index);
int create_empty_file_in_free_inode(const char *image_path, uint16_t perms);
int inode_append_block(const char *image_path, struct inode *in, uint32_t new_block_number);
int inode_trunc_data(const char *image_path, struct inode *in);

// read-write-data.c
int inode_read_data(const char *image_path, uint32_t inode_number, void *data_buf, size_t len, size_t offset);
int inode_write_data(const char *image_path, uint32_t inode_number, void *data_buf, size_t len, size_t offset);

// rootdir.c
int create_root_dir(const char *image_path);

// bitmap.c
int bitmap_free_block(const char *image_path, uint32_t block_nbr);
int bitmap_set_first_free(const char *image_path);
void print_bitmap_block(uint8_t *buffer, uint32_t size);

// ls-func.c
const char *str_file_type(uint16_t mode);
const char *str_file_permissions(uint16_t mode);
const char *str_user(uint16_t uid);
const char *str_group(uint16_t gid);
void str_timestamp(uint32_t ts, char *buffer, size_t size);
void print_inode(const struct inode *in, uint32_t inode_nbr, const char *filename);
int name_is_valid(const char *name);
int dir_lookup(const char *image_path, const char *filename);
int add_dir_entry(const char *image_path, const char *filename, uint32_t inode_number);
int remove_dir_entry(const char *image_path, const char *filename);

#endif // VFS_H