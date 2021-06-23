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


void free_colas(size_t iterations){
	for (; iterations > 0; iterations--){
		void (*destruction_f)(void*) = free;
		cola_destruir(cache.fifos[iterations-1], destruction_f);
	}
	free(cache.fifos);
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

	//inicio las colas "fifo"
	printf("N_BLOQUES: %i\n", cantidad_bloques);
	cache.fifos = malloc(cantidad_bloques*(sizeof(cola_t*)));
	if (!&cache.fifos){
		// TODO: Liberar cache
		return ;	
	} 

	for (size_t i = 0; i < cantidad_bloques; i++){
		(cache.fifos)[i] = cola_crear();
		if (!(cache.fifos)[i]){
			free_colas(i);
			// TODO: Liberar cache
			return ;
		}
	}
		printf("FIN_INIT\n");
}


unsigned int find_set(int address){
	unsigned short index = address << cache.bits_tag;
  	index = index >> (cache.bits_offset + cache.bits_tag);
  	return index;
}

unsigned int find_earliest(int setnum){
	cola_t *fifo = (cache.fifos)[setnum];
	unsigned int *via = (unsigned int*) cola_desencolar(fifo);
	unsigned int n_via = *via;
	free(via);
	return n_via;
}

unsigned int get_blocknum(int address){
	return (address >> cache.bits_offset);	
}

unsigned int get_offset(int address){
	unsigned int offset = address << (cache.bits_tag + cache.bits_index);
	return address >> (cache.bits_tag + cache.bits_index);
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

}

void write_byte_tomem(int address, char value) {
	memoria_ppal[address] = value;
}

char read_byte_cache(int address){
	
	unsigned int offset = get_offset(address);
	block_t* bloque = obtener_bloque_de_cache(get_blocknum(address) << cache.bits_offset);
	return bloque->data[offset];
}

void fifo_update(unsigned int set_num, unsigned int n_via){
	unsigned int *via = malloc(sizeof(unsigned int));
	*via = n_via;
	cola_t *fifo = (cache.fifos)[set_num];
	cola_encolar(fifo, via);
	//abria que comunicar error en caso de fallar el malloc
}

char read_byte(int address, char *hit) {

	unsigned int tag = get_tag(address);
	unsigned int index = find_set(address);
	unsigned int offset = get_offset(address);

	printf("READ %i ::::::::::TAG: %i - INDEX: %i - OFFSET - %i\n", address, tag, index, offset);

	int empty_block = -1;
	for (int i = 0; i< cache.cant_vias; i++){
		if (!is_valid(&(cache.vias[i].bloques[index]))){
			empty_block = i;
			continue;
		}  
		if (tag_compare(&(cache.vias[i].bloques[index]), tag)){
			printf("READ HIT \n");
			*hit = 1;
			return read_byte_b(&(cache.vias[i].bloques[index]), offset);
		}//hit
	}//miss
	printf("READ MISS \n");
	*hit = 0;
	cache.misses++;
	unsigned int n_via;

	if (empty_block > 0){
		n_via = empty_block;
	} else{
		n_via = find_earliest(index);
	}
	block_t* block_to_replace = &cache.vias[n_via].bloques[index];
	write_block(block_to_replace, &(memoria_ppal[address]));
	set_tag(block_to_replace, tag);

	fifo_update(index, n_via);

	return read_byte_b(block_to_replace, offset);
}

char write_byte(int address, char value, char *hit) {

	memoria_ppal[address] =  value;
	unsigned int tag = get_tag(address);
	unsigned int index = find_set(address);
	unsigned int offset = get_offset(address);

	printf("WRITE %i ::::::::::TAG: %i - INDEX: %i - OFFSET - %i\n", address, tag, index, offset);

	for (int i = 0; i< cache.cant_vias; i++){
		if (!is_valid(&(cache.vias[i].bloques[index]))){
			continue;
		}  
		if (tag_compare(&(cache.vias[i].bloques[index]), tag)){
			write_byte_b(&(cache.vias[i].bloques[index]), offset, value);
			printf("HIT \n");
			*hit = 1;
			cache.hits++;
			return *hit;
		}//hit
	}//miss
	printf("MISS \n");
	*hit = 0;
	cache.misses++;
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

