#include "cache.h"
#include <stdlib.h>
#include <math.h>



//Devuelve la cantidad de bits para representar el numero dado
int bits_para_representar(int numero){
	return (int) ceil(log2(numero));
}

void cache_init() {
	cache.inicializada = true;
	cache.hits = 0;
	cache.misses = 0;
	cache.tamanio_cache = tamanio_cache;
	cache.tamanio_bloque = tamanio_bloque;
	cache.cant_vias = cant_vias;
	cache.bits_index = bits_para_representar(cant_vias);
	cache.bits_offset = bits_para_representar(tamanio_bloque);
	cache.bits_tag = BITS_DIRECCION - (cache.bits_index + cache.bits_offset);
}

void init() {

	if (cache.inicializada) {
		cache_destruir();
	}
	
	cache_init();
	cache.vias = malloc(cant_vias * sizeof(via_t));
	int cantidad_bloques = tamanio_cache/(cant_vias * tamanio_bloque);
	for(int i = 0; i < cant_vias; i++){
		via_init(&cache.vias[i], cantidad_bloques, tamanio_bloque);
	}	
}


unsigned int find_set(int address){
	unsigned short index = address << cache.bits_tag;
  	index = index >> (cache.bits_offset + cache.bits_tag);
  	return index;
}

unsigned int find_earliest(int setnum){
	if(0 < setnum && setnum < cache.tamanio_cache/(cache.cant_vias * cache.tamanio_bloque)) return setnum;

	int ant_actual = cache.vias[0].bloques[setnum].antiguedad;
	for (int i = 1; i < cache.cant_vias; i++){
		int ant_bloque = cache.vias[i].bloques[setnum].antiguedad;
		if(ant_bloque == 0) return i;

		if(ant_bloque < ant_actual){
			ant_actual = ant_bloque;
		}
	}
	return ant_actual;
}

void read_block(int blocknum){}

void write_byte_tomem(int address, char value){}

char read_byte(int address, char *hit){}

char write_byte(int address, char value, char *hit){}

char get_miss_rate(){
	int accesos = cache.hits + cache.misses;
	if(accesos = 0) return 0;

	return (cache.misses*100)/(accesos);
}


void cache_destruir() {
	for(int i = 0; i < cant_vias; i++){
		via_destroy(&cache.vias[i]);
	}
	free(cache.vias);
	cache.inicializada = false;
}


