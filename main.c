/*
 * CI3825 Sistemas de Operación I · Laboratorio · Abr-Jul 2026
 * Ing. Néstor Bueno
 *
 * main.c  —  Orquestador del mini-grep multihilo
 *
 * DEBES COMPLETAR LOS BLOQUES TODO DE ESTE ARCHIVO.
 * También debes completar src/grep.c.
 *
 * Uso esperado al final:
 *   ./grep <palabra> <archivo1> [archivo2 ... archivoN]
 *
 * Flujo que debes implementar:
 *   1. Validar argumentos.
 *   2. Crear el pipe.
 *   3. Hacer fork() para crear el proceso monitor (hijo).
 *   4. El hijo lee del pipe e imprime hasta EOF, luego termina.
 *   5. El padre inicializa Resultado, lanza N hilos (uno por archivo),
 *      espera a que terminen, cierra el pipe y espera al hijo.
 *   6. Imprimir el total de coincidencias y liberar recursos.
 */

#include "include/grep.h"

/* ================================================================== */
/*  ejecutar_monitor                                                    */
/*  Llamada SOLO por el proceso hijo.                                  */
/*  Lee mensajes del pipe y los imprime en stdout hasta recibir EOF.   */
/*                                                                      */
/*  Parámetro: fd_lectura — extremo de lectura del pipe.               */
/* ================================================================== */
static void ejecutar_monitor(int fd_lectura) {

    /* TODO-M (Monitor)
     * ----------------
     * En un bucle, lee bloques del pipe con read() y escríbelos
     * directamente a STDOUT_FILENO con write().
     * El bucle termina cuando read() retorna 0 (EOF) o negativo (error).
     * Al salir del bucle, cierra fd_lectura.
     *
     * Pista:
     *   char buf[MAX_RUTA + MAX_LINEA + 64];
     *   ssize_t n;
     *   while ((n = read(fd_lectura, buf, sizeof(buf))) > 0)
     *       write(STDOUT_FILENO, buf, n);
     *
     * ~5 líneas de código.
     */
    char buf[MAX_RUTA + MAX_LINEA + 64];
    ssize_t n;
    while ((n = read(fd_lectura, buf, sizeof(buf))) > 0){
        write(STDOUT_FILENO, buf, n);
    }
    close(fd_lectura);  
}

/* ================================================================== */
/*  main                                                                */
/* ================================================================== */
int main(int argc, char *argv[]) {

    /* ── TODO-1: Validar argumentos ──────────────────────────────────
     *
     * El programa necesita al menos 3 argumentos:
     *   argv[0] = "./grep"
     *   argv[1] = palabra a buscar
     *   argv[2] = primer archivo  (puede haber más)
     *
     * Si argc < 3, imprime en stderr:
     *   "Uso: ./grep <palabra> <archivo1> [archivo2 ...]\n"
     * y retorna 1.
     *
     * Calcula n_arch = argc - 2  (cantidad de archivos).
     * Si n_arch > MAX_HILOS, imprime error y retorna 1.
     *
     * ~8 líneas de código.
     */

    if (argc < 3){
        fprintf(stderr, "Uso: ./grep <palabra> <archivo1> [archivo2 ...]\n");
        return 1;
    } 
    // const char *palabra = argv[1];
    int         n_arch  = argc - 2;
    if (n_arch > MAX_HILOS){
        fprintf(stderr, "Error: la cantidad de archivos es mayor al máximo número de hilos\n");
        return 1;    
    }


    /* ── TODO-2: Crear el pipe ───────────────────────────────────────
     *
     * Declara:   int fds[2];
     * Llama a:   pipe(fds);
     *
     * Si pipe() retorna -1, usa perror("pipe") y retorna 1.
     * Convención que usarás en todo el programa:
     *   fds[0] = extremo de LECTURA   (lo usa el monitor)
     *   fds[1] = extremo de ESCRITURA (lo usan los hilos)
     *
     * ~4 líneas de código.
     */

    int fds[2];
    if (pipe(fds) == -1){
        perror("pipe");
        return 1;
    }

    /* ── TODO-3: fork() — crear el proceso monitor ───────────────────
     *
     * Llama a fork().
     * Si retorna -1: perror("fork") y retorna 1.
     *
     * PROCESO HIJO  (fork() retorna 0):
     *   - Cierra fds[1]  (el hijo no escribe al pipe)
     *   - Llama a ejecutar_monitor(fds[0])
     *   - Llama a exit(0)
     *
     * PROCESO PADRE (fork() retorna > 0):
     *   - Cierra fds[0]  (el padre no lee del pipe)
     *   - Continúa con el resto de main()
     *
     * ~12 líneas de código.
     */

    pipe(fds);

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    } else if (pid == 0) {
        close(fds[1]);
        ejecutar_monitor(fds[0]);
        exit(0);
    } else {
        close(fds[0]);
    }

    /* ── TODO-4: Inicializar la estructura compartida ────────────────
     *
     * Declara:   Resultado res;
     * Llama a:   grep_init(&res, fds[1]);
     *   (fds[1] es el fd de escritura que los hilos usarán)
     *
     * ~2 líneas de código.
     */

    //TODO-4:
    Resultado res;
    grep_init(&res, fds[1]);

    /* ── TODO-5: Preparar argumentos y lanzar hilos ──────────────────
     *
     * Declara arrays:
     *   pthread_t hilos[MAX_HILOS];
     *   ArgsHilo  args[MAX_HILOS];
     *
     * Para i = 0 .. n_arch-1:
     *   args[i].id_hilo = i;
     *   strncpy(args[i].ruta,    argv[i+2], MAX_RUTA  - 1);
     *   strncpy(args[i].palabra, palabra,   MAX_LINEA - 1);
     *   args[i].res = &res;
     *   pthread_create(&hilos[i], NULL, grep_buscar, &args[i]);
     *     → Si pthread_create falla: perror("pthread_create")
     *       (no es fatal; los demás hilos pueden continuar)
     *
     * ~10 líneas de código.
     */

    //TODO: 5
    const char *palabra = argv[1];
    pthread_t hilos[MAX_HILOS];
    ArgsHilo  args[MAX_HILOS];
    for (int i = 0; i < n_arch; i++){
        args[i].id_hilo = i;
        strncpy(args[i].ruta,    argv[i+2], MAX_RUTA  - 1);
        strncpy(args[i].palabra, palabra,   MAX_LINEA - 1);
        args[i].res = &res;
        int code = pthread_create(&hilos[i], NULL, grep_buscar, &args[i]);
            if(code != 0){
                perror("pthread_create");
            }
        };
      // TODO: 6
    for (int i = 0; i < n_arch; i++){
        pthread_join(hilos[i], NULL);
    }

    //TODO: 7
    close(fds[1]);
    wait(NULL);

    //TODO: 8
    printf("---\nTotal de coincidencias: %d\n", res.total_coincidencias);
    grep_destruir(&res);
    return 0;
    /* ── TODO-6: Esperar a todos los hilos ───────────────────────────
     *
     * Para i = 0 .. n_arch-1:
     *   pthread_join(hilos[i], NULL);
     *
     * ~3 líneas de código.
     */

    /* ── TODO-7: Señalar EOF al monitor y esperar que termine ─────────
     *
     * Cierra fds[1]:
     *   close(fds[1]);
     *   → Esto genera EOF en el extremo de lectura del monitor.
     *   → Si no lo cierras, el monitor espera para siempre (deadlock).
     *
     * Espera al proceso monitor:
     *   wait(NULL);
     *
     * ~2 líneas de código.
     */

    /* ── TODO-8: Resultado final y limpieza ──────────────────────────
     *
     * Imprime:
     *   printf("---\nTotal de coincidencias: %d\n", res.total_coincidencias);
     *
     * Llama a grep_destruir(&res).
     * Retorna 0.
     *
     * ~3 líneas de código.
     */

    /* ← reemplazar cuando implementes TODO-8 */
}
