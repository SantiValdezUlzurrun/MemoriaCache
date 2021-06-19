#ifndef __VIA_H__
#define __VIA_H__

#include <stdlib.h>

#include "block.h"

typedef struct via{
	block_t* bloques;
	int cant_bloques;
	int bloque_mas_viejo;
	int* tags;
}via_t;

void via_init(via_t *via, int cantidad_bloques, int tamanio_bloque);
void via_destroy(via_t *via);

#endif /* __VIA_H__ */