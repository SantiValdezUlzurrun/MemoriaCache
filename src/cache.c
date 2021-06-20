#include "cache.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

bool en_rango(unsigned int a_chequear, unsigned int maximo){
	return (0 <= a_chequear && a_chequear <= maximo);
}

//Devuelve la cantidad de bits para representar el numero dado
int bits_para_representar(int numero){
	return (int) ceil(log2(numero));
}

void cache_init() {
	cache.inicializada = true;
	cache.hits = 0;
	cache.misses = 0;
	cache.tamanio_cache = tamanio_cache * 1024;
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
	int cantidad_bloques = cache.tamanio_cache/(cant_vias * tamanio_bloque);
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

		if(ant_bloque > ant_actual){
			ant_actual = ant_bloque;
		}
	}
	return ant_actual;
}

unsigned int get_blocknum(int address){
	return (address >> cache.bits_offset);	
}

unsigned int get_offset(int address){
	unsigned int offset = address << (cache.bits_tag + cache.bits_index);
	return address >> (cache.bits_tag + cache.bits_index);
}

block_t* obtener_bloque_de_cache(int address){
	unsigned int set = find_set(address);
	for(int i = 0; i < cant_vias; i++){
		block_t* bloque = &(cache.vias[i].bloques[set]);
		if((get_blocknum(address) == get_blocknum(bloque->direccion)) && (bloque->valid)){
			return bloque;
		}
	}
	return NULL;
}

bool hay_hit(int address){
	return (obtener_bloque_de_cache(address) != NULL);
}

block_t obtener_bloque_de_memoria(int address) {
	block_t bloque; 
	block_init(&bloque, tamanio_bloque);
	bloque.valid = true;
	bloque.direccion = address;
	memcpy(bloque.data, &memoria_ppal[address], tamanio_bloque * sizeof(char));
	return bloque;
}

void actualizar_antiguedad(int address){
	unsigned int set = find_set(address);
	for (int i = 0; i < cant_vias; i++){
		block_t* bloque = &cache.vias[i].bloques[set];
		bloque->antiguedad++;
	}
}

void read_block(int blocknum) {
	
	int cantidad_bloques_mem_ppal = TAMANIO_MEMORIA_PRINCIPAL/tamanio_bloque;
	if (!en_rango(blocknum, cantidad_bloques_mem_ppal)) return;

	int address_16 = blocknum << cache.bits_offset;
	unsigned int set = find_set(address_16);
	unsigned int posicion_via = find_earliest(set);
	
	block_destroy(&cache.vias[posicion_via].bloques[set]);
	cache.vias[posicion_via].bloques[set] = obtener_bloque_de_memoria(address_16);
	actualizar_antiguedad(address_16);
	cache.vias[posicion_via].bloques[set].antiguedad = 1;
}

void write_byte_tomem(int address, char value) {
	memoria_ppal[address] = value;
}

char read_byte_cache(int address){
	
	unsigned int offset = get_offset(address);
	block_t* bloque = obtener_bloque_de_cache(address);
	return bloque->data[offset];
}

char read_byte(int address, char *hit) {
	
	
	if(!hay_hit(address)){
		read_block(get_blocknum(address));	
		cache.misses++;	
		*hit = 0;
	}else{
		*hit = 1;
		cache.hits++;
	}
	
	return read_byte_cache(address);
}

char write_byte(int address, char value, char *hit) {
	
	if(hay_hit(address)) {
		cache.hits++;
		unsigned int offset = get_offset(address);
		block_t* bloque = obtener_bloque_de_cache(address);
		bloque->data[offset] = value;
		bloque->dirty = true;
		actualizar_antiguedad(address);
		bloque->antiguedad = 1;
		*hit = 1;
	}else {
		cache.misses++;
		*hit = 0;
	}
	write_byte_tomem(address, value);
	return *hit;
}

char get_miss_rate(){
	int accesos = cache.hits + cache.misses;
	if(accesos == 0) return 0;

	return (cache.misses*100)/(accesos);
}

void cache_destruir() {
	for(int i = 0; i < cant_vias; i++){
		via_destroy(&cache.vias[i]);
	}
	free(cache.vias);
	cache.inicializada = false;
}

