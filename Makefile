# Directorios
SRC_DIR = src
INC_DIR = include

# Compilador y opciones
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I$(INC_DIR)

ifdef DEBUG
CFLAGS += -DDEBUG
endif

# Archivos comunes
COMMON_SRCS = \
	$(SRC_DIR)/read-write-block.c \
	$(SRC_DIR)/bitmap.c \
	$(SRC_DIR)/superblock.c \
	$(SRC_DIR)/rootdir.c \
	$(SRC_DIR)/inode.c \
	$(SRC_DIR)/ls-func.c \
	$(SRC_DIR)/read-write-data.c

COMMON_HDRS = $(INC_DIR)/vfs.h

# Ejecutables
BINS = vfs-mkfs vfs-info vfs-copy vfs-touch vfs-ls vfs-cat vfs-rm vfs-trunc

# Archivos objeto comunes
COMMON_OBJS = $(patsubst $(SRC_DIR)/%.c,$(SRC_DIR)/%.o,$(COMMON_SRCS))

.PHONY: all clean

all: $(BINS)

# Regla para compilar cada ejecutable
$(BINS): %: $(SRC_DIR)/%.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Regla para compilar archivos comunes a objetos
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(COMMON_HDRS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Limpiar
clean:
	rm -f $(SRC_DIR)/*.o $(BINS)
