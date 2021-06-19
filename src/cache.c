#include "cache.h"
#include <stdlib.h>
#include <math.h>



//Devuelve la cantidad de bits para representar el numero dado
int bits_para_representar(int numero){
	return (int) ceil(log2(numero));
}

void init();


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

/*
	Via-0		Via-1		Via-2		Via-3
	| A	|		| A	|		| A	|		| A	|
	| B	|		| B	|		| B	|		| B	|
	| C	|		| C	|		| C	|		| C	|
	| D	|		| D	|		| D	|		| D	|
	
*/