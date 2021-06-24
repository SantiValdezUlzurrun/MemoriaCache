#include "cache.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

bool en_rango(unsigned int a_chequear, unsigned int maximo){
	return (a_chequear <= maximo);
}

bool es_addr_valido(unsigned int address){
	return address < TAMANIO_MEMORIA_PRINCIPAL;
}

//Devuelve la cantidad de bits para representar el numero dado
int bits_para_representar(int numero){
	return ceil(log2(numero));
}

void cache_init() {
	cache.inicializada = true;
	cache.hits = 0;
	cache.misses = 0;
	cache.tamanio_cache = tamanio_cache * 1024;
	cache.tamanio_bloque = tamanio_bloque;
	cache.cant_vias = cant_vias;
	cache.bits_index = bits_para_representar(cache.tamanio_cache/(tamanio_bloque*cant_vias));
	cache.bits_offset = bits_para_representar(tamanio_bloque);
	cache.bits_tag = BITS_DIRECCION - (cache.bits_index + cache.bits_offset);
}

void init() {

	if (cache.inicializada) {
		cache_destruir();
	}
	
	cache_init();
	cache.vias = calloc(cant_vias, sizeof(via_t));
	int cantidad_bloques = cache.tamanio_cache/(cant_vias * tamanio_bloque);
	for(int i = 0; i < cant_vias; i++){
		via_init(&(cache.vias[i]), cantidad_bloques, tamanio_bloque);
	}
}

unsigned int find_set(int address){
	unsigned short address_16 = address;
	address_16 = address_16 << cache.bits_tag;
	address_16 = address_16 >> (cache.bits_offset + cache.bits_tag);
	unsigned int valor_retorno = (0xFFFF & address_16);
	return valor_retorno;
	/*
	unsigned short index = address << cache.bits_tag;
  	index = index >> (cache.bits_offset + cache.bits_tag);
  	return index;*/
}

unsigned int find_earliest(int setnum){
    if(!(0 <= setnum && setnum <= cache.tamanio_cache/(cache.cant_vias * cache.tamanio_bloque))) return setnum;
    int indice = 0;

    int ant_actual = cache.vias[0].bloques[setnum].antiguedad;
    bool valido = cache.vias[0].bloques[setnum].valid;
    if(!valido) return 0;

    for (int i = 1; i < cache.cant_vias; i++){
        valido = cache.vias[i].bloques[setnum].valid;
        if(!valido) return i;

        int ant_bloque = cache.vias[i].bloques[setnum].antiguedad;
        if(ant_bloque > ant_actual){
            ant_actual = ant_bloque;
            indice = i;
        }
    }
    return indice;
}

unsigned int get_blocknum(int address){
	return (address >> cache.bits_offset);	
}

unsigned int get_offset(int address){
	unsigned int offset = address & (tamanio_bloque-1);
	return offset;
}

unsigned int get_tag(int address){
	return (address >> (cache.bits_offset + cache.bits_index));
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
		if(bloque->antiguedad != 0){
			bloque->antiguedad++;
		}
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
	block_t* bloque = obtener_bloque_de_cache(get_blocknum(address) << cache.bits_offset);
	return bloque->data[offset];
}

char read_byte(int address, char *hit) {
	if (!es_addr_valido(address)){
		return ' ';
	}
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
	if (!es_addr_valido(address)){
		return ' ';
	}
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
	for(int i = 0; i < cache.cant_vias; i++){
		via_destroy(&cache.vias[i]);
	}
	free(cache.vias);
	cache.inicializada = false;
}

