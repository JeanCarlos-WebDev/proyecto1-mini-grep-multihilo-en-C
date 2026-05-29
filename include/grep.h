/*
 * CI3825 Sistemas de Operación I · Laboratorio · Abr-Jul 2026
 * Ing. Néstor Bueno
 *
 * grep.h  —  Tipos y prototipos del mini-grep multihilo
 *
 * NO MODIFICAR ESTE ARCHIVO.
 */

#ifndef GREP_H
#define GREP_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* ------------------------------------------------------------------ */
/*  Constantes                                                          */
/* ------------------------------------------------------------------ */

#define MAX_RUTA   512
#define MAX_LINEA  1024
#define MAX_HILOS  16

/* ------------------------------------------------------------------ */
/*  Estructura de resultados compartidos entre hilos                    */
/* ------------------------------------------------------------------ */

typedef struct {
    int             total_coincidencias;   /* proteger con mutex        */
    int             pipe_escritura;        /* fd de escritura del pipe  */
    pthread_mutex_t mutex;
} Resultado;

/* ------------------------------------------------------------------ */
/*  Argumentos que el padre pasa a cada hilo                           */
/* ------------------------------------------------------------------ */

typedef struct {
    int       id_hilo;
    char      ruta[MAX_RUTA];
    char      palabra[MAX_LINEA];
    Resultado *res;
} ArgsHilo;

/* ------------------------------------------------------------------ */
/*  Prototipos — grep.c                                                 */
/* ------------------------------------------------------------------ */

void  grep_init    (Resultado *res, int fd);
void *grep_buscar  (void *arg);
void  grep_destruir(Resultado *res);

#endif /* GREP_H */
