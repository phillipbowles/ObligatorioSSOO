# Directorios
SRC_DIR = src
INC_DIR = include

# Compilador y opciones
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I$(INC_DIR)

ifdef DEBUG
CFLAGS += -DDEBUG
endif

# Archivos comunes (fuentes sin main)
COMMON_SRCS = $(SRC_DIR)/read-write-block.c $(SRC_DIR)/bitmap.c $(SRC_DIR)/superblock.c $(SRC_DIR)/rootdir.c $(SRC_DIR)/inode.c $(SRC_DIR)/ls-func.c $(SRC_DIR)/read-write-data.c
COMMON_HDRS = $(INC_DIR)/vfs.h

# Ejecutables - fuentes con función main
BINS = vfs-mkfs vfs-info vfs-copy vfs-touch vfs-ls vfs-lsort vfs-cat vfs-trunc vfs-rm

# Objetos comunes
COMMON_OBJS = $(COMMON_SRCS:.c=.o)

# Regla por defecto: compilar todos los ejecutables
all: $(BINS)

# Regla para compilar archivos objeto
%.o: %.c $(COMMON_HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Reglas para cada ejecutable
vfs-mkfs: $(SRC_DIR)/vfs-mkfs.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

vfs-info: $(SRC_DIR)/vfs-info.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

vfs-copy: $(SRC_DIR)/vfs-copy.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

vfs-touch: $(SRC_DIR)/vfs-touch.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

vfs-ls: $(SRC_DIR)/vfs-ls.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

vfs-lsort: $(SRC_DIR)/vfs-lsort.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

vfs-cat: $(SRC_DIR)/vfs-cat.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

vfs-trunc: $(SRC_DIR)/vfs-trunc.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

vfs-rm: $(SRC_DIR)/vfs-rm.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Limpiar archivos generados
clean:
	rm -f $(COMMON_OBJS) $(SRC_DIR)/vfs-*.o $(BINS)

# Limpiar solo ejecutables
clean-bins:
	rm -f $(BINS)

# Regla para compilar con debug
debug:
	$(MAKE) DEBUG=1

# Declarar targets que no son archivos
.PHONY: all clean clean-bins debug