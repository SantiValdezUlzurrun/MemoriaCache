#include "interprete.h"

/* Para desechar la línea en caso de error */
#define MAX_LINEA 100

bool interprete_crear(interprete_t* interprete, char* archivo_entrada, char* archivo_salida){
	if (!interprete) return ERROR;

	if (!(interprete->archivo_entrada = fopen(archivo_entrada, "r")))
		return ERROR;

	if (strcmp(archivo_salida,"") == 0) { 
		interprete->archivo_salida = stdout;
	}else {
		if(!(interprete->archivo_salida = fopen(archivo_salida, "w")))
			return ERROR;
	}
	return EXITO;
}


void escribir(interprete_t* inter){
	char hit = 0;
	int direccion = 0;
	unsigned char byte = 0;
	fscanf(inter->archivo_entrada, "%i,%hhu\n", &direccion, &byte);
	write_byte(direccion, byte, &hit);
	if (cache.error){
		fprintf(inter->archivo_salida, "Dirreccion: %05i fuera de rango\n", direccion);
		cache.error = false;
	}
	else {
		fprintf(inter->archivo_salida, "Escritura:\tDireccion(%05i)\t\tValor(%hhu)\t%s\n", direccion, byte, hit? "Hit":"Miss");
	}
}


void leer(interprete_t* inter){
	char hit = 0;
	int direccion = 0;
	fscanf(inter->archivo_entrada, "%i\n",&direccion);
	unsigned char byte = read_byte(direccion, &hit);
	if (cache.error){
		fprintf(inter->archivo_salida, "Dirreccion: %05i fuera de rango\n", direccion);
		cache.error = false;
	}
	else {
		fprintf(inter->archivo_salida,"Lectura:\tDireccion(%05i)\t\tValor(%hhu)\t%s\n", direccion, byte, hit?"Hit":"Miss");
	}
}



void interprete_interpretar(interprete_t* inter){
	char instruccion[5] = "";
	int i = 1;
	char tacho[MAX_LINEA];
	while(!feof(inter->archivo_entrada)){
		fscanf(inter->archivo_entrada,"%s", instruccion);
		if(strcmp(instruccion, "init") == 0){
			init();
		}else if(strcmp(instruccion, "MR") == 0){
			int miss_rate = get_miss_rate();
			fprintf(inter->archivo_salida, "Miss rate: %i%%\n", miss_rate);
		}else if (strcmp(instruccion, "R") == 0){
			leer(inter);
		}else if (strcmp(instruccion, "W") == 0){
			escribir(inter);
		}else if(strlen(instruccion) == 0){
		}else{
			fscanf(inter->archivo_entrada, "%[^\n]\n", tacho);
			fprintf(stderr, "Error de sintaxis. Linea: %i\n", i);
		}
		strcpy(instruccion, "");
		i++;
	}
}

bool interprete_destruir(interprete_t* interprete) {
	if (!interprete) return ERROR;

	if (fclose(interprete->archivo_entrada) ||
		fclose(interprete->archivo_salida)) 
		return ERROR;
	
	return EXITO;
}


