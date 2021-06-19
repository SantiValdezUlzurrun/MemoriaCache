#ifndef _INTERPRETE_H
#define _INTERPRETE_H

#include <stdio.h>
#include <string.h>

#include "cache.h"

#define ERROR -1
#define EXITO 0

typedef struct intreprete {
	FILE* archivo_entrada;
	FILE* archivo_salida;
} interprete_t;

bool interprete_crear(interprete_t* interprete, char* archivo_entrada, char* archivo_salida);
void interprete_interpretar(interprete_t* interprete);
bool interprete_destruir(interprete_t* interprete);

#endif /* _INTERPRETE_H */
