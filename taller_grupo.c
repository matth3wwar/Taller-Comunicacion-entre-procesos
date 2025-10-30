/******************************************
* Pontificia Universidad Javeriana
* Autores: Mateo David Guerra, Ángel García
* Fecha: 27 de Octubre del 2025
* Materia: Sistemas Operativos
* Tema: Fork Pipe Named
* Descripción:
*******************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int leerArchivo(const char *archivo, int N, int **salida) {
	if (N == 0) {
		printf("\nAlerta de arreglo vacío en archivo '%s'\n", archivo);
		*salida = NULL;
		return 0;
	}

	FILE *f = fopen(archivo, "r");
	if (!f) {
		printf("\nNo se encuentra el archivo con el nombre \"%s\"\n\n", archivo);
		return 0;
	}

	int *arreglo = malloc(N * sizeof(int));
	if (!arreglo) {
		printf("\nNo se pudo inicializar el arreglo del primer archivo\n");
		fclose(f);
		return 0;
	}

	size_t i = 0;
	while (i < N && fscanf(f, "%d", &arreglo[i]) == 1) {
		i++;
	}

	if (ferror(f)) {
		fprintf(stderr, "\nError al leer el archivo: %s\n", archivo);
		free(arreglo);
		fclose(f);
		return 0;
	}

	fclose(f);
	*salida = arreglo;
	return 1;
}

void imprimirArreglo(int *arr, int N) {
	if (arr == NULL) {
		printf("\nArreglo Vacío\n");
	} else {
		printf("\nArreglo de tamaño %d:\n", N);
		for (int i = 0; i < N; i++) {
			char c = i == N - 1 ? ' ' : ',';
			printf(" %d%c", arr[i], c);
		}
		printf("\n");
	}
}

/*
void processOperations(int op) {
        switch (op) {
                case 0:
                        size_t i;
                        a
                        break;
        }
}
*/



int main(char argc, char *argv[]) {

	char *endptr;

	errno = 0;
	long n1 = strtol(argv[1], &endptr, 10);
	if (errno || *endptr != '\0' || n1 < 0) {
		fprintf(stderr, "N1 no es válido (%s)", argv[1]);
		return 0;
	}

	errno = 0;
	long n2 = strtol(argv[3], &endptr, 10);
	if (errno || *endptr != '\0' || n1 < 0) {
		fprintf(stderr, "N1 no es válido (%s)", argv[1]);
		return 0;
	}

	size_t N1 = (size_t)n1;
	size_t N2 = (size_t)n2;

	int *arreglo1 = NULL;
	int *arreglo2 = NULL;

	if (!leerArchivo(argv[2], N1, &arreglo1)) {
		free(arreglo1);
		free(arreglo2);
		return 0;
	}

	if (!leerArchivo(argv[4], N2, &arreglo2)) {
		free(arreglo1);
		free(arreglo2);
		return 0;
	}
	printf("\nArreglo 1:\n");
	imprimirArreglo(arreglo1, N1);

	printf("\nArreglo 2:\n");
	imprimirArreglo(arreglo2, N2);

	pid_t pid = fork();

	//Primer hijo (suma total desde los dos arreglos)
	if (pid == 0) {
		size_t j;
		int sumaT = 0;
		for (j = 0; j < N1; j++) {
			sumaT += arreglo1[j];
		}
		for (j = 0; j < N2; j++) {
			sumaT += arreglo2[j];
		}
		printf("\nSuma T = %d\n", sumaT);
		pid = fork();
	}

	//Segundo hijo (suma B de archivo01)
	if (pid == 0) {
		size_t j;
		int sumaB = 0;
		for (j = 0; j < N2; j++) {
			sumaB += arreglo2[j];
		}
		printf("\nSuma B = %d\n", sumaB);
		pid = fork();
	}

	//Grand hijo (suma A de archivo01)
	if (pid == 0) {
		size_t j;
		int sumaA = 0;
		for (j = 0; j < N1; j++) {
			sumaA += arreglo1[j];
		}
		printf("\nSuma A = %d\n", sumaA);
	}

	printf("\nPID: %d\n", pid);
	return 1;
}
