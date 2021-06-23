#include "cola.h"
#include <stdlib.h>
#include <stdio.h>


/* ******************************************************************
 *                    DEFINICÓN DE LAS ESTRUCTURAS
 * *****************************************************************/

typedef struct nodo {
	void* dato;
	struct nodo* next;
} nodo_t;

struct cola {
	nodo_t* primero;
	nodo_t* ultimo;
};


/* ******************************************************************
 *                    FUNCIONES AUXILIARES
 * *****************************************************************/

//Inicializa un nodo
//Post: Se devuelve un nodo con el valor como dato
//devuelve NULL si no se puede inicializar
nodo_t* crear_nodo(void* valor){
	nodo_t* nodo = malloc(sizeof(nodo_t));
	if (!nodo) 
		return NULL;
	nodo->dato = valor;
	nodo->next = NULL;
	return nodo;
}


/* ******************************************************************
 *                    PRIMITIVAS DE LA COLA
 * *****************************************************************/

cola_t* cola_crear(void){
	cola_t* cola = malloc(sizeof(cola_t));
	if (cola == NULL)
		return NULL;
	cola->primero = NULL;
	cola->ultimo = NULL;
	return cola;
}

void cola_destruir(cola_t *cola, void destruir_dato(void*)){	
	while (!cola_esta_vacia(cola)){
		void* dato = cola_desencolar(cola);
		if (destruir_dato)
			destruir_dato(dato);
	}
	free(cola);
}

bool cola_esta_vacia(const cola_t *cola){
	return  cola->ultimo == NULL && cola->primero == NULL;
}

bool cola_encolar(cola_t *cola, void* valor){
	nodo_t* nuevo_nodo = crear_nodo(valor);
	if (!nuevo_nodo)
		return false;
	if (cola_esta_vacia(cola))
		cola->primero = nuevo_nodo;	
	
	else
		cola->ultimo->next = nuevo_nodo;	
	
	cola->ultimo = nuevo_nodo;
	
	return true;
}

void* cola_ver_primero(const cola_t *cola){
	if (cola_esta_vacia(cola))
		return NULL;
	return cola->primero->dato;
}

void* cola_desencolar(cola_t *cola){
	if (cola_esta_vacia(cola))
		return NULL;

	nodo_t* nodo = cola->primero;
	void*  dato = nodo->dato;
	
	if (cola->primero == cola->ultimo){
		cola->primero = NULL;
		cola->ultimo = NULL;
	}
	else{
		cola->primero = cola->primero->next;
	}
	
	free(nodo);
	
	return dato;
}