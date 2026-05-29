#!/usr/bin/env bash
# =============================================================================
# CI3825 Sistemas de Operación I · Laboratorio · Abr-Jul 2026
# tests/evaluar.sh — Corrector automático del Proyecto 1
#
# Valida:
#   1. Línea "Total de coincidencias: N" exacta
#   2. Número de líneas [Hilo X] correcto
#   3. Que cada hilo mencione su archivo asignado
#   4. Mensajes de error en stderr cuando corresponde
#
# El orden entre hilos es no-determinista — el corrector lo tolera.
# =============================================================================

EXEC="./grep"
VERDE="\033[0;32m"
ROJO="\033[0;31m"
AMARILLO="\033[1;33m"
RESET="\033[0m"
NEGRITA="\033[1m"

TOTAL_CASOS=6
APROBADOS=0

get_expected() { grep "^${2}=" "$1" | cut -d'=' -f2-; }

# ─── Función principal ───────────────────────────────────────────────────────
run_caso() {
    local num="$1" desc="$2" cmd="$3"
    local numpad=$(printf "%02d" "$num")
    local expected="tests/caso${numpad}.expected"

    printf "${NEGRITA}[ Caso %s ]${RESET} %s\n" "$num" "$desc"

    if [ ! -f "$EXEC" ]; then
        printf "  ${ROJO}✗ Ejecutable no encontrado. Ejecuta 'make' primero.${RESET}\n\n"
        return
    fi

    SALIDA=$(timeout 5s bash -c "$EXEC $cmd" 2>/tmp/p1_stderr_${num})
    EXIT_CODE=$?

    if [ $EXIT_CODE -eq 124 ]; then
        printf "  ${ROJO}✗ TIMEOUT (5s). Posible deadlock — ¿cerraste fds[1] antes de wait()?${RESET}\n\n"
        return
    fi

    local errores=0

    # Validación 1 — línea Total
    local total_esp total_obt
    total_esp=$(get_expected "$expected" "TOTAL_LINE")
    total_obt=$(echo "$SALIDA" | grep "^Total de coincidencias:")
    if [ "$total_obt" = "$total_esp" ]; then
        printf "  ${VERDE}✓${RESET} %s\n" "$total_esp"
    else
        printf "  ${ROJO}✗ Total incorrecto.${RESET}\n"
        printf "      Esperado : '%s'\n" "$total_esp"
        printf "      Obtenido : '%s'\n" "$total_obt"
        errores=$((errores+1))
    fi

    # Validación 2 — conteo de líneas [Hilo X]
    local count_esp count_obt
    count_esp=$(get_expected "$expected" "COINCIDENCIAS_COUNT")
    if [ -n "$count_esp" ]; then
        count_obt=$(echo "$SALIDA" | grep -c "^\[Hilo" || true)
        if [ "$count_obt" -eq "$count_esp" ]; then
            printf "  ${VERDE}✓${RESET} Líneas [Hilo X]: %d\n" "$count_esp"
        else
            printf "  ${ROJO}✗ Líneas [Hilo X] incorrectas: esperado %d, obtenido %d${RESET}\n" \
                   "$count_esp" "$count_obt"
            errores=$((errores+1))
        fi
    fi

    # Validación 3 — cada hilo menciona su archivo
    for key in $(grep "^HILO_[0-9]*_ARCHIVO=" "$expected" | cut -d'=' -f1); do
        local id_h arch_esp
        id_h=$(echo "$key" | sed 's/HILO_\([0-9]*\)_ARCHIVO/\1/')
        arch_esp=$(get_expected "$expected" "$key")
        if echo "$SALIDA" | grep -q "^\[Hilo ${id_h}\] ${arch_esp}:"; then
            printf "  ${VERDE}✓${RESET} Hilo %d → %s\n" "$id_h" "$arch_esp"
        else
            printf "  ${ROJO}✗ Hilo %d no reportó '%s'${RESET}\n" "$id_h" "$arch_esp"
            printf "      ¿Usas args->ruta en el formato del mensaje?\n"
            errores=$((errores+1))
        fi
    done

    # Validación 4 — stderr con error cuando hay archivo inexistente
    local err_esp
    err_esp=$(get_expected "$expected" "ERROR_HILO")
    if [ -n "$err_esp" ]; then
        if grep -qi "error" "/tmp/p1_stderr_${num}" 2>/dev/null; then
            printf "  ${VERDE}✓${RESET} Error en stderr para archivo inexistente\n"
        else
            printf "  ${ROJO}✗ Falta mensaje de error en stderr${RESET}\n"
            printf "      ¿Imprimes a stderr cuando fopen retorna NULL?\n"
            errores=$((errores+1))
        fi
    fi

    # Resultado
    if [ $errores -eq 0 ]; then
        printf "  ${VERDE}${NEGRITA}→ APROBADO${RESET}\n\n"
        APROBADOS=$((APROBADOS+1))
    else
        printf "  ${ROJO}${NEGRITA}→ FALLIDO (%d error(es))${RESET}\n" "$errores"
        if [ -n "$SALIDA" ]; then
            printf "  ${AMARILLO}Salida completa:${RESET}\n"
            echo "$SALIDA" | sed 's/^/    /'
        fi
        local err_content
        err_content=$(cat "/tmp/p1_stderr_${num}" 2>/dev/null)
        if [ -n "$err_content" ]; then
            printf "  ${AMARILLO}Stderr:${RESET}\n"
            echo "$err_content" | sed 's/^/    /'
        fi
        printf "\n"
    fi
}

# =============================================================================
printf "\n${NEGRITA}============================================================${RESET}\n"
printf "${NEGRITA} CI3825 · Proyecto 1 · Corrector automático${RESET}\n"
printf "${NEGRITA}============================================================${RESET}\n\n"

run_caso "01" "1 hilo · 1 coincidencia"                        \
    "pthread_join tests/datos/hilos.txt"

run_caso "02" "1 hilo · 4 coincidencias"                       \
    "proceso tests/datos/sistemas.txt"

run_caso "03" "2 hilos · mutex compartido · 2 coincidencias"   \
    "mutex tests/datos/sistemas.txt tests/datos/hilos.txt"

run_caso "04" "1 hilo · 0 coincidencias"                       \
    "INEXISTENTE tests/datos/pipes.txt"

run_caso "05" "4 hilos simultáneos · 9 coincidencias"          \
    "proceso tests/datos/sistemas.txt tests/datos/hilos.txt tests/datos/pipes.txt tests/datos/memoria.txt"

run_caso "06" "2 hilos: 1 OK + 1 archivo inexistente"          \
    "pipe tests/datos/pipes.txt tests/datos/NOEXISTE.txt"

# =============================================================================
printf "${NEGRITA}============================================================${RESET}\n"
PUNTOS=$(echo "scale=2; $APROBADOS * 8 / $TOTAL_CASOS" | bc)
printf "${NEGRITA} Resultado: %d / %d casos aprobados${RESET}\n" "$APROBADOS" "$TOTAL_CASOS"
printf "${NEGRITA} Puntos estimados (código): %.2f / 8.00${RESET}\n" "$PUNTOS"
printf "${NEGRITA}============================================================${RESET}\n\n"

if   [ "$APROBADOS" -eq "$TOTAL_CASOS" ]; then
    printf "${VERDE}${NEGRITA}¡Perfecto! Todos los casos pasan. Revisa el informe.${RESET}\n\n"
elif [ "$APROBADOS" -ge 4 ]; then
    printf "${AMARILLO}Buen avance — revisa los casos fallidos arriba.${RESET}\n\n"
else
    printf "${ROJO}Aún hay trabajo. Lee los mensajes con cuidado.${RESET}\n\n"
fi
