#include "via.h"

void via_init(via_t *via, int cantidad_bloques, int tamanio_bloque){
  via -> bloques = malloc(cantidad_bloques * sizeof(block_t));
  for(int i = 0; i < cantidad_bloques; i++){
    block_init(&via->bloques[i], tamanio_bloque);
  }
  via->cant_bloques = cantidad_bloques;
}

void via_destroy(via_t *via) {

  for(int i = 0; i < via->cant_bloques; i++){
		block_destroy(&via->bloques[i]);
	}
	free(via->bloques);
}
