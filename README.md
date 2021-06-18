# Simulacion de una Memoria Cache

Implementacion de una memoria caché asociativa por conjuntos.

## Aplicacion
Implementacion de una memoria caché asociativa por conjuntos, en que se puedan pasar por parámetro el número de vı́as, la capacidad y el tamaño de bloque. La polı́tica de reemplazo será FIFO y la polı́tica de escrituraserá WT/¬WA. Se asume que el espacio de direcciones es de 16 bits, y hay entonces una memoria principal a simular con un tamaño de 64KB

## Uso
```bash
$ tp2 -h
Usage:
	tp2 -h
	tp2 -V
	tp2 options archivo
Options:
	-h, --help		Imprime ayuda.
	-V, --version	Versión del programa.
	-o, --output	Archivo de salida.
	-w, --ways		Cantidad de vı́as.
	-cs, --cachesize Tama	no del caché en kilobytes.
	-bs, --blocksize Tama	no de bloque en bytes.
Examples:
	tp2 -w 4 -cs 8 -bs 16 prueba1.mem
```
