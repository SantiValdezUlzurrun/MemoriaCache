#include "cache.h"
#include <stdlib.h>
#include <math.h>


bloque_t bloque_crear(){
	bloque_t bloque;
	bloque.valido = false;
	bloque.dirty = false;
	bloque.direccion = 0x0;
	bloque.datos = malloc(cache.tamanio_bloque*sizeof(char));
	return bloque;
}




bloque_t* inicializar_bloques(int cant_bloques){
	bloque_t* bloque = malloc(cant_bloques * sizeof(bloque_t));
	if(!bloque) return NULL;
		
	for (int j = 0; j < cant_bloques; j++){
		bloque[j] = bloque_crear();
	}
	return bloque;
}


via_t* inicializar_vias(int cant_vias,int cant_bloques){
	via_t* vias = malloc(cant_vias * sizeof(via_t));
	if(!vias) return NULL;

	for(int i = 0; i < cant_vias; i++){
		vias[i].bloques = inicializar_bloques();
		if(!vias[i].bloques){
			free(vias);
			return NULL;
		}
	}

	vias.cant_bloques = cant_bloques;
	return vias;
}

//Devuelve la cantidad de bits para representar el numero dado
int bits_para_representar(int numero){
	return (int) ceil(log2(numero));
}

void init(){
	if(cache.inicializada) return;

	int tamanio_vias = cache.tamanio_cache/cache.cant_vias;
	int cant_bloques = tamanio_vias/cache.tamanio_bloque;

	cache.bits_tag = bits_para_representar(BITS_DIRECCION - cache.cant_vias - cant_bloques);
	cache.bits_index = bits_para_representar(cache.cant_vias);
	cache.bits_offset = bits_para_representar(cant_bloques);
	cache.hits = 0;
	cache.misses = 0;
	cache.inicializada = true;	

	cache.vias = inicializar_vias(cache.cant_vias ,cant_bloques);
}



unsigned int find_set(int address){}

unsigned int find_earliest(int setnum){}

void read_block(int blocknum){}

void write_byte_tomem(int address, char value){}

char read_byte(int address, char *hit){}

char write_byte(int address, char value, char *hit){}

char get_miss_rate(){
	int accesos = cache.hits + cache.misses;
	if(accesos = 0) return 0;

	return (cache.misses*100)/(accesos);
}

void cache_destruir(){}