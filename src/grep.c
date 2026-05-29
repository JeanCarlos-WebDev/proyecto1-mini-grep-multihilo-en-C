/*
 * CI3825 Sistemas de Operación I · Laboratorio · Abr-Jul 2026
 * Ing. Néstor Bueno
 *
 * src/grep.c  —  Lógica del hilo buscador
 *
 * Implementa las tres funciones declaradas en include/grep.h.
 * Lee con cuidado los comentarios antes de escribir código.
 *
 * Restricciones:
 *   - No modifiques grep.h.
 *   - Puedes agregar funciones auxiliares estáticas si lo necesitas.
 *   - Compila con:  make
 *   - Prueba con:   make test
 */

#include "../include/grep.h"

/* ================================================================== */
/*  grep_init                                                           */
/* ================================================================== */
void grep_init(Resultado *res, int fd) {
    /*
     * TODO-A  (~3 líneas)
     * -------------------
     * Inicializa los tres campos de *res:
     *
     *   res->total_coincidencias = 0;
     *   res->pipe_escritura      = fd;
     *
     * Inicializa el mutex (elige una de las dos formas):
     *
     *   Opción 1 (macro):
     *     res->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
     *
     *   Opción 2 (función, más explícita):
     *     pthread_mutex_init(&res->mutex, NULL);
     */
}

/* ================================================================== */
/*  grep_buscar                                                         */
/*  Función ejecutada por cada hilo.                                   */
/* ================================================================== */
void *grep_buscar(void *arg) {
    /*
     * TODO-B  (~25 líneas)
     * ---------------------
     * PASO 1 — Cast del argumento:
     *   ArgsHilo *args = (ArgsHilo *)arg;
     *
     * PASO 2 — Abrir el archivo:
     *   FILE *f = fopen(args->ruta, "r");
     *   Si f == NULL:
     *     fprintf(stderr, "[Hilo %d] Error: no se pudo abrir %s\n",
     *             args->id_hilo, args->ruta);
     *     return NULL;
     *
     * PASO 3 — Leer línea por línea:
     *   char linea[MAX_LINEA];
     *   while (fgets(linea, sizeof(linea), f)) {
     *
     * PASO 4 — Detectar coincidencia:
     *     if (strstr(linea, args->palabra) != NULL) {
     *
     * PASO 5 — Quitar el '\n' final:
     *       linea[strcspn(linea, "\n")] = '\0';
     *
     * PASO 6 — Sección crítica (mutex protege contador Y write):
     *       pthread_mutex_lock(&args->res->mutex);
     *
     *           args->res->total_coincidencias++;
     *
     *           char msg[MAX_RUTA + MAX_LINEA + 64];
     *           int  n = snprintf(msg, sizeof(msg),
     *                        "[Hilo %d] %s: %s\n",
     *                        args->id_hilo,
     *                        args->ruta,
     *                        linea);
     *           write(args->res->pipe_escritura, msg, n);
     *
     *       pthread_mutex_unlock(&args->res->mutex);
     *     }  // fin if strstr
     *   }    // fin while fgets
     *
     * PASO 7 — Cerrar y retornar:
     *   fclose(f);
     *   return NULL;
     */
    return NULL; /* ← quitar cuando implementes */
}

/* ================================================================== */
/*  grep_destruir                                                       */
/* ================================================================== */
void grep_destruir(Resultado *res) {
    /*
     * TODO-C  (~1 línea)
     * ------------------
     * Destruye el mutex:
     *   pthread_mutex_destroy(&res->mutex);
     *
     * No uses free(): res vive en el stack de main.c.
     * No cierres el pipe: lo hace main.c en TODO-7.
     */
}
