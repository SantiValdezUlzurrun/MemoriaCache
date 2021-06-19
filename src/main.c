#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <getopt.h>
#include "cache.h"
#include "interprete.h"

#define ERROR_LECTURA -1
#define ERROR_LEC_INT 1
#define MAXIMO_ARCHIVO 100

const char VERSION[] = "1.0.0";

extern char *optarg;
extern int optind, opterr, optopt;


typedef struct config{
	bool overflow;
	bool argumentos_equivocados;
	bool ayuda;
	bool version;
	unsigned int tamanio_cache;
	unsigned int cantidad_vias;
	unsigned int tamanio_bloque;
	char archivo_entrada[MAXIMO_ARCHIVO];
	char archivo_salida[MAXIMO_ARCHIVO];
} config_t;

unsigned int leer_uint(char* string){
	long resultado = strtol(string, NULL, 10);
	if(resultado > UINT_MAX || errno == ERANGE || resultado <= 1){
		return ERROR_LEC_INT;
	}
	return (unsigned int) resultado;
}


void mensaje_ayuda(){
	
	// Uso 
	// ./tp2 -w ways -cs cachesize -bs blocksize [-o <outputprefix>] archivo
	// ./tp2 [-h|-V]
	//
	// Convenciones: https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html
	printf("$ tp2 -h\n");
	printf("Usage:\n");
	printf("  tp2 -h\n");
	printf("  tp2 -V\n");
	printf("  tp2 options archivo\n");
	printf("Options:\n");
	printf("  -h, --help     Imprime este mensaje.\n");
	printf("  -V, --version  Version del programa.\n");
	printf("  -o, --output   Archivo de salida.\n");
	printf("  -w, --ways     Cantidad de vı́as.\n");
	printf("  -c, --cachesize Tamaño del caché en kilobytes.\n");
	printf("  -b, --blocksize Tamaño de bloque en bytes.\n");
	printf("Ejemplos:\n");
	printf("  tp2 -w 4 -cs 8 -bs 16 prueba1.mem\n");
}

void mensaje_version(){
	printf("%s\n", VERSION);
}


config_t leer_argumentos(int argc, char* argv[]){

	static struct option opciones_largas[] = {
		{"help", no_argument, 0, 'h'},
		{"version", no_argument, 0, 'V'},
		{"output", required_argument, 0, 'o'},
		{"ways", required_argument, 0, 'w'},
		{"cachesize", required_argument, 0, 'c'},
		{"blocksize", required_argument, 0, 'b'},
		{0, 0, 0, 0}
	};
	config_t config = {false, false, false, false, 0, 0, 0, "", ""};
	int opt;
	
	while((opt = getopt_long(argc, argv, "o:c:b:w:Vh", opciones_largas, NULL)) != -1 ) {
		
		switch(opt){

			case 'o':	
				strcpy(config.archivo_salida, optarg);
			break;
			
			case 'c':
				if ((config.tamanio_cache = leer_uint(optarg)) == ERROR_LEC_INT) {
					config.overflow = true;
				} 
			break;

			case 'b':
				if ((config.tamanio_bloque = leer_uint(optarg)) == ERROR_LEC_INT) {
					config.overflow = true;
				}
			break;

			case 'w':
				if ((config.cantidad_vias = leer_uint(optarg)) == ERROR_LEC_INT) {
					config.overflow = true;
				}	
			break;

			case 'h':
				config.ayuda = true;
				return config;
			break;

			case 'V':
				config.version = true;	
				return config;
			break;	
		}
	}
	if (argc == 8 || argc == 10) {
		strcpy(config.archivo_entrada, argv[argc - 1]);
	}
	return config;
}

bool config_invalida(config_t config){
	return (config.cantidad_vias == 0 ||
			config.tamanio_bloque == 0 ||
			config.tamanio_cache == 0 ||
			config.overflow ||
			strcmp(config.archivo_entrada, "") == 0);
}

int main(int argc, char* argv[]){		
	
	/* Manejo de la entrada */
	config_t configuracion = leer_argumentos(argc, argv);	

	if(configuracion.ayuda){
		mensaje_ayuda();
		return 0;
	}

	if(configuracion.version){
		mensaje_version();
		return 0;
	}
	
	if (config_invalida(configuracion)) {
		fprintf(stderr,"Error en los argumentos ingresados. Consulta las ayudas con ./tp2 -h\n");
		return ERROR_LECTURA;
	}	
	
	/* Setteando hiper parametros */
	tamanio_cache = configuracion.tamanio_cache;
	tamanio_bloque = configuracion.tamanio_bloque;
	cant_vias = configuracion.cantidad_vias;
	
	/* interprete del cache */
	interprete_t interprete;
	interprete_crear(&interprete,
					 configuracion.archivo_salida,
					 configuracion.archivo_entrada);
	
	/* Se lee el archivo y se ejecuta correctamente */
	init();
	interprete_interpretar(&interprete);
	cache_destruir();
	
	/* Fin de la aplicacion */
	interprete_destruir(&interprete);
	return 0;
}
