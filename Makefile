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