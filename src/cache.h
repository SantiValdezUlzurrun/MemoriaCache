#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "block.h"

#define TAMANIO_MEMORIA_PRINCIPAL (64*1024)
#define BITS_DIRECCION 16
char memoria_ppal[TAMANIO_MEMORIA_PRINCIPAL];

int tamanio_cache;
int tamanio_bloque;
int cant_vias;


// typedef struct bloque{
// 	bool valido;
// 	bool dirty;
// 	int antiguedad;
// 	unsigned short direccion;
// 	char* datos;
// }bloque_t;

typedef struct via{
	block_t* bloques;
	int cant_bloques;
	int bloque_mas_viejo;
	int* tags;
}via_t;


typedef struct cache{
	bool inicializada;
	int hits;
	int misses;
	via_t* vias;
	int tamanio_cache;
	int tamanio_bloque;
	int cant_vias;
	int bits_tag;
	int bits_index;
	int bits_offset;
}cache_t;

cache_t cache = {false, 0, 0, NULL, 0, 0, 0};

void cache_destruir();

//Debe inicializar los bloques de la caché como inválidos, la memoria 
//simulada en 0 y la tasa de misses a 0.
void init();

//Debe devolver el conjunto de caché al que mapea la dirección address.
unsigned int find_set(int address);

//Debe devolver el bloque más ’antiguo’ dentro de un conjunto, utilizando 
//el campo correspondiente de los metadatos de los bloques del conjunto.
unsigned int find_earliest(int setnum);

//Debe leer el bloque blocknum de memoria y guardarlo en el lugar que le 
//corresponda en la memoria caché.
void read_block(int blocknum);

//Debe escribir el valor value en la memoria.
void write_byte_tomem(int address, char value);

//Debe retornar el valor correspondiente a la posición de memoria address,
//buscándolo primero en el caché, y escribir 1 en *hit si es un hit y 0 si 
//es un miss.
char read_byte(int address, char *hit);

//Debe escribir el valor value en la posición correcta del bloque que 
//corresponde a address, si está en el caché, y en la memoria en todos 
//los casos, y devolver 1 si es un hit y 0 si es un miss.
char write_byte(int address, char value, char *hit);

//Debe devolver el porcentaje de misses desde que se inicializó el cache.
char get_miss_rate();
#endif /* CACHE_H */
