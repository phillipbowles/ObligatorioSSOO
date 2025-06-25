#!/bin/bash
# comprehensive_vfs_tests.sh
# Suite completa de pruebas para el sistema VFS

# Colores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Contadores
TESTS_PASSED=0
TESTS_FAILED=0
TOTAL_TESTS=0

# Función para logging
log_test() {
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo -e "${BLUE}[TEST $TOTAL_TESTS]${NC} $1"
}

log_success() {
    TESTS_PASSED=$((TESTS_PASSED + 1))
    echo -e "${GREEN}✓ PASSED:${NC} $1"
}

log_failure() {
    TESTS_FAILED=$((TESTS_FAILED + 1))
    echo -e "${RED}✗ FAILED:${NC} $1"
}

log_info() {
    echo -e "${YELLOW}ℹ INFO:${NC} $1"
}

# Función para verificar si un comando fue exitoso
check_success() {
    if [ $? -eq 0 ]; then
        log_success "$1"
        return 0
    else
        log_failure "$1"
        return 1
    fi
}

# Función para verificar si un comando falló (esperado)
check_failure() {
    if [ $? -ne 0 ]; then
        log_success "$1"
        return 0
    else
        log_failure "$1"
        return 1
    fi
}

# Limpiar archivos previos
cleanup() {
    rm -f test*.img temp*.txt sample*.txt large*.txt
    rm -f archivo*.txt prueba*.txt test*.dat
}

echo -e "${BLUE}===========================================${NC}"
echo -e "${BLUE}    SUITE COMPLETA DE PRUEBAS VFS${NC}"
echo -e "${BLUE}===========================================${NC}"

cleanup

echo -e "\n${YELLOW}=== GRUPO 1: PRUEBAS BÁSICAS VFS-MKFS ===${NC}"

log_test "Crear filesystem básico"
./vfs-mkfs test1.img 100 50 > /dev/null 2>&1
check_success "Filesystem básico creado correctamente"

log_test "Crear filesystem más grande"
./vfs-mkfs test2.img 1000 200 > /dev/null 2>&1
check_success "Filesystem grande creado correctamente"

log_test "Crear filesystem tamaño mínimo"
./vfs-mkfs test3.img 50 16 > /dev/null 2>&1
check_success "Filesystem tamaño mínimo creado"

log_test "Rechazar parámetros inválidos - bloques muy pocos"
./vfs-mkfs test_invalid1.img 10 50 > /dev/null 2>&1
check_failure "Correctamente rechazó bloques insuficientes"

log_test "Rechazar parámetros inválidos - demasiados bloques"
./vfs-mkfs test_invalid2.img 100000 50 > /dev/null 2>&1
check_failure "Correctamente rechazó demasiados bloques"

log_test "Rechazar parámetros inválidos - inodos insuficientes"
./vfs-mkfs test_invalid3.img 100 5 > /dev/null 2>&1
check_failure "Correctamente rechazó inodos insuficientes"

log_test "Rechazar archivo existente"
touch existing_file.img
./vfs-mkfs existing_file.img 100 50 > /dev/null 2>&1
check_failure "Correctamente rechazó archivo existente"
rm -f existing_file.img

echo -e "\n${YELLOW}=== GRUPO 2: PRUEBAS VFS-INFO ===${NC}"

log_test "Mostrar información de filesystem válido"
./vfs-info test1.img > /dev/null 2>&1
check_success "Información mostrada correctamente"

log_test "Rechazar archivo inexistente"
./vfs-info noexiste.img > /dev/null 2>&1
check_failure "Correctamente rechazó archivo inexistente"

log_test "Rechazar archivo inválido"
echo "no es un filesystem" > invalid.img
./vfs-info invalid.img > /dev/null 2>&1
check_failure "Correctamente rechazó filesystem inválido"
rm -f invalid.img

echo -e "\n${YELLOW}=== GRUPO 3: PRUEBAS VFS-LS Y VFS-LSORT ===${NC}"

log_test "Listar directorio vacío inicial"
./vfs-ls test1.img | grep -q "\." && ./vfs-ls test1.img | grep -q "\.\."
check_success "Directorio raíz muestra . y .. correctamente"

log_test "Listar con vfs-lsort (directorio vacío)"
./vfs-lsort test1.img > /dev/null 2>&1
check_success "vfs-lsort funciona en directorio vacío"

log_test "Verificar formato de salida ls"
OUTPUT=$(./vfs-ls test1.img 2>/dev/null)
echo "$OUTPUT" | grep -q "INOD TYPE PERMS"
check_success "Formato de header correcto en vfs-ls"

echo -e "\n${YELLOW}=== GRUPO 4: PRUEBAS VFS-TOUCH ===${NC}"

log_test "Crear archivo simple"
./vfs-touch test1.img archivo1.txt > /dev/null 2>&1
check_success "Archivo simple creado"

log_test "Crear múltiples archivos"
./vfs-touch test1.img archivo2.txt archivo3.txt prueba.dat > /dev/null 2>&1
check_success "Múltiples archivos creados"

log_test "Crear archivo con caracteres válidos"
./vfs-touch test1.img test_123.txt-valid > /dev/null 2>&1
check_success "Archivo con caracteres válidos creado"

log_test "Rechazar archivo existente"
./vfs-touch test1.img archivo1.txt > /dev/null 2>&1
check_failure "Correctamente rechazó archivo existente"

log_test "Rechazar nombre inválido con espacios"
./vfs-touch test1.img "archivo con espacios" > /dev/null 2>&1
check_failure "Correctamente rechazó nombre con espacios"

log_test "Rechazar nombre inválido con caracteres especiales"
./vfs-touch test1.img "archivo@invalid#" > /dev/null 2>&1
check_failure "Correctamente rechazó caracteres especiales"

log_test "Rechazar nombre muy largo"
LONG_NAME=$(printf 'a%.0s' {1..50})
./vfs-touch test1.img "$LONG_NAME" > /dev/null 2>&1
check_failure "Correctamente rechazó nombre muy largo"

echo -e "\n${YELLOW}=== GRUPO 5: PRUEBAS VFS-COPY ===${NC}"

# Crear archivos de prueba
echo "Contenido simple" > temp1.txt
echo -e "Línea 1\nLínea 2\nLínea 3" > temp2.txt
echo "Archivo con caracteres especiales: áéíóú ñÑ" > temp3.txt
seq 1 100 > temp4.txt
dd if=/dev/zero of=temp5.txt bs=1024 count=5 2>/dev/null

log_test "Copiar archivo de texto simple"
./vfs-copy test1.img temp1.txt simple.txt > /dev/null 2>&1
check_success "Archivo de texto copiado"

log_test "Copiar archivo multi-línea"
./vfs-copy test1.img temp2.txt multilinea.txt > /dev/null 2>&1
check_success "Archivo multi-línea copiado"

log_test "Copiar archivo con caracteres especiales"
./vfs-copy test1.img temp3.txt especiales.txt > /dev/null 2>&1
check_success "Archivo con caracteres especiales copiado"

log_test "Copiar archivo con números"
./vfs-copy test1.img temp4.txt numeros.txt > /dev/null 2>&1
check_success "Archivo de números copiado"

log_test "Copiar archivo binario (5KB)"
./vfs-copy test1.img temp5.txt binario.dat > /dev/null 2>&1
check_success "Archivo binario copiado"

log_test "Rechazar archivo origen inexistente"
./vfs-copy test1.img noexiste.txt destino.txt > /dev/null 2>&1
check_failure "Correctamente rechazó archivo origen inexistente"

log_test "Rechazar nombre destino inválido"
./vfs-copy test1.img temp1.txt "nombre inválido" > /dev/null 2>&1
check_failure "Correctamente rechazó nombre destino inválido"

log_test "Rechazar nombre destino existente"
./vfs-copy test1.img temp1.txt simple.txt > /dev/null 2>&1
check_failure "Correctamente rechazó nombre destino existente"

echo -e "\n${YELLOW}=== GRUPO 6: PRUEBAS VFS-CAT ===${NC}"

log_test "Mostrar contenido de archivo simple"
OUTPUT=$(./vfs-cat test1.img simple.txt 2>/dev/null)
echo "$OUTPUT" | grep -q "Contenido simple"
check_success "Contenido de archivo simple mostrado"

log_test "Mostrar contenido de archivo multi-línea"
OUTPUT=$(./vfs-cat test1.img multilinea.txt 2>/dev/null)
echo "$OUTPUT" | grep -q "Línea 1" && echo "$OUTPUT" | grep -q "Línea 3"
check_success "Contenido multi-línea mostrado correctamente"

log_test "Mostrar múltiples archivos concatenados"
OUTPUT=$(./vfs-cat test1.img simple.txt multilinea.txt 2>/dev/null)
echo "$OUTPUT" | grep -q "Contenido simple" && echo "$OUTPUT" | grep -q "Línea 1"
check_success "Múltiples archivos concatenados correctamente"

log_test "Mostrar archivo con números"
OUTPUT=$(./vfs-cat test1.img numeros.txt 2>/dev/null)
echo "$OUTPUT" | grep -q "1" && echo "$OUTPUT" | grep -q "100"
check_success "Archivo de números mostrado"

log_test "Rechazar archivo inexistente"
./vfs-cat test1.img noexiste.txt > /dev/null 2>&1
check_failure "Correctamente rechazó archivo inexistente"

log_test "Mostrar archivo vacío"
./vfs-cat test1.img archivo1.txt > /dev/null 2>&1
check_success "Archivo vacío mostrado sin errores"

echo -e "\n${YELLOW}=== GRUPO 7: PRUEBAS VFS-TRUNC ===${NC}"

log_test "Truncar archivo con contenido"
./vfs-trunc test1.img numeros.txt > /dev/null 2>&1
check_success "Archivo truncado correctamente"

log_test "Verificar que archivo truncado tiene tamaño 0"
OUTPUT=$(./vfs-ls test1.img 2>/dev/null | grep numeros.txt)
echo "$OUTPUT" | grep -q "0.*numeros.txt"
check_success "Archivo truncado tiene tamaño 0"

log_test "Truncar archivo ya vacío"
./vfs-trunc test1.img archivo1.txt > /dev/null 2>&1
check_success "Archivo vacío truncado sin errores"

log_test "Truncar múltiples archivos"
./vfs-trunc test1.img simple.txt multilinea.txt > /dev/null 2>&1
check_success "Múltiples archivos truncados"

log_test "Rechazar archivo inexistente para truncar"
./vfs-trunc test1.img noexiste.txt > /dev/null 2>&1
check_failure "Correctamente rechazó archivo inexistente para truncar"

log_test "Rechazar truncar directorio"
./vfs-trunc test1.img . > /dev/null 2>&1
check_failure "Correctamente rechazó truncar directorio"

echo -e "\n${YELLOW}=== GRUPO 8: PRUEBAS VFS-RM ===${NC}"

log_test "Eliminar archivo simple"
./vfs-rm test1.img archivo2.txt > /dev/null 2>&1
check_success "Archivo simple eliminado"

log_test "Eliminar múltiples archivos"
./vfs-rm test1.img archivo3.txt prueba.dat > /dev/null 2>&1
check_success "Múltiples archivos eliminados"

log_test "Verificar que archivos eliminados no aparecen en ls"
OUTPUT=$(./vfs-ls test1.img 2>/dev/null)
! echo "$OUTPUT" | grep -q "archivo2.txt"
check_success "Archivos eliminados no aparecen en listado"

log_test "Rechazar eliminar archivo inexistente"
./vfs-rm test1.img noexiste.txt > /dev/null 2>&1
check_failure "Correctamente rechazó eliminar archivo inexistente"

log_test "Rechazar eliminar directorio ."
./vfs-rm test1.img . > /dev/null 2>&1
check_failure "Correctamente rechazó eliminar directorio ."

log_test "Rechazar eliminar directorio .."
./vfs-rm test1.img .. > /dev/null 2>&1
check_failure "Correctamente rechazó eliminar directorio .."

echo -e "\n${YELLOW}=== GRUPO 9: PRUEBAS DE INTEGRACIÓN ===${NC}"

log_test "Workflow completo: crear, copiar, listar, leer, truncar, eliminar"
./vfs-mkfs test_integration.img 500 100 > /dev/null 2>&1 && \
echo "Test workflow" > workflow_test.txt && \
./vfs-copy test_integration.img workflow_test.txt workflow.txt > /dev/null 2>&1 && \
./vfs-ls test_integration.img | grep -q workflow.txt && \
./vfs-cat test_integration.img workflow.txt | grep -q "Test workflow" && \
./vfs-trunc test_integration.img workflow.txt > /dev/null 2>&1 && \
./vfs-rm test_integration.img workflow.txt > /dev/null 2>&1
check_success "Workflow completo ejecutado correctamente"

log_test "Crear muchos archivos pequeños"
for i in {1..10}; do
    echo "Archivo $i" > small$i.txt
    ./vfs-copy test2.img small$i.txt file$i.txt > /dev/null 2>&1
done
COUNT=$(./vfs-ls test2.img 2>/dev/null | grep file | wc -l)
[ "$COUNT" -eq 10 ]
check_success "10 archivos pequeños creados y listados"

log_test "Verificar orden alfabético en vfs-lsort"
OUTPUT=$(./vfs-lsort test2.img 2>/dev/null | grep file | head -2)
echo "$OUTPUT" | grep -q "file1.txt" && echo "$OUTPUT" | tail -1 | grep -q "file10.txt"
check_success "Orden alfabético correcto en vfs-lsort"

echo -e "\n${YELLOW}=== GRUPO 10: PRUEBAS DE LÍMITES Y STRESS ===${NC}"

log_test "Crear archivo cerca del límite de tamaño"
dd if=/dev/zero of=large_file.txt bs=1024 count=50 2>/dev/null
./vfs-copy test2.img large_file.txt large.dat > /dev/null 2>&1
check_success "Archivo grande copiado correctamente"

log_test "Llenar filesystem hasta el límite"
for i in {1..20}; do
    dd if=/dev/zero of=fill$i.txt bs=1024 count=10 2>/dev/null
    ./vfs-copy test2.img fill$i.txt fill$i.dat > /dev/null 2>&1
done
# El último debería fallar por falta de espacio
./vfs-copy test2.img large_file.txt shouldfail.dat > /dev/null 2>&1
check_failure "Correctamente detectó falta de espacio"

log_test "Operaciones en filesystem lleno"
./vfs-ls test2.img > /dev/null 2>&1 && \
./vfs-info test2.img > /dev/null 2>&1
check_success "Operaciones de lectura funcionan en filesystem lleno"

echo -e "\n${YELLOW}=== GRUPO 11: PRUEBAS DE ROBUSTEZ ===${NC}"

log_test "Operaciones en filesystem corrupto (magic number incorrecto)"
# Crear copia y corromper
cp test1.img corrupted.img
printf '\x00\x00\x00\x00' | dd of=corrupted.img bs=1 count=4 seek=0 conv=notrunc 2>/dev/null
./vfs-info corrupted.img > /dev/null 2>&1
check_failure "Correctamente detectó filesystem corrupto"

log_test "Manejo de archivos con nombres límite"
NAME_27_CHARS="abcdefghijklmnopqrstuvwxyz1"  # 27 caracteres (límite es 28)
echo "test" > limit_test.txt
./vfs-copy test1.img limit_test.txt "$NAME_27_CHARS" > /dev/null 2>&1
check_success "Nombre de 27 caracteres aceptado"

log_test "Verificar consistencia después de múltiples operaciones"
./vfs-info test1.img > /dev/null 2>&1 && \
./vfs-ls test1.img > /dev/null 2>&1 && \
./vfs-lsort test1.img > /dev/null 2>&1
check_success "Filesystem mantiene consistencia"

echo -e "\n${YELLOW}=== LIMPIEZA ===${NC}"
cleanup
rm -f workflow_test.txt small*.txt large_file.txt fill*.txt limit_test.txt

echo -e "\n${BLUE}===========================================${NC}"
echo -e "${BLUE}           RESUMEN DE PRUEBAS${NC}"
echo -e "${BLUE}===========================================${NC}"
echo -e "Total de pruebas ejecutadas: ${YELLOW}$TOTAL_TESTS${NC}"
echo -e "Pruebas exitosas: ${GREEN}$TESTS_PASSED${NC}"
echo -e "Pruebas fallidas: ${RED}$TESTS_FAILED${NC}"

if [ $TESTS_FAILED -eq 0 ]; then
    echo -e "\n${GREEN}🎉 ¡TODAS LAS PRUEBAS PASARON! 🎉${NC}"
    echo -e "${GREEN}Tu sistema VFS está funcionando correctamente.${NC}"
    exit 0
else
    PERCENTAGE=$((TESTS_PASSED * 100 / TOTAL_TESTS))
    echo -e "\n${YELLOW}⚠️  Algunas pruebas fallaron ⚠️${NC}"
    echo -e "Porcentaje de éxito: ${YELLOW}$PERCENTAGE%${NC}"
    echo -e "${YELLOW}Revisa los errores arriba para debugging.${NC}"
    exit 1
fi